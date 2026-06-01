#include "orientationcpsrenderer.h"

OrientationCPsRenderer::OrientationCPsRenderer() : orientationControlPoints() {}

OrientationCPsRenderer::OrientationCPsRenderer(QVector<OrientationControlPoint*> orientationControlPoints) : 
    orientationControlPoints(orientationControlPoints) 
{}

OrientationCPsRenderer::~OrientationCPsRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoOrientationControlPoints);
    gl->glDeleteBuffers(1, &vboOrientationControlPoints);
    gl->glDeleteVertexArrays(1, &vaoControlLines);
    gl->glDeleteBuffers(1, &vboControlLines);
}


void OrientationCPsRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

    shader.link();
}

void OrientationCPsRenderer::initBuffers()
{
    gl->glGenVertexArrays(1, &vaoOrientationControlPoints);
    gl->glBindVertexArray(vaoOrientationControlPoints);
    gl->glGenBuffers(1, &vboOrientationControlPoints);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboOrientationControlPoints);

    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, xNormal));
    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));
    gl->glBindVertexArray(0);

    gl->glGenVertexArrays(1, &vaoControlLines);
    gl->glBindVertexArray(vaoControlLines);
    gl->glGenBuffers(1, &vboControlLines);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboControlLines);

    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, xNormal));
    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));
    gl->glBindVertexArray(0);
}

void OrientationCPsRenderer::updateBuffers()
{
    QVector<Vertex> allVertices;
    for (OrientationControlPoint* cp : orientationControlPoints) {
        allVertices.append(cp->getVertexArr());
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboOrientationControlPoints);
    gl->glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(Vertex), allVertices.data(), GL_STATIC_DRAW);

    controlLines.clear();
    int n = orientationControlPoints.size();
    for (int i = 0; i < n - 1; ++i) {
        QVector3D start = orientationControlPoints[i]->getPosition();
        QVector3D end = orientationControlPoints[i+1]->getPosition();
        QVector3D col = QVector3D(0.0f, 1.0f, 1.0f);
        controlLines.append(Vertex(start, col, col));
        controlLines.append(Vertex(end, col, col));
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, vboControlLines);
    gl->glBufferData(GL_ARRAY_BUFFER, controlLines.size() * sizeof(Vertex), controlLines.data(), GL_STATIC_DRAW);
}

void OrientationCPsRenderer::updateUniforms()
{
    shader.bind();
    shader.setUniformValue("modelTransform", modelTransform);
    shader.setUniformValue("normalTransform", normalTransform);
    shader.setUniformValue("projTransform", projTransform);

    shader.setUniformValue("lightPos", settings->lightPos);
    shader.setUniformValue("lightCol", settings->lightColor);

    shader.release();
}

void OrientationCPsRenderer::paintGL()
{
    qDebug() << "OrientationCPsRenderer::paintGL";
    shader.bind();
    if(settings->showOrientationControlPoints) {
        gl->glBindVertexArray(vaoOrientationControlPoints);
        int vertexCount = 0;
        for (OrientationControlPoint* cp : orientationControlPoints)
            vertexCount += cp->getVertexArr().size();
        gl->glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        gl->glBindVertexArray(vaoControlLines);
        gl->glDrawArrays(GL_LINES, 0, controlLines.size());
    }
    gl->glBindVertexArray(0);
    shader.release();
}
