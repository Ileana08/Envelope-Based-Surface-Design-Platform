#include "controlpointsrenderer.h"

ControlPointsRenderer::ControlPointsRenderer() : controlPoints() {}

ControlPointsRenderer::ControlPointsRenderer(QVector<ControlPoint*> controlPoints) : 
    controlPoints(controlPoints) 
{}

ControlPointsRenderer::~ControlPointsRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoControlPoints);
    gl->glDeleteBuffers(1, &vboControlPoints);
}

void ControlPointsRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shader.link();
}

void ControlPointsRenderer::initBuffers()
{
    gl->glGenVertexArrays(1, &vaoControlPoints);
    gl->glBindVertexArray(vaoControlPoints);
    gl->glGenBuffers(1, &vboControlPoints);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboControlPoints);

    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));

    gl->glBindVertexArray(0);
}

void ControlPointsRenderer::updateBuffers()
{
    QVector<Vertex> allVertices;
    for (ControlPoint* cp : controlPoints) {
        allVertices.append(cp->getVertexArr());
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboControlPoints);
    gl->glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(Vertex), allVertices.data(), GL_STATIC_DRAW);
}

void ControlPointsRenderer::updateUniforms()
{
    shader.bind();
    shader.setUniformValue("modelTransform", modelTransform);
    shader.setUniformValue("projTransform", projTransform);
    shader.release();
}

void ControlPointsRenderer::paintGL()
{
    qDebug() << "ControlPointsRenderer::paintGL";
    shader.bind();
    if(settings->showControlPoints) {
        gl->glBindVertexArray(vaoControlPoints);
        int vertexCount = 0;
        for (ControlPoint* cp : controlPoints)
            vertexCount += cp->getVertexArr().size();
        gl->glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
    gl->glBindVertexArray(0);
    shader.release();
}
