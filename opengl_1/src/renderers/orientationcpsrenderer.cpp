#include "orientationcpsrenderer.h"

/**
 * @brief OrientationCPsRenderer::OrientationCPsRenderer Creates new orientation control points.
 */
OrientationCPsRenderer::OrientationCPsRenderer() : orientationControlPoints() {}

/**
 * @brief OrientationCPsRenderer::OrientationCPsRenderer Creates new orientation control points renderer.
 * with the orientation control points.
 * @param orientationControlPoints List of control points.
 */
OrientationCPsRenderer::OrientationCPsRenderer(QVector<ControlPoint*> orientationControlPoints) : 
    orientationControlPoints(orientationControlPoints) 
{}

/**
 * @brief OrientationCPsRenderer::~OrientationCPsRenderer Destroys the orientation control points renderer.
 */
OrientationCPsRenderer::~OrientationCPsRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoOrientationControlPoints);
    gl->glDeleteBuffers(1, &vboOrientationControlPoints);
    gl->glDeleteVertexArrays(1, &vaoControlLines);
    gl->glDeleteBuffers(1, &vboControlLines);
}

/**
 * @brief OrientationCPsRenderer::initShaders Initialises the shader for the orientation control points renderer.
 */
void OrientationCPsRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shader.link();
}

/**
 * @brief OrientationCPsRenderer::initBuffers Initialises the buffers for the orientation control points renderer.
 */
void OrientationCPsRenderer::initBuffers()
{
    // Create a vertex array object and a vertex buffer object for the orientation points
    gl->glGenVertexArrays(1, &vaoOrientationControlPoints);
    gl->glBindVertexArray(vaoOrientationControlPoints);
    gl->glGenBuffers(1, &vboOrientationControlPoints);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboOrientationControlPoints);

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

    // Create a vertex array object and a vertex buffer object for the direction lines
    gl->glGenVertexArrays(1, &vaoDirectionLines);
    gl->glBindVertexArray(vaoDirectionLines);
    gl->glGenBuffers(1, &vboDirectionLines);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboDirectionLines);

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
 * @brief OrientationCPsRenderer::updateBuffers Updates the buffers with the set orientation control points.
 */
void OrientationCPsRenderer::updateBuffers()
{
    QVector<Vertex> allVertices;
    for (ControlPoint* cp : orientationControlPoints) {
        allVertices.append(cp->getVertexArr());
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboOrientationControlPoints);
    gl->glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(Vertex), allVertices.data(), GL_STATIC_DRAW);

    controlLines.clear();
    directionLines.clear();
    int n = orientationControlPoints.size();
    // Add the polygon lines between the orientation control points
    for (int i = 0; i < n - 1; ++i) {
        QVector3D start = orientationControlPoints[i]->getPosition();
        QVector3D end = orientationControlPoints[i+1]->getPosition();
        QVector3D col = QVector3D(0.0f, 1.0f, 1.0f);
        controlLines.append(Vertex(start, col, col));
        controlLines.append(Vertex(end, col, col));
    }

    // Add the direction lines between each orientation control point and its corresponding control point
    for (int i = 0; i < n ; ++i) {
        QVector3D col = QVector3D(0.0f, 1.0f, 1.0f);
        QVector3D start = orientationControlPoints[i]->getPosition();
        QVector3D cpPosition = controlPoints[i]->getPosition();
        directionLines.append(Vertex(cpPosition, col, col));
        directionLines.append(Vertex(start, col, col));
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboControlLines);
    gl->glBufferData(GL_ARRAY_BUFFER, controlLines.size() * sizeof(Vertex), controlLines.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboDirectionLines);
    gl->glBufferData(GL_ARRAY_BUFFER, directionLines.size() * sizeof(Vertex), directionLines.data(), GL_STATIC_DRAW);
}

/**
 * @brief OrientationCPsRenderer::updateUniforms Updates the uniforms for the orientation control points renderer.
 */
void OrientationCPsRenderer::updateUniforms()
{
    shader.bind();
    shader.setUniformValue("modelTransform", modelTransform);
    shader.setUniformValue("normalTransform", normalTransform);
    shader.setUniformValue("viewTransform", viewTransform);
    shader.setUniformValue("projTransform", projTransform);

    shader.setUniformValue("cameraPos", cameraPos);
    shader.setUniformValue("lightPos", lightPos);
    shader.setUniformValue("lightCol", settings->lightColor);

    shader.setUniformValue("reflFlag", false);
    shader.setUniformValue("reflFreq", settings->reflFreq);
    shader.setUniformValue("percentBlack", settings->percentBlack);

    shader.release();
}

/**
 * @brief OrientationCPsRenderer::paintGL Draws the orientation control points with the corresponding lines.
 */
void OrientationCPsRenderer::paintGL()
{
    //qDebug() << "OrientationCPsRenderer::paintGL";
    shader.bind();
    if(settings->showOrientationControlPoints) {
        gl->glBindVertexArray(vaoOrientationControlPoints);
        int vertexCount = 0;
        for (ControlPoint* cp : orientationControlPoints)
            vertexCount += cp->getVertexArr().size();
        gl->glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        gl->glBindVertexArray(vaoControlLines);
        gl->glDrawArrays(GL_LINES, 0, controlLines.size());
        gl->glBindVertexArray(vaoDirectionLines);
        gl->glDrawArrays(GL_LINES, 0, directionLines.size());
    }
    gl->glBindVertexArray(0);
    shader.release();
}
