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
#include "mainview.h"
#include "renderers/beziertoolrenderer.h"
#include "renderers/controlpointsrenderer.h"

namespace Ui {
  class BezierToolView;
}

class BezierToolView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
  Q_OBJECT
  friend class MainWindow;
  MainView* mainView;

  CubicBezier2D bezier;
  int draggedPoint = -1;  // index of point being dragged, -1 if none
  float pointRadius = 0.1f; // for dragging
  float cpDelta = 0.1;
  float cpRadius = 0.05f; // for drawing

  // updating flag
  bool updateAllCPs = true;

  BezierToolRenderer* renderer = nullptr;

  // viewing window
  float left = 0.0f;
  float right = 1.0f;
  float bottom = 0.0f;
  float top = 1.0f;

  int N = 100; //sections of the bezier curve.

  QMatrix4x4 modelMatrix;
  QMatrix4x4 projectionMatrix;
  QMatrix4x4 invTransform;

  Settings settings;

signals:
  void bezierToolChanged(CubicBezier2D bezier);

public:
  Ui::BezierToolView *ui;

  BezierToolView(QWidget *parent = nullptr);
  ~BezierToolView() override;

  void setBezier(CubicBezier2D b) {bezier = b; updateAllCPs = true;}

protected:
  void initializeGL() override;
  void paintGL() override;
  void updateUI();
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