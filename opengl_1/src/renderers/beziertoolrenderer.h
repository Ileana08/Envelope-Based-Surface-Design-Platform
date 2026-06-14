//
// Created by david on 6/9/26.
//

#ifndef OPENGL_1_BEZIERTOOLRENDERER_H
#define OPENGL_1_BEZIERTOOLRENDERER_H
#include <QOpenGLShaderProgram>

#include "CubicBezier2D.h"
#include "renderer.h"
#include "movement/controlpoint.h"


class BezierToolRenderer : public Renderer
{

    QVector<ControlPoint*> controlPoints;
    CubicBezier2D bezier;

    GLuint vaoControlPoints;
    GLuint vboControlPoints;

    QVector<Vertex> controlLines;
    GLuint vaoControlLines;
    GLuint vboControlLines;

    QVector<Vertex> bezierCurve;
    GLuint vaoBezierCurve;
    GLuint vboBezierCurve;
    int N = 100;

    QOpenGLShaderProgram shader;

  public:
    BezierToolRenderer();
    BezierToolRenderer(QVector<ControlPoint*> controlPoints);
    ~BezierToolRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers() override;
    void updateUniforms() override;
    void paintGL() override;

    inline void setControlPoints(QVector<ControlPoint*> controlPoints) { this->controlPoints = controlPoints; }
    inline void setBezier(CubicBezier2D bezier) { this->bezier = bezier; }

};


#endif //OPENGL_1_BEZIERTOOLRENDERER_H