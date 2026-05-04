#ifndef CONTROLPOINTSRENDERER_H
#define CONTROLPOINTSRENDERER_H

#include "renderer.h"
#include "../movement/controlpoint.h"

class ControlPointsRenderer : public Renderer
{
    QVector<ControlPoint*> controlPoints;

    GLuint vaoControlPoints;
    GLuint vboControlPoints;

    QOpenGLShaderProgram shader;

public:
    ControlPointsRenderer();
    ControlPointsRenderer(QVector<ControlPoint*> controlPoints);
    ~ControlPointsRenderer();

    void initShaders() override;
    void initBuffers() override;
    void updateBuffers() override;
    void updateUniforms() override;
    void paintGL() override;

    inline void setControlPoints(QVector<ControlPoint*> controlPoints) { this->controlPoints = controlPoints; }
    
};

#endif // CONTROLPOINTSRENDERER_H