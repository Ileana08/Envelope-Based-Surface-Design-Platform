#include "controlpointsrenderer.h"

/**
 * @brief ControlPointsRenderer::ControlPointsRenderer Creates new control points.
 */
ControlPointsRenderer::ControlPointsRenderer() : controlPoints() {}

/**
 * @brief ControlPointsRenderer::ControlPointsRenderer Creates new control points renderer 
 * with the control points.
 * @param controlPoints List of control points.
 */
ControlPointsRenderer::ControlPointsRenderer(QVector<ControlPoint*> controlPoints) : 
    controlPoints(controlPoints) 
{}

/**
 * @brief ControlPointsRenderer::~ControlPointsRenderer Destroys the control points renderer.
 */
ControlPointsRenderer::~ControlPointsRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoControlPoints);
    gl->glDeleteBuffers(1, &vboControlPoints);
    gl->glDeleteVertexArrays(1, &vaoControlLines);
    gl->glDeleteBuffers(1, &vboControlLines);
}

/**
 * @brief ControlPointsRenderer::initShaders Initialises the shader for the control points renderer.
 */
void ControlPointsRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shader.link();
}

/**
 * @brief ControlPointsRenderer::initBuffers Initialises the buffers for the control points renderer.
 */
void ControlPointsRenderer::initBuffers()
{
    // Create a vertex array object and a vertex buffer object for the control points
    gl->glGenVertexArrays(1, &vaoControlPoints);
    gl->glBindVertexArray(vaoControlPoints);
    gl->glGenBuffers(1, &vboControlPoints);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboControlPoints);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, xNormal));
    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));
    gl->glBindVertexArray(0);

    // Create a vertex array object and a vertex buffer object for the control lines
    gl->glGenVertexArrays(1, &vaoControlLines);
    gl->glBindVertexArray(vaoControlLines);
    gl->glGenBuffers(1, &vboControlLines);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboControlLines);

    // Set up the vertex attributes
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, xNormal));
    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));
    gl->glBindVertexArray(0);
}

/**
 * @brief ControlPointsRenderer::updateBuffers Updates the buffers with the set control points.
 */
void ControlPointsRenderer::updateBuffers()
{
    QVector<Vertex> allVertices;
    for (ControlPoint* cp : controlPoints) {
        allVertices.append(cp->getVertexArr());
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboControlPoints);
    gl->glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(Vertex), allVertices.data(), GL_STATIC_DRAW);

    controlLines.clear();
    float cpSize = controlPoints[0]->getRadius();
    int n = controlPoints.size();
    // Add the polygon lines between the control points
    for (int i = 0; i < n - 1; ++i) {
        QVector3D start = controlPoints[i]->getPosition();
        QVector3D end = controlPoints[i+1]->getPosition();
        QVector3D dir = (end - start).normalized();
        QVector3D col = QVector3D(0.0f, 1.0f, 0.0f);
        controlLines.append(Vertex(start + dir*cpSize, col, col));
        controlLines.append(Vertex(end - dir*cpSize, col, col));
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboControlLines);
    gl->glBufferData(GL_ARRAY_BUFFER, controlLines.size() * sizeof(Vertex), controlLines.data(), GL_STATIC_DRAW);
}

/**
 * @brief ControlPointsRenderer::updateUniforms Updates the uniforms for the control points renderer.
 */
void ControlPointsRenderer::updateUniforms()
{
    shader.bind();
    shader.setUniformValue("modelTransform", modelTransform);
    shader.setUniformValue("normalTransform", normalTransform);
    shader.setUniformValue("projTransform", projTransform);

    shader.setUniformValue("lightPos", settings->lightPos);
    shader.setUniformValue("lightCol", settings->lightColor);

    shader.setUniformValue("reflFlag", false);
    shader.setUniformValue("reflFreq", settings->reflFreq);
    shader.setUniformValue("percentBlack", settings->percentBlack);

    shader.release();
}

/**
 * @brief ControlPointsRenderer::paintGL Draws the control points with the corresponding lines.
 */
void ControlPointsRenderer::paintGL()
{
    //qDebug() << "ControlPointsRenderer::paintGL";
    shader.bind();
    if(settings->showControlPoints) {
        gl->glBindVertexArray(vaoControlPoints);
        int vertexCount = 0;
        for (ControlPoint* cp : controlPoints)
        {
            vertexCount += cp->getVertexArr().size();
        }
        gl->glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        gl->glBindVertexArray(vaoControlLines);
        gl->glDrawArrays(GL_LINES, 0, controlLines.size());
    }
    gl->glBindVertexArray(0);
    shader.release();
}
