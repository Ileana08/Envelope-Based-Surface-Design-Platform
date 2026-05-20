#include "tool.h"

void Tool::computeTool() {
    vertexArr.clear();
    Vertex v1,v2,v3,v4;
    for (int sIdx = 0; sIdx<sectors; sIdx++) {
        float s0 = (float)sIdx/sectors;
        float s1 = (float)(sIdx+1)/sectors;
        for (int tIdx = 0; tIdx < sectors; tIdx++) {
            float t0 = (float)tIdx/sectors;
            float t1 = (float)(tIdx+1)/sectors;

            //TODO: Why the *2*PI, shouldn't t just go from 0-1?

            v1 = getToolSurfaceAt(s0, t0*2*PI);
            v2 = getToolSurfaceAt(s1,t0*2*PI);
            v3 = getToolSurfaceAt(s0,t1*2*PI);
            v4 = getToolSurfaceAt(s1,t1*2*PI);

            // Add vertices to array
            vertexArr.append(v1);
            vertexArr.append(v4);
            vertexArr.append(v2);
            vertexArr.append(v1);
            vertexArr.append(v3);
            vertexArr.append(v4);
        }
    }
}
