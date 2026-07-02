#include <QDebug>
#include <QElapsedTimer>

#include "mainview.h"

/**
 * @brief MainView::keyPressEvent Triggered by a key press.
 * @param ev Key event.
 */
void MainView::keyPressEvent(QKeyEvent *ev) {
  switch (ev->key()) {
    case 'A':
      qDebug() << "A pressed";
      break;
    case 16777234: // <-
      qDebug() << "left pressed";
      break;
    case 16777235: // ^
      qDebug() << "up pressed";
      break;
    case 16777236: // ->
      qDebug() << "right pressed";
      break;
    case 16777237: // _
      qDebug() << "down pressed";
      break;
    default:
      // ev->key() is an integer. For alpha numeric characters keys it
      // equivalent with the char value ('A' == 65, '1' == 49) Alternatively,
      // you could use Qt Key enums, see http://doc.qt.io/qt-6/qt.html#Key-enum
      qDebug() << ev->key() << "pressed";
      break;
  }
  // Used to update the screen after changes
  update();
}

// Triggered by releasing a key
/**
 * @brief MainView::keyReleaseEvent Triggered by a key released.
 * @param ev Key event.
 */
void MainView::keyReleaseEvent(QKeyEvent *ev) {
  switch (ev->key()) {
    case 'A':
      qDebug() << "A released";
      break;
    default:
      qDebug() << ev->key() << "released";
      break;
  }

  update();
}

/**
 * @brief MainView::mouseDoubleClickEvent Triggered by clicking two subsequent
 * times on any mouse button. It also fires two mousePress and mouseRelease
 * events.
 * @param ev Mouse events.
 */
void MainView::mouseDoubleClickEvent(QMouseEvent *ev) {
  //qDebug() << "Mouse double clicked:" << ev->button();

  if (ev->button() == Qt::LeftButton)
  {
    QVector2D currentMousePosition(ev->position());
    isDoubleDragging = true;
    lastMousePos = currentMousePosition;
    update();
  }
}

/**
 * @brief MainView::mouseMoveEvent Triggered when moving the mouse inside the
 window (only when the mouse is clicked).
 * @param ev Mouse event.
 */
void MainView::mouseMoveEvent(QMouseEvent *ev) {
  //qDebug() << "x" << ev->position().x() << "y" << ev->position().y();

  QVector2D currentMousePos(ev->position());

  if(controlPointPressed == true) {
    // Movement of the control point accordingly to the mouse
    QVector<ControlPoint*>& cps = envelopeControlPoints[selectedEnvelope];
    ControlPoint* controlPoint = cps[selectedControlPoint];
    QVector<ControlPoint*>& orientationcps = envelopeOrientationCPs[selectedEnvelope];
    ControlPoint* orientationControlPoint = orientationcps[selectedControlPoint];
    QVector3D position = controlPoint->getPosition();

    QVector4D clipCoordinates = projTransf * modelTransf * QVector4D(position, 1.0f);
    float ndcZ = clipCoordinates.z() / clipCoordinates.w();
    QVector2D currentScreenPos = toScreenCoordinates(position);
    QVector2D newScreenPos = currentScreenPos + (currentMousePos - lastMousePos);
    QVector3D newWorldPos = toWorldCoordinates(newScreenPos, ndcZ);
    controlPoint->setPosition(newWorldPos);

    // Maintain the distance and direction between a control point and its orientation control point
    for(int i = 0; i<cps.size(); i++){
      float height = envelopes[selectedEnvelope]->getTool()->getHeight();
      orientationcps[i]->setPosition(cps[i]->getPosition() + directioncps[i] * height);
    }

    // Path tangent continuity
    envelopes[selectedEnvelope]->getToolMovement().getPath().ensureContinuityForCps(selectedControlPoint);

    controlPointsRenderers[selectedEnvelope]->updateBuffers();
    orientationCPsRenderers[selectedEnvelope]->updateBuffers();
  } else if(orientationControlPointPressed == true) {
    // Movement of the orientation control point accordingly to the mouse
    QVector<ControlPoint*>& orientationcps = envelopeOrientationCPs[selectedEnvelope];
    ControlPoint* orientationControlPoint = orientationcps[selectedOrientationControlPoint];
    QVector3D position = orientationControlPoint->getPosition();

    QVector4D clipCoordinates = projTransf * modelTransf * QVector4D(position, 1.0f);
    if (clipCoordinates.w() != 0.0f) {
      float ndcZ = clipCoordinates.z() / clipCoordinates.w();
      QVector2D currentScreenPos = toScreenCoordinates(position);
      QVector2D newScreenPos = currentScreenPos + (currentMousePos - lastMousePos);
      QVector3D newWorldPos = toWorldCoordinates(newScreenPos, ndcZ);
      orientationControlPoint->setPosition(newWorldPos);

      // Orientation path continuity
      float height = envelopes[selectedEnvelope]->getTool()->getHeight();
      envelopes[selectedEnvelope]->getToolMovement().getPath().ensureContinuityForOcps(orientationcps, selectedOrientationControlPoint, height);

      controlPointsRenderers[selectedEnvelope]->updateBuffers();
      orientationCPsRenderers[selectedEnvelope]->updateBuffers();
    }
  } else if (isSingleDragging)
  {
    QVector2D delta = currentMousePos - lastMousePos;
    float sensitivity = settings.rotSensitivity;
    QQuaternion deltaRot = QQuaternion::fromEulerAngles(
        delta.y() * sensitivity,  // pitch
        delta.x() * sensitivity,  // yaw
        0.0f
    );
    modelRotation = QMatrix4x4(deltaRot.toRotationMatrix()) * modelRotation;
    updateAllUniforms = true;
  } else if (isDoubleDragging)
  {
    QVector2D delta = currentMousePos - lastMousePos;
    float sensitivity = settings.panSensitivity;
    modelTranslation.translate(delta.x() * sensitivity, -delta.y() * sensitivity, 0.0f);
    updateAllUniforms = true;
  }
  lastMousePos = currentMousePos;

  update();
}

/**
 * @brief MainView::mousePressEvent Triggered when pressing any mouse button.
 * @param ev Mouse event.
 */
void MainView::mousePressEvent(QMouseEvent *ev) {
  //qDebug() << "Mouse button pressed:" << ev->button();

  QVector2D currentMousePos(ev->position());
  if(settings.showControlPoints==true && ev->button() == Qt::LeftButton){
    // Check if a control point is selected
    for(int e = 0; e<envelopes.size(); e++) {
      const QVector<ControlPoint*>& cps = envelopeControlPoints[e];
      directioncps.clear();
      // Save orientation direction at each control point of the envelope e
      for(int i=0; i<cps.size(); i++){
        directioncps.append(envelopes[e]->getToolMovement().getAxisAtCp(i));
      }

      for(int i=0; i<cps.size(); i++){
        QVector3D pos = cps[i]->getPosition();
        QVector2D controlPointScreenPos = toScreenCoordinates(pos);
        if((currentMousePos - controlPointScreenPos).length() <= 20.0f){
          selectedControlPoint = i;
          selectedEnvelope = e;
          controlPointPressed = true;
          lastMousePos = currentMousePos;
          break;
        }
      }
      if (controlPointPressed) {
        break;
      }
    }
  }

  if(settings.showControlPoints==true && ev->button() == Qt::RightButton) {
    // Check if last control point is right-clicked to add a new Bezier curve
    for(int e = 0; e<envelopes.size(); e++) {
      const QVector<ControlPoint*>& cps = envelopeControlPoints[e];
      if(cps.size() > 0) {
        QVector3D pos = cps[cps.size()-1]->getPosition();
        QVector2D controlPointScreenPos = toScreenCoordinates(pos);
        if((currentMousePos - controlPointScreenPos).length() <= 20.0f){
          selectedEnvelope = e;
          addNewBezierCurve = true;
          lastMousePos = currentMousePos;
          break;  
        }
      }
      if (addNewBezierCurve) {
        break;
      }
    }
  }

  if(settings.showOrientationControlPoints==true && ev->button() == Qt::LeftButton){
    // Check if a orientation control point is selected
    for(int e = 0; e<envelopes.size(); e++) {
      const QVector<ControlPoint*>& orientationcps = envelopeOrientationCPs[e];
      for(int i=0; i<orientationcps.size(); i++){
        QVector3D pos = orientationcps[i]->getPosition();
        QVector2D orientationcpScreenPos = toScreenCoordinates(pos);
        if((currentMousePos - orientationcpScreenPos).length() <= 20.0f){
          selectedOrientationControlPoint = i;
          selectedEnvelope = e;
          orientationControlPointPressed = true;
          lastMousePos = currentMousePos;
          break;
        }
      }
      if (orientationControlPointPressed) {
        break;
      }
    }
  }

  if (ev->button() == Qt::LeftButton)
  {
    isSingleDragging = true;
    lastMousePos = currentMousePos;
  }

  update();
  // Do not remove the line below, clicking must focus on this widget!
  this->setFocus();
}

/**
 * @brief MainView::mouseReleaseEvent Triggered when releasing any mouse button.
 * @param ev Mouse event.
 */
void MainView::mouseReleaseEvent(QMouseEvent *ev) {
  //qDebug() << "Mouse button released" << ev->button();
  m_timer.restart();
  m_pendingTimerMeasurement = true;
  qDebug() << "measuring mouse release";


  QVector2D currentMousePos(ev->position());

  if (controlPointPressed == true || orientationControlPointPressed == true || addNewBezierCurve == true) {
    updateMotionAfterInteraction();
    envelopes[selectedEnvelope]->getToolMovement().getPath().updateVertexArr();
    QSet<int> depEnvs = envelopes[selectedEnvelope]->getAllDependents();
    updateToolTransf();
    envelopeMeshUpdates += depEnvs;
    toolMeshUpdates += depEnvs;
    toolTransfUpdates += depEnvs;
    updateAllUniforms = true;
    update();
    selectedControlPoint = -1;
    selectedOrientationControlPoint = -1;
    selectedEnvelope = -1;
    controlPointPressed = false;
    orientationControlPointPressed = false;
    addNewBezierCurve = false;
  } else if (isSingleDragging && ev->button() == Qt::LeftButton)
  {
    isSingleDragging = false;
    lastMousePos = currentMousePos;
    update();
  } else if (isDoubleDragging && ev->button() == Qt::LeftButton)
  {
    isDoubleDragging = false;
    lastMousePos = currentMousePos;
    update();
  }
}

/**
 * @brief MainView::wheelEvent Triggered when clicking scrolling with the scroll
 * wheel on the mouse.
 * @param ev Mouse event.
 */
void MainView::wheelEvent(QWheelEvent *ev) {
  // Implement something
  //qDebug() << "Mouse wheel:" << ev->angleDelta();
  QPoint delta = ev->angleDelta();
  float scroll = delta.y();
  float scale = powf(1.1f, -settings.scaleSensitivity * scroll);
  scalingFactor = qBound(0.1f, scalingFactor*scale, 20.0f);

  updateAllUniforms = true;

  update();
}
