#include <QDebug>

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
  qDebug() << "Mouse double clicked:" << ev->button();

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
  qDebug() << "x" << ev->position().x() << "y" << ev->position().y();

  QVector2D currentMousePos(ev->position());

  if(controlPointPressed == true) {
    // movement of the control point accordingly to the mouse
    QVector<ControlPoint*>& cps = envelopeControlPoints[selectedEnvelope];
    ControlPoint* controlPoint = cps[selectedControlPoint];
    QVector3D position = controlPoint->getPosition();

    QVector4D clipCoordinates = projTransf * modelTransf * QVector4D(position, 1.0f);
    if (clipCoordinates.w() != 0.0f) {
      float ndcZ = clipCoordinates.z() / clipCoordinates.w();
      QVector2D currentScreenPos = toNormalizedScreenCoordinates(position);
      QVector2D newScreenPos = currentScreenPos + (currentMousePos - lastMousePos);
      QVector3D newWorldPos = toNormalizedWorldCoordinates(newScreenPos, ndcZ);
      controlPoint->setPosition(newWorldPos);
      controlPointsRenderers[selectedEnvelope]->updateBuffers();
    }
  } else if(orientationControlPointPressed == true) {
    // movement of the orientation control point accordingly to the mouse
    QVector<ControlPoint*>& orientationcps = envelopeOrientationCPs[selectedEnvelope];
    ControlPoint* orientationControlPoint = orientationcps[selectedOrientationControlPoint];
    QVector3D position = orientationControlPoint->getPosition();

    QVector4D clipCoordinates = projTransf * modelTransf * QVector4D(position, 1.0f);
    if (clipCoordinates.w() != 0.0f) {
      float ndcZ = clipCoordinates.z() / clipCoordinates.w();
      QVector2D currentScreenPos = toNormalizedScreenCoordinates(position);
      QVector2D newScreenPos = currentScreenPos + (currentMousePos - lastMousePos);
      QVector3D newWorldPos = toNormalizedWorldCoordinates(newScreenPos, ndcZ);
      orientationControlPoint->setPosition(newWorldPos);
      orientationcps[1]->setPosition(1/3.0f * (2 * orientationcps[0]->getPosition() + orientationcps[3]->getPosition()));
      orientationcps[2]->setPosition(1/3.0f * (orientationcps[1]->getPosition() + 2 * orientationcps[3]->getPosition()));
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
  qDebug() << "Mouse button pressed:" << ev->button();

  QVector2D currentMousePos(ev->position());
  // if control points are shown and one of them is selected
  if(settings.showControlPoints==true && ev->button() == Qt::LeftButton){
    for(int e = 0; e<envelopes.size(); e++) {
      const QVector<ControlPoint*>& cps = envelopeControlPoints[e];
      for(int i=0; i<cps.size(); i++){
        QVector3D pos = cps[i]->getPosition();
        QVector2D controlPointScreenPos = toNormalizedScreenCoordinates(pos);
        if((currentMousePos - controlPointScreenPos).length() <= 10.0f){
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

  if(settings.showOrientationControlPoints==true && ev->button() == Qt::LeftButton){
    for(int e = 0; e<envelopes.size(); e++) {
      const QVector<ControlPoint*>& orientationcps = envelopeOrientationCPs[e];
      for(int i=0; i<orientationcps.size(); i++){
        if(i != 0 && i!=orientationcps.size()-1) continue;
        QVector3D pos = orientationcps[i]->getPosition();
        QVector2D orientationcpScreenPos = toNormalizedScreenCoordinates(pos);
        if((currentMousePos - orientationcpScreenPos).length() <= 10.0f){
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
  qDebug() << "Mouse button released" << ev->button();

  QVector2D currentMousePos(ev->position());

  if (controlPointPressed == true || orientationControlPointPressed == true) {
    // update everything based on the new position of the control 
    envelopes[selectedEnvelope]->getToolMovement().getPath().updateVertexArr();
    //envelopes[selectedEnvelope]->update();
    QSet<int> depEnvs = envelopes[selectedEnvelope]->getAllDependents();
    //moveRenderers[selectedEnvelope]->updateBuffers();
    //envelopeRenderers[selectedEnvelope]->updateBuffers();
    //toolRenderers[selectedEnvelope]->updateBuffers();
    // for(int idx : depEnvs) {
    //   envelopes[idx]->getToolMovement().getPath().updateVertexArr();
    //   envelopes[idx]->update();
    //   moveRenderers[idx]->updateBuffers();
    //   envelopeRenderers[idx]->updateBuffers();
    //   toolRenderers[idx]->updateBuffers();
    // }
    updateToolTransf();
    envelopeMeshUpdates += depEnvs;
    toolMeshUpdates += depEnvs;
    toolTransfUpdates += depEnvs;
    updateAllUniforms = true;
    update();
    // the mouse selection of the control point is released
    selectedControlPoint = -1;
    selectedOrientationControlPoint = -1;
    selectedEnvelope = -1;
    controlPointPressed = false;
    orientationControlPointPressed = false;
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
  qDebug() << "Mouse wheel:" << ev->angleDelta();
  QPoint delta = ev->angleDelta();
  float scroll = delta.y();
  float scale = powf(1.1f, -settings.scaleSensitivity * scroll);
  scalingFactor = qBound(0.1f, scalingFactor*scale, 20.0f);

  updateAllUniforms = true;

  update();
}
