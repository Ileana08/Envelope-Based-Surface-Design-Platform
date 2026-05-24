#ifndef SPHERE_H
#define SPHERE_H

#include <QVector3D>
#include <QList>
#include "vertex.h"

class Sphere
{
    QVector3D position;
    float radius;

    constexpr static float PI = 3.14159265;

    QVector<Vertex> vertexArr;
public:
    Sphere() : position(QVector3D(0, 0, 0)), radius(1) {}
    Sphere(QVector3D position, float radius) : position(position), radius(radius) {}

    inline QVector<Vertex>& getVertexArr() { return vertexArr; }
    inline QVector3D getPosition() { return position; }
    inline float getRadius() { return radius; }
    inline void setPosition(QVector3D position) { this->position = position; }
    inline void setRadius(float radius) { this->radius = radius; }

    inline void generateVertexArr(int numSlices, int numStacks) {
        vertexArr.clear();
        QVector3D v1, v2, v3, v4;
        QVector3D n1, n2, n3, n4;
        QVector3D c1, c2, c3, c4;
        for (int i = 0; i < numSlices; i++)
        {
            for (int j = 0; j < numStacks; j++)
            {
                v1 = QVector3D(radius * cos(2 * PI * i / numSlices) * sin(PI * j / numStacks),
                                radius * sin(2 * PI * i / numSlices) * sin(PI * j / numStacks),
                                radius * cos(PI * j / numStacks));
                v2 = QVector3D(radius * cos(2 * PI * (i + 1) / numSlices) * sin(PI * j / numStacks),
                                radius * sin(2 * PI * (i + 1) / numSlices) * sin(PI * j / numStacks),
                                radius * cos(PI * j / numStacks));
                v3 = QVector3D(radius * cos(2 * PI * (i + 1) / numSlices) * sin(PI * (j + 1) / numStacks),
                                radius * sin(2 * PI * (i + 1) / numSlices) * sin(PI * (j + 1) / numStacks),
                                radius * cos(PI * (j + 1) / numStacks));
                v4 = QVector3D(radius * cos(2 * PI * i / numSlices) * sin(PI * (j + 1) / numStacks),
                                radius * sin(2 * PI * i / numSlices) * sin(PI * (j + 1) / numStacks),
                                radius * cos(PI * (j + 1) / numStacks));

                n1 = v1.normalized();
                n2 = v2.normalized();
                n3 = v3.normalized();
                n4 = v4.normalized();

                c1 = QVector3D(0, 1, 0);
                c2 = QVector3D(0, 1, 0);
                c3 = QVector3D(0, 1, 0);
                c4 = QVector3D(0, 1, 0);
                
                v1 += position;
                v2 += position;
                v3 += position;
                v4 += position;

                vertexArr.push_back(Vertex(v1, n1, c1));
                vertexArr.push_back(Vertex(v2, n2, c2));
                vertexArr.push_back(Vertex(v3, n3, c3));
                vertexArr.push_back(Vertex(v3, n3, c3));
                vertexArr.push_back(Vertex(v4, n4, c4));
                vertexArr.push_back(Vertex(v1, n1, c1));
            }
        }
        
    }
};

#endif // SPHERE_H
