//
// Created by david on 6/8/26.
//

#include "beziertoolview.h"

#include <complex>
#include <QDateTime>
#include <QMouseEvent>
#include <QOpenGLVersionFunctionsFactory>
#include <QDebug>
#include <QElapsedTimer>

#include "mainwindow.h"
#include "renderers/beziertoolrenderer.h"

BezierToolView::BezierToolView(QWidget* parent) : QOpenGLWidget(parent), mainView(nullptr)
{
  bezier = CubicBezier2D();
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

  BezierToolRenderer* rend = new BezierToolRenderer();
  renderer = rend;
  rend->setBezier(bezier);
  QMatrix3x3 normal;
  projectionMatrix.ortho(left, right, bottom, top, -1.0f, 1.0f);
  modelMatrix.setToIdentity();

  QVector3D center = QVector3D((left + right) / 2.0f, (bottom + top) / 2.0f, 0.0f);
  modelMatrix.translate(center.x(), center.y(), center.z());
  modelMatrix.rotate(90.0f, 0.0f, 0.0f, 1.0f);
  modelMatrix.translate(-center.x(), -center.y(), -center.z());
  invTransform = (projectionMatrix * modelMatrix).inverted();

  normal = modelMatrix.normalMatrix();
  rend->setProjTransf(projectionMatrix);
  rend->setModelTransf(modelMatrix);
  rend->setNormalTransf(normal);

  //TODO make this somewhat global or give this window its own settings
  settings.showControlPoints = true;
  rend->init(gl, &settings);

  updateAllCPs = true;
  rend->updateUniforms();
  rend->updateBuffers();
}

void BezierToolView::paintGL()
{
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (updateAllCPs)
  {
    updateControlPoints();
  }

  renderer->updateUniforms();
  renderer->updateBuffers();
  renderer->paintGL();
}

void BezierToolView::resizeGL(int w, int h)
{
  QOpenGLWidget::resizeGL(w, h);
}

void BezierToolView::mousePressEvent(QMouseEvent* ev) {
  //qDebug() << "Mouse button pressed:" << ev->button();

  QVector2D pos = pixelToGL(ev->pos());
  for (int i = 0; i < 4; i++) {
    if ((pos - bezier.getPoint(i)).length() < pointRadius) {
      draggedPoint = i;
      return;
    }
  }
}

void BezierToolView::mouseMoveEvent(QMouseEvent* ev) {
  //qDebug() << "x" << ev->position().x() << "y" << ev->position().y();

  if (draggedPoint < 0) return;
  QVector2D glPos = pixelToGL(ev->pos());
  QVector2D newPos;

  switch (draggedPoint)
  {
    case 0:
      newPos.setX(bezier.getP0().x());
      newPos.setY(std::clamp(glPos.y(), cpDelta, 1.0f));
      break;
    case 1:
      newPos.setX(std::clamp(glPos.x(), bezier.getP0().x() + cpDelta, bezier.getP3().x() - cpDelta));
      newPos.setY(std::clamp(glPos.y(), 0.0f, 1.0f));
      break;
    case 2:
      newPos.setX(std::clamp(glPos.x(), bezier.getP0().x() + cpDelta, bezier.getP3().x() - cpDelta));
      newPos.setY(std::clamp(glPos.y(), 0.0f, 1.0f));
      break;
    case 3:
      newPos.setX(bezier.getP3().x());
      newPos.setY(std::clamp(glPos.y(), cpDelta, 1.0f));
      break;
  default:
    return;
  }
  bezier.setPoint(draggedPoint, newPos);
  updateAllCPs = true;
  update();
}

void BezierToolView::mouseReleaseEvent(QMouseEvent* ev) {
  //qDebug() << "Mouse button released" << ev->button();
  // mainView->m_pendingTimerMeasurement = true;
  // mainView->m_timer.restart();
  // qDebug() << "Measuring envelope attachment";

  if (draggedPoint != -1) {bezierToolChanged(bezier);}
  draggedPoint = -1;
}

/**
 * Translates widget space to the world space.
 * @param p 2d point in widget space
 * @return world space coordinates
 */
QVector2D BezierToolView::pixelToGL(QPoint p) const {
  float ndcX = 2.0f * p.x() / width()  - 1.0f;
  float ndcY = 1.0f - 2.0f * p.y() / height();
  QVector4D clip(ndcX, ndcY, 0.0f, 1.0f);
  QVector4D worldPos = invTransform * clip;
  return worldPos.toVector2D();
}

/**
 * Updates the control point objects with the values from the Bezier curve.
 */
void BezierToolView::updateControlPoints()
{
  if (!renderer) {
    qDebug() << "called too early!";
    return;
  }

  for (int i = 0; i < 4; i++)
  {
    renderer->updateControlPoint(i, bezier.getPoint(i));
  }
}


