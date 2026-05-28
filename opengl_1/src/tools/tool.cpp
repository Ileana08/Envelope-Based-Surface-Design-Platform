#include "tool.h"

void Tool::computeTool() {
    vertexArr.clear();
    QVector3D v1,v2,v3,v4;
    QVector3D n1,n2,n3,n4;
    QVector3D c1,c2,c3,c4;
    for (int sIdx = 0; sIdx<sectors; sIdx++) {
        float s0 = (float)sIdx/sectors;
        float s1 = (float)(sIdx+1)/sectors;
        for (int tIdx = 0; tIdx < sectors; tIdx++) {
            float t0 = (float)tIdx/sectors;
            float t1 = (float)(tIdx+1)/sectors;

            v1 = getToolSurfaceAt(s0, t0*2*PI);
            v2 = getToolSurfaceAt(s1,t0*2*PI);
            v3 = getToolSurfaceAt(s0,t1*2*PI);
            v4 = getToolSurfaceAt(s1,t1*2*PI);

            n1 = getToolNormalAt(s0, t0*2*PI);
            n2 = getToolNormalAt(s1, t0*2*PI);
            n3 = getToolNormalAt(s0, t1*2*PI);
            n4 = getToolNormalAt(s1, t1*2*PI);

            c1 = QVector3D(0.8f, 0.0f, 0.0f);
            c2 = QVector3D(0.8f, 0.0f, 0.0f);
            c3 = QVector3D(0.8f, 0.0f, 0.0f);
            c4 = QVector3D(0.8f, 0.0f, 0.0f);

            // Add vertices to array
            vertexArr.append(Vertex(v1, n1, c1));
            vertexArr.append(Vertex(v4, n4, c4));
            vertexArr.append(Vertex(v2, n2, c2));
            vertexArr.append(Vertex(v1, n1, c1));
            vertexArr.append(Vertex(v3, n3, c3));
            vertexArr.append(Vertex(v4, n4, c4));

        }
    }
}

QVector3D Tool::getToolSurfaceAt(float a, float tRad) {
    QVector3D planeAxis(cosf(tRad), sinf(tRad), 0);
    planeAxis.normalize();
    /* Old implementation
    float toolRad = getRadiusAt(a);
    QVector3D p(
        toolRad*cosf(tRad),
        toolRad*sinf(tRad),
        a*height
        );
    */
    QVector3D p = getProfile(a).x() * getAxisVector() + getProfile(a).y() * planeAxis;
    return p;
    //return getProfile(a).x() * getAxisVector() + getProfile(a).y() * planeAxis;
}

QVector3D Tool::getToolNormalAt(float a, float tRad)
{
    QVector3D planeAxis(cosf(tRad), sinf(tRad), 0);
    QVector3D normal = (getProfileNormal(a).x() * getAxisVector() + getProfileNormal(a).y() * planeAxis).normalized();

    return normal;
}
