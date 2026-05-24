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

  if(controlPointPressed == true && settings.selectedIdx >= 0) {
    QVector<ControlPoint*>& cps = envelopeControlPoints[settings.selectedIdx];
    ControlPoint* controlPoint = cps[selectedControlPoint];
    QVector3D position = controlPoint->getPosition();

    QVector4D clipCoordinates = projTransf * modelTransf * QVector4D(position, 1.0f);
    if (clipCoordinates.w() != 0.0f) {
      float ndcZ = clipCoordinates.z() / clipCoordinates.w();
      QVector2D currentScreenPos = toNormalizedScreenCoordinates(position);
      QVector2D newScreenPos = currentScreenPos + (currentMousePos - lastMousePos);
      QVector3D newWorldPos = toNormalizedWorldCoordinates(newScreenPos, ndcZ);
      controlPoint->setPosition(newWorldPos);
      updateAllUniforms = true;
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
    controlPointsRenderers[settings.selectedIdx]->updateBuffers();
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
  if(settings.showControlPoints==true && ev->button() == Qt::LeftButton && settings.selectedIdx >= 0){
    const QVector<ControlPoint*>& cps = envelopeControlPoints[settings.selectedIdx];
    for(int i=0; i<cps.size(); i++){
      QVector3D pos = cps[i]->getPosition();
      QVector2D controlPointScreenPos = toNormalizedScreenCoordinates(pos);
      if((currentMousePos - controlPointScreenPos).length() <= 20.0f){
        selectedControlPoint = i;
        controlPointPressed = true;
        lastMousePos = currentMousePos;
        break;
      }
    }
  } else if (ev->button() == Qt::LeftButton)
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

  if (settings.selectedIdx >= 0 && envelopes[settings.selectedIdx]) {
    envelopes[settings.selectedIdx]->getToolMovement().getPath().updateVertexArr();
    envelopes[settings.selectedIdx]->update();
    QSet<int> depEnvs = envelopes[settings.selectedIdx]->getAllDependents();
    moveRenderers[settings.selectedIdx]->updateBuffers();
    envelopeRenderers[settings.selectedIdx]->updateBuffers();
    toolRenderers[settings.selectedIdx]->updateBuffers();
    for(int idx : depEnvs) {
      envelopes[idx]->getToolMovement().getPath().updateVertexArr();
      envelopes[idx]->update();
      moveRenderers[idx]->updateBuffers();
      envelopeRenderers[idx]->updateBuffers();
      toolRenderers[idx]->updateBuffers();
    }
    updateToolTransf();
    updateAllUniforms = true;
    selectedControlPoint = -1;
    controlPointPressed = false;
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
