//
// Created by david on 6/8/26.
//

#include "beziertoolview.h"

#include <QDateTime>
#include <QMouseEvent>
#include <QOpenGLVersionFunctionsFactory>

#include "mainwindow.h"

BezierToolView::BezierToolView(QWidget* parent) : QOpenGLWidget(parent)
{
  qDebug() << "BezierToolView constructor";
}

BezierToolView::~BezierToolView()
{
  qDebug() << "BezierToolView destructor";
}

void BezierToolView::initializeGL()
{
  initializeOpenGLFunctions();
  gl =
        QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(
            this->context());

  glClearColor(0.37f, 0.42f, 0.45f, 1.0f);

  ControlPointsRenderer* rend = new ControlPointsRenderer();
  renderer = rend;
  QMatrix4x4 proj;
  QMatrix4x4 model;
  QMatrix3x3 normal;
  proj.ortho(left, right, bottom, top, -1.0f, 1.0f);
  model.setToIdentity();
  normal.setToIdentity();
  rend->setProjTransf(proj);
  rend->setModelTransf(model);
  rend->setNormalTransf(normal);

  //TODO make this somewhat global
  settings.showControlPoints = true;
  rend->init(gl, &settings);

  updateControlPoints();
  rend->updateUniforms();
  rend->updateBuffers();
}

void BezierToolView::paintGL()
{
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderer->updateBuffers();
  renderer->paintGL();
}

void BezierToolView::resizeGL(int w, int h)
{
  QOpenGLWidget::resizeGL(w, h);
}

void BezierToolView::mousePressEvent(QMouseEvent* ev) {
  qDebug() << "Mouse button pressed:" << ev->button();

  QVector2D pos = pixelToGL(ev->pos());
  qDebug() << pos;
  for (int i = 0; i < 4; i++) {
    if ((pos - bezier.getPoint(i)).length() < pointRadius) {
      draggedPoint = i;
      return;
    }
  }
}

void BezierToolView::mouseMoveEvent(QMouseEvent* ev) {
  qDebug() << "x" << ev->position().x() << "y" << ev->position().y();

  if (draggedPoint < 0) return;
  QVector2D glPos = pixelToGL(ev->pos());
  QVector2D newPos;

  switch (draggedPoint)
  {
    case 0:
      newPos.setX(bezier.getP0().x());
      newPos.setY(std::clamp(static_cast<float>(glPos.y()), 0.0f, 1.0f));
      break;
    case 1:
      newPos.setX(std::clamp(static_cast<float>(glPos.x()), bezier.getP0().x() + cpDelta, bezier.getP2().x() - cpDelta));
      newPos.setY(std::clamp(static_cast<float>(glPos.y()), 0.0f, 1.0f));
      break;
    case 2:
      newPos.setX(std::clamp(static_cast<float>(glPos.x()), bezier.getP1().x() + cpDelta, bezier.getP3().x() - cpDelta));
      newPos.setY(std::clamp(static_cast<float>(glPos.y()), 0.0f, 1.0f));
      break;
    case 3:
      newPos.setX(bezier.getP3().x());
      newPos.setY(std::clamp(static_cast<float>(glPos.y()), 0.0f, 1.0f));
      break;
  default:
    return;
  }
  bezier.setPoint(draggedPoint, newPos);
  updateControlPoints();
  renderer->updateUniforms();
  renderer->updateBuffers();
  update();
}

void BezierToolView::mouseReleaseEvent(QMouseEvent* ev) {
  qDebug() << "Mouse button released" << ev->button();
  if (draggedPoint != -1) {bezierToolChanged(bezier);}
  draggedPoint = -1;
}

QVector2D BezierToolView::pixelToGL(QPoint p) const {
  return {
    (right - left) * p.x() / width()  - left,
    top - (top - bottom) * p.y() / height()
  };
}

void BezierToolView::updateControlPoints()
{
  QVector<ControlPoint*> controlPoints;
  for (int i = 0; i < 4; i++)
  {
    QVector3D position = QVector3D(bezier.getPoint(i), 0.0f);
    controlPoints.append(new ControlPoint(position, cpRadius, 10));
  }

  renderer->setControlPoints(controlPoints);
}


