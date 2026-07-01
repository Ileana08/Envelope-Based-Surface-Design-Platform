#include "controlpoint.h"

ControlPoint::ControlPoint():
    position(QVector3D(0.0,0.0,0.0)),
    color(QVector3D(0.0f, 1.0f, 0.0f)),
    radius(0.2), 
    sectors(20),
    vertexArr()
{
    computeControlPoint();
}

ControlPoint::ControlPoint(QVector3D position, float radius , int sectors) : 
    position(position),
    color(QVector3D(0.0f, 1.0f, 0.0f)),
    radius(radius), 
    sectors(sectors),
    vertexArr()
{
    computeControlPoint();
}

ControlPoint::ControlPoint(QVector3D position, QVector3D color, float radius, int sectors) :
    position(position),
    color(color),
    radius(radius),
    sectors(sectors),
    vertexArr()
{
    computeControlPoint();
}

Vertex ControlPoint::getSurfaceAt(float angle, float rotation)
{
    QVector3D p(
        position.x() + radius*cos(rotation)*cos(angle),
        position.y() + radius*cos(rotation)*sin(angle),
        position.z() + radius*sin(rotation)
    );

    QVector3D n = (p - position).normalized();
    return Vertex(p, n, color);
}

void ControlPoint::computeControlPoint() {
    vertexArr.clear();

    Vertex v1, v2, v3, v4;

    float angle = 2.0f * PI / sectors;
    float rotation = PI / sectors;

    for (int i = 0; i < sectors; i++) {
        float angle0 = i * angle;
        float angle1 = (i + 1) * angle;
        for (int j = 0; j < sectors; j++) {
            float rot0 = -PI/2.0f + j*rotation;
            float rot1 = -PI/2.0f + (j+1)*rotation;

            v1 = getSurfaceAt(angle0, rot0);
            v2 = getSurfaceAt(angle1, rot0);
            v3 = getSurfaceAt(angle1, rot1);
            v4 = getSurfaceAt(angle0, rot1);

            // Add vertices to array
            vertexArr.append(v1);
            vertexArr.append(v3);
            vertexArr.append(v2);
            vertexArr.append(v1);
            vertexArr.append(v4);
            vertexArr.append(v3);
        }
    }
       
};
