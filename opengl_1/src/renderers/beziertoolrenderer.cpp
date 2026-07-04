//
// Created by david on 6/9/26.
//

#include "beziertoolrenderer.h"

BezierToolRenderer::BezierToolRenderer() : controlPoints()
{
    for (int i = 0; i < 4; i++)
    {
        controlPoints.append(new ControlPoint(QVector3D(0, 0, 0), 0.2, 10));
    }
}

BezierToolRenderer::BezierToolRenderer(QVector<ControlPoint*> controlPoints) :
    controlPoints(controlPoints)
{}

BezierToolRenderer::~BezierToolRenderer()
{
    gl->glDeleteVertexArrays(1, &vaoControlPoints);
    gl->glDeleteBuffers(1, &vboControlPoints);
    gl->glDeleteVertexArrays(1, &vaoControlLines);
    gl->glDeleteBuffers(1, &vboControlLines);
}


void BezierToolRenderer::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader2d.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader2d.glsl");

    shader.link();
}

void BezierToolRenderer::initBuffers()
{
    controlPoints.clear();
    for (int i = 0; i < 4; i++)
    {
        QVector3D position = QVector3D(bezier.getPoint(i), 0.0f);
        controlPoints.append(new ControlPoint(position, QVector3D(1, 0.7, 0.0), 0.05, 20));
    }

    gl->glGenVertexArrays(1, &vaoControlPoints);
    gl->glBindVertexArray(vaoControlPoints);
    gl->glGenBuffers(1, &vboControlPoints);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboControlPoints);

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

    gl->glGenVertexArrays(1, &vaoBezierCurve);
    gl->glBindVertexArray(vaoBezierCurve);
    gl->glGenBuffers(1, &vboBezierCurve);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboBezierCurve);

    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, xCoord));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, xNormal));
    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, rVal));
    gl->glBindVertexArray(0);
}

void BezierToolRenderer::updateBuffers()
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

    bezierCurve.clear();
    for (int i = 0; i < N; i++)
    {
        float t = float(i) / float(N);;
        float t_1 = float(i+1) / float(N);
        QVector3D pos = QVector3D(bezier.at(t), 0.0f);
        QVector3D pos_1 = QVector3D(bezier.at(t_1), 0.0f);
        QVector3D col = QVector3D(0.0f, 1.0f, 1.0f);
        bezierCurve.append(Vertex(pos, pos, col));
        bezierCurve.append(Vertex(pos_1, pos_1, col));
    }
    gl->glBindBuffer(GL_ARRAY_BUFFER, vboBezierCurve);
    gl->glBufferData(GL_ARRAY_BUFFER, bezierCurve.size() * sizeof(Vertex), bezierCurve.data(), GL_STATIC_DRAW);
}

void BezierToolRenderer::updateUniforms()
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

void BezierToolRenderer::paintGL()
{
    //qDebug() << "BezierToolRenderer::paintGL";
    shader.bind();

    if(settings->showControlPoints) {
        gl->glLineWidth(5.0f);
        gl->glBindVertexArray(vaoBezierCurve);
        gl->glDrawArrays(GL_LINES, 0, bezierCurve.size());


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

void BezierToolRenderer::updateControlPoint(int i, QVector2D pos)
{
    if (i < 0 || i >= controlPoints.size())
    {
        return;
    }

    controlPoints[i]->setPosition(QVector3D(pos.x(), pos.y(), 0.0f));
    bezier.setPoint(i, pos);
}