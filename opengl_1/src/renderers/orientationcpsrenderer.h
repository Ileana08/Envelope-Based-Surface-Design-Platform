#ifndef ORIENTATIONCPSRENDERER_H
#define ORIENTATIONCPSRENDERER_H

#include "renderer.h"
#include "../movement/controlpoint.h"

class OrientationCPsRenderer : public Renderer
{
    QVector<ControlPoint*> orientationControlPoints;

    GLuint vaoOrientationControlPoints;
    GLuint vboOrientationControlPoints;

    QVector<ControlPoint*> controlPoints;

    QVector<Vertex> controlLines;
    GLuint vaoControlLines;
    GLuint vboControlLines;

    QVector<Vertex> directionLines;
    GLuint vaoDirectionLines;
    GLuint vboDirectionLines;

    QOpenGLShaderProgram shader;

public:
    OrientationCPsRenderer();
    OrientationCPsRenderer(QVector<ControlPoint*> orientationControlPoints);
    ~OrientationCPsRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers() override;
    void updateUniforms() override;
    void paintGL() override;

    inline void setOrientationControlPoints(QVector<ControlPoint*> orientationControlPoints) { this->orientationControlPoints = orientationControlPoints; }
    inline void setControlPoints(QVector<ControlPoint*> controlPoints) { this->controlPoints = controlPoints; }
    
};

#endif // ORIENTATIONCPSRENDERER_H