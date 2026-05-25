#include "controlpoint.h"

ControlPoint::ControlPoint():
    position(QVector3D(0.0,0.0,0.0)), 
    radius(0.2), 
    sectors(20),
    vertexArr()
{
    computeSphere();
}

ControlPoint::ControlPoint(QVector3D position, float radius , int sectors) : 
    position(position), 
    radius(radius), 
    sectors(sectors),
    vertexArr()
{
    computeSphere();
}

Vertex ControlPoint::getSphereSurfaceAt(float angle, float rotation)
{
    QVector3D p(
        position.x() + radius*cos(rotation)*cos(angle),
        position.y() + radius*cos(rotation)*sin(angle),
        position.z() + radius*sin(rotation)
    );

    QVector3D n = (p - position).normalized();
    return Vertex(p, n, QVector3D(0.0f, 1.0f, 0.0f));
}

void ControlPoint::computeSphere() {
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

            v1 = getSphereSurfaceAt(angle0, rot0);
            v2 = getSphereSurfaceAt(angle1, rot0);
            v3 = getSphereSurfaceAt(angle1, rot1);
            v4 = getSphereSurfaceAt(angle0, rot1);

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

