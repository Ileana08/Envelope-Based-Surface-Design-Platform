//
// Created by david on 6/8/26.
//

#ifndef BEZIERTOOLVIEW_H
#define BEZIERTOOLVIEW_H
#include <QObject>
#include <qobjectdefs.h>
#include <qopenglcontext.h>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>
#include <qtmetamacros.h>

#include "CubicBezier2D.h"
#include "renderers/beziertoolrenderer.h"
#include "renderers/controlpointsrenderer.h"


class BezierToolView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
  Q_OBJECT
  friend class MainWindow;

  CubicBezier2D bezier;
  int draggedPoint = -1;  // index of point being dragged, -1 if none
  float pointRadius = 0.1f;
  float cpDelta = 0.05;
  float cpRadius = 0.05f;

  BezierToolRenderer* renderer;

  float left = 0.0f;
  float right = 1.0f;
  float bottom = 0.0f;
  float top = 1.0f;

  int N = 100; //sections of the bezier curve.

  Settings settings;

signals:
  void bezierToolChanged(CubicBezier2D bezier);

public:
  BezierToolView(QWidget *parent = nullptr);
  ~BezierToolView() override;

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;

  // For dragging
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  QVector2D pixelToGL(QPoint p) const;
  QVector2D* pointByIndex(int i);
  void updateControlPoints();

  QOpenGLFunctions_4_1_Core *gl;
};


#endif // BEZIERTOOLVIEW_H