#ifndef ORIENTATIONCPSRENDERER_H
#define ORIENTATIONCPSRENDERER_H

#include "renderer.h"
#include "../movement/orientationcontrolpoint.h"

class OrientationCPsRenderer : public Renderer
{
    QVector<OrientationControlPoint*> orientationControlPoints;

    GLuint vaoOrientationControlPoints;
    GLuint vboOrientationControlPoints;

    QVector<Vertex> controlLines;
    GLuint vaoControlLines;
    GLuint vboControlLines;

    QOpenGLShaderProgram shader;

public:
    OrientationCPsRenderer();
    OrientationCPsRenderer(QVector<OrientationControlPoint*> orientationControlPoints);
    ~OrientationCPsRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers() override;
    void updateUniforms() override;
    void paintGL() override;

    inline void setOrientationControlPoints(QVector<OrientationControlPoint*> orientationControlPoints) { this->orientationControlPoints = orientationControlPoints; }
    
};

#endif // ORIENTATIONCPSRENDERER_H