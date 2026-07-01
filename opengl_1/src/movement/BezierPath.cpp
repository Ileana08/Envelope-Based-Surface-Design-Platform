#include "BezierPath.h"

BezierPath::BezierPath()
    : controlPoints(QVector<ControlPoint*>())
{}

BezierPath::BezierPath(QVector<ControlPoint*> controlPoints)
    : controlPoints(controlPoints)
{}

QVector3D BezierPath::getPathAt(float t)
{
    int nrBezierCurves = controlPoints.size() / 4;
    //find in which segment t is 
    int segment = int(t * nrBezierCurves);
    if(t==1){
        segment = nrBezierCurves-1; 
    }
    // segment boundaries
    float ti1 = float(segment)/float(nrBezierCurves);
    float ti2 = float(segment+1)/float(nrBezierCurves);

    float localT = (t - ti1) / (ti2 - ti1);

    QVector3D p0 = controlPoints[4 * segment + 0]->getPosition();
    QVector3D p1 = controlPoints[4 * segment + 1]->getPosition();
    QVector3D p2 = controlPoints[4 * segment + 2]->getPosition();
    QVector3D p3 = controlPoints[4 * segment + 3]->getPosition();

    QVector3D ct = (1-localT) * (1-localT) * (1-localT) * p0 + 3 * (1-localT) * (1-localT) * localT * p1 + 3 * (1-localT) * localT * localT * p2 + localT * localT * localT * p3;
    return ct;
}

void BezierPath::addNewBezierCurve()
{
    QVector3D p3 = controlPoints[controlPoints.size() - 1]->getPosition();
    QVector3D p2 = controlPoints[controlPoints.size() - 2]->getPosition();
    controlPoints.append(new ControlPoint(p3, 0.05f, 20));
    controlPoints.append(new ControlPoint(2.0f * p3 - p2, 0.05f, 20));
    controlPoints.append(new ControlPoint(3.0f * p3 - 2.0f * p2, 0.05f, 20));
    controlPoints.append(new ControlPoint(4.0f * p3 - 3.0f * p2, 0.05f, 20));

    updateVertexArr();
}

void BezierPath::updateVertexArr()
{
    vertexArr.clear();
    QVector3D color(0,0,1);

    for(int i=0; i<=sectors; ++i){
        float t = (float) i / sectors;
        vertexArr.append(Vertex(getPathAt(t), color, color));
    }
}

QVector3D BezierPath::getDerivativeAt(float t)
{
    int nrBezierCurves = controlPoints.size() / 4;
    int segment = int(t * nrBezierCurves);
    if(t==1){
        segment = nrBezierCurves-1; 
    }
    float ti1 = float(segment)/float(nrBezierCurves);
    float ti2 = float(segment+1)/float(nrBezierCurves);

    float localT = (t - ti1) / (ti2 - ti1);

    QVector3D p0 = controlPoints[4 * segment + 0]->getPosition();
    QVector3D p1 = controlPoints[4 * segment + 1]->getPosition();
    QVector3D p2 = controlPoints[4 * segment + 2]->getPosition();
    QVector3D p3 = controlPoints[4 * segment + 3]->getPosition();
    float first_multiplier = -1 * 3 * (1-localT) * (1-localT);
    float second_multiplier = 3 * (1-localT) * (1-localT) - 6 * (1-localT) * localT;
    float third_multiplier = 6 * (1-localT) * localT - 3 * localT * localT;
    float fourth_multiplier = 3 * localT * localT;

    QVector3D tangent = first_multiplier * p0 + second_multiplier * p1 + third_multiplier * p2 + fourth_multiplier * p3;
    return tangent;
}

QVector3D BezierPath::getDerivative2At(float t) {
    int nrBezierCurves = controlPoints.size() / 4;
    int segment = int(t * nrBezierCurves);
    if(t==1){
        segment = nrBezierCurves-1; 
    }
    float ti1 = float(segment)/float(nrBezierCurves);
    float ti2 = float(segment+1)/float(nrBezierCurves);

    float localT = (t - ti1) / (ti2 - ti1);

    QVector3D p0 = controlPoints[4 * segment + 0]->getPosition();
    QVector3D p1 = controlPoints[4 * segment + 1]->getPosition();
    QVector3D p2 = controlPoints[4 * segment + 2]->getPosition();
    QVector3D p3 = controlPoints[4 * segment + 3]->getPosition();
    float first_multiplier = 6 * (1-localT);
    float second_multiplier = 6 * localT - 12 * (1-localT);
    float third_multiplier = 6 * (1-localT) - 12 * localT;
    float fourth_multiplier = 6 * localT;

    QVector3D acc = first_multiplier * p0 + second_multiplier * p1 + third_multiplier * p2 + fourth_multiplier * p3;
    return acc;
}

QVector3D BezierPath::getDerivative3At(float t) {
    int nrBezierCurves = controlPoints.size() / 4;
    int segment = int(t * nrBezierCurves);
    if(t==1){
        segment = nrBezierCurves-1; 
    }
    float ti1 = float(segment)/float(nrBezierCurves);
    float ti2 = float(segment+1)/float(nrBezierCurves);

    float localT = (t - ti1) / (ti2 - ti1);

    QVector3D p0 = controlPoints[4 * segment + 0]->getPosition();
    QVector3D p1 = controlPoints[4 * segment + 1]->getPosition();
    QVector3D p2 = controlPoints[4 * segment + 2]->getPosition();
    QVector3D p3 = controlPoints[4 * segment + 3]->getPosition();
    float first_multiplier = -6;
    float second_multiplier = 18;
    float third_multiplier = -18;
    float fourth_multiplier = 6;

    QVector3D acc = first_multiplier * p0 + second_multiplier * p1 + third_multiplier * p2 + fourth_multiplier * p3;
    return acc;
}

QVector3D BezierPath::getDerivative4PlusAt(float t) {
    return QVector3D(0,0,0);
}

void BezierPath::ensureContinuityForCps(int cpMoved){
    for (int i = 3; i<controlPoints.size(); i++) {
        if (i%4 == 0) {
            controlPoints[i]->setPosition(controlPoints[i-1]->getPosition());
        }
        if (i%4 == 1) {
            // ensuring continuity depending on the control point that was moved
            if(cpMoved % 4 != 1) {
            controlPoints[i]->setPosition(-controlPoints[i-3]->getPosition() + 2*controlPoints[i-1]->getPosition());
            } else {
            controlPoints[i-3]->setPosition(-controlPoints[i]->getPosition() + 2*controlPoints[i-1]->getPosition());
            }
        }
    }

}

void BezierPath::ensureContinuityForOcps(QVector<ControlPoint*> ocps, int ocpMoved, float height){
    for (int i = 3; i<ocps.size(); i++) {
        if (i%4 == 0) {
            ocps[i]->setPosition(ocps[i-1]->getPosition());
        }
        if (i%4 == 1) {
            // ensuring continuity depending on the orientation control point that was moved
            // getting the 3 orientation vector directions
            QVector3D a1 = (ocps[i-3]->getPosition() - controlPoints[i-3]->getPosition()).normalized();
            QVector3D a2 = (ocps[i-1]->getPosition() - controlPoints[i-1]->getPosition()).normalized();
            QVector3D a3 = (ocps[i]->getPosition() - controlPoints[i]->getPosition()).normalized();
            // 2 (N\cdot L) N - L
            if(ocpMoved % 4 != 1) {
            a3 = 2 * QVector3D::dotProduct(a2,a1) * a2 - a1;
            ocps[i]->setPosition(controlPoints[i]->getPosition() + height*a3);
            } else {
            a1 = 2 * QVector3D::dotProduct(a3,a2) * a2 - a3;
            ocps[i-3]->setPosition(controlPoints[i-3]->getPosition() + height*a1);
            }
        }
    }
}
