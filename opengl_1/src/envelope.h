#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "vertex.h"
#include "movement/cylindermovement.h"
#include <QMatrix2x2>
#include <QQuaternion>

#include "mathutility.h"
#include "settings.h"

class Envelope
{
    int index;
    bool active;
    CylinderMovement toolMovement;
    Tool *tool;

    QVector<Envelope*> dependentEnvelopes;
    Envelope *adjEnvA0 = nullptr;
    Envelope *adjEnvA1 = nullptr;
    bool tanContToAdj = false;
    double adjAxisAngle1;
    double adjAxisAngle2;

    int sectorsA;
    int sectorsT;

    QVector<Vertex> vertexArr;
    QVector<Vertex> vertexArrCenters;
    QVector<Vertex> vertexArrGrazingCurve;
    QVector<QVector<Vertex>> vertexArrNormals;

    QVector<QVector3D> axisCache;
    QVector<QVector3D> axisDtCache;
    QVector<QVector3D> axisDt2Cache;
    QVector<QVector3D> axisDt3Cache;
    QVector<QVector3D> axisDt4Cache;

    QVector<QVector3D> pathCache;
    QVector<QVector3D> pathDtCache;
    QVector<QVector3D> pathDt2Cache;
    QVector<QVector3D> pathDt3Cache;

    QVector<QVector3D> normalA0Cache;
    QVector<QVector3D> normalA0DtCache;

    QVector<QVector3D> normalA1Cache;
    QVector<QVector3D> normalA1DtCache;

    QVector<QVector3D> a0EnvelopeCache;
    QVector<QVector3D> a1EnvelopeCache;

    QVector<QVector3D> a0EnvelopeDtCache;
    QVector<QVector3D> a1EnvelopeDtCache;

    // Render settings for reflection lines
    bool reflectionLines=false;
    float reflFreq=20;
    float percentBlack=0.5;


public:
    Envelope(int index);
    Envelope(int index, Tool *tool, const BezierPath &path);
    Envelope(int index, Tool *tool, const BezierPath &path, Envelope *adjEnvelope);

    inline void updateRenderSettings(const Settings &settings) {
        reflectionLines=settings.reflectionLines;
        reflFreq=settings.reflFreq;
        percentBlack=settings.percentBlack;
    }

    inline void setSectorsA(int n) { sectorsA = n; }
    inline void setSectorsT(int n) { sectorsT = n; }

    inline int getIndex() const { return index; }
    inline QList<Envelope*> getDependentEnvelopes() const { return dependentEnvelopes; }
    inline Envelope *getAdjA0Envelope() { return adjEnvA0; }
    void setAdjacentA0Envelope(Envelope *env);
    inline Envelope *getAdjA1Envelope() { return adjEnvA1; }
    void setAdjacentA1Envelope(Envelope *env);

    void registerDependent(Envelope *dependent);
    void deregisterDependent(Envelope *dependent);
    bool checkDependencies();
    QSet<int> getAllDependents();
    inline bool isIndependent() const {return adjEnvA0 == nullptr && adjEnvA1 == nullptr;}

    void initEnvelope();
    void update();

    void updateCaches();

    void computeEnvelope();
    QVector3D calcEnvelopeAt(float t, float a);
    QVector3D calcEnvelopeDtAt(float t, float a);
    QVector3D calcEnvelopeDt2At(float t, float a);
    QVector3D calcEnvelopeDt3At(float t, float a);

    void computeToolCenters();

    void computeGrazingCurves();

    void computeNormals();
    QVector3D calcSurfaceNormalAt(float t, float a);
    QVector3D calcNormalAt(float t, float a);
    QVector3D calcNormalDtAt(float t, float a);
    QVector3D calcNormalDt2At(float t, float a);
    QVector3D calcNormalDt3At(float t, float a);

    QVector3D calcPathAt(float t);
    QVector3D calcPathDtAt(float t);
    QVector3D calcPathDt2At(float t);
    QVector3D calcPathDt3At(float t);
    QVector3D calcPathDt4At(float t);

    QQuaternion calcAxisRotationAt(float t);
    QVector3D calcAxisAt(float t);
    QVector3D calcAxisDtAt(float t);
    QVector3D calcAxisDt2At(float t);
    QVector3D calcAxisDt3At(float t);
    QVector3D calcAxisDt4At(float t);



    inline bool isActive() { return active; }
    inline void setActive(bool value) { active = value; }

    inline bool isPositContinuous() { return adjEnvA0 != nullptr; }
    inline void setTanContinuity(bool value){ tanContToAdj = value; }
    inline bool getTanContinuity() { return tanContToAdj; }
    inline bool isTanContinuous() { return !isAxisConstrained() && isPositContinuous() && tanContToAdj; }
    inline bool isAxisConstrained() {return isPositContinuous() && adjEnvA1 != nullptr; }

    // These methods get the "cached" values, instead of recalculating them.
    inline QVector3D getA0EnvelopeAt(float t) {return MathUtility::interpolateArray(a0EnvelopeCache, t);}
    inline QVector3D getA0EnvelopeDtAt(float t) {return MathUtility::interpolateArray(a0EnvelopeDtCache, t);}
    inline QVector3D getA1EnvelopeAt(float t) {return MathUtility::interpolateArray(a1EnvelopeCache, t);}
    inline QVector3D getA1EnvelopeDtAt(float t) {return MathUtility::interpolateArray(a1EnvelopeDtCache, t);}
    inline QVector3D getPathAt(float t) {return MathUtility::interpolateArray(pathCache, t);}
    inline QVector3D getPathDtAt(float t) {return MathUtility::interpolateArray(pathDtCache, t);}
    inline QVector3D getPathDt2At(float t) {return MathUtility::interpolateArray(pathDt2Cache, t);}
    inline QVector3D getPathDt3At(float t) {return MathUtility::interpolateArray(pathDt3Cache, t);}
    inline QVector3D getAxisAt(float t) {return MathUtility::interpolateArray(axisCache, t);}
    inline QVector3D getAxisDtAt(float t) {return MathUtility::interpolateArray(axisDtCache, t);}
    inline QVector3D getAxisDt2At(float t) {return MathUtility::interpolateArray(axisDt2Cache, t);}
    inline QVector3D getAxisDt3At(float t) {return MathUtility::interpolateArray(axisDt3Cache, t);}
    inline QVector3D getAxisDt4At(float t) {return MathUtility::interpolateArray(axisDt4Cache, t);}
    inline QVector3D getNormalA0At(float t) {return MathUtility::interpolateArray(normalA0Cache, t);}
    inline QVector3D getNormalA0DtAt(float t) {return MathUtility::interpolateArray(normalA0DtCache, t);}
    inline QVector3D getNormalA1At(float t) {return MathUtility::interpolateArray(normalA1Cache, t);}
    inline QVector3D getNormalA1DtAt(float t) {return MathUtility::interpolateArray(normalA1DtCache, t);}

    inline bool setAxes(QVector3D axisA0, QVector3D axisA1) { return toolMovement.setAxisDirections(axisA0, axisA1); }
    inline void setAdjacentAxisAngles(double angle1, double angle2) { adjAxisAngle1 = angle1; adjAxisAngle2 = angle2; }
    inline double getAdjAxisAngle1() const { return adjAxisAngle1; }
    inline double getAdjAxisAngle2() const { return adjAxisAngle2; }

    inline CylinderMovement& getToolMovement() { return toolMovement; }
    inline Tool* getTool(){ return tool; }
    inline void setTool(Tool *tool) { this->tool = tool; }

    inline QVector<Vertex>& getVertexArr(){ return vertexArr; }
    inline QVector<Vertex>& getVertexArrCenters(){ return vertexArrCenters; }
    inline QVector<Vertex>& getVertexArrGrazingCurve(){ return vertexArrGrazingCurve; }
    inline QVector<QVector<Vertex>>& getVertexArrNormals() { return vertexArrNormals; }

    QMatrix4x4 getToolTransformAt(float t);
};

#endif // ENVELOPE_H
