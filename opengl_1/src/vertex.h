#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>

struct Vertex {
    float xCoord;
    float yCoord;
    float zCoord;
    float xNormal;
    float yNormal;
    float zNormal;
    float rVal;
    float gVal;
    float bVal;

public:
    Vertex()
        : xCoord(0.0), yCoord(0.0), zCoord(0.0),
          xNormal(0.0), yNormal(0.0), zNormal(0.0),
          rVal(0.0), gVal(0.0), bVal(0.0) {}

    Vertex(float xCoord, float yCoord, float zCoord, float xNorm, float yNorm, float zNorm)
        : xCoord(xCoord), yCoord(yCoord), zCoord(zCoord),
          xNormal(xNorm), yNormal(yNorm), zNormal(zNorm),
          rVal(0.0), gVal(0.0), bVal(0.0) {}

    Vertex(QVector3D position, QVector3D normal, float rVal, float gVal, float bVal)
        : xCoord(position.x()), yCoord(position.y()), zCoord(position.z()),
          xNormal(normal.x()), yNormal(normal.y()), zNormal(normal.z()),
          rVal(rVal), gVal(gVal), bVal(bVal) {}

    Vertex(QVector3D position, QVector3D normal, QVector3D color)
        : xCoord(position.x()), yCoord(position.y()), zCoord(position.z()),
          xNormal(normal.x()), yNormal(normal.y()), zNormal(normal.z()),
          rVal(color.x()), gVal(color.y()), bVal(color.z()) {}

    QVector3D getPosition() { return QVector3D(xCoord, yCoord, zCoord); }
    QVector3D getNormal()   { return QVector3D(xNormal, yNormal, zNormal); }
    QVector3D getColor()    { return QVector3D(rVal, gVal, bVal); }
};

#endif // VERTEX_H
