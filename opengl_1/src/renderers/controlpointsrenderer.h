#ifndef CONTROLPOINTSRENDERER_H
#define CONTROLPOINTSRENDERER_H

#include "CubicBezier2D.h"
#include "renderer.h"
#include "../movement/controlpoint.h"

/**
 * @brief The ControlPointsRenderer class is a renderer for the control points of the tool.
 */
class ControlPointsRenderer : public Renderer
{
    QVector<ControlPoint*> controlPoints;

    GLuint vaoControlPoints;
    GLuint vboControlPoints;

    QVector<Vertex> controlLines;
    GLuint vaoControlLines;
    GLuint vboControlLines;

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