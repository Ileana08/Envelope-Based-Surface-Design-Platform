//
// Created by david on 5/19/26.
//

#include "bezierTool.h"

#include <complex>
#include <QMatrix2x2>

BezierTool::BezierTool()
  : Tool(ToolType::Tool_Bezier), r(1.0f), innerRadius(0.0f)
{
}

BezierTool::BezierTool(CubicBezier2D bezier, float radius, float innerRadius, float height, int sectors, QVector3D position)
  : Tool(ToolType::Tool_Bezier, sectors, height, position), r(radius), innerRadius(innerRadius), bezier(bezier)
{
}

void BezierTool::setPoints(QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3)
{
  bezier.setPoints(p0, p1, p2, p3);
}

float BezierTool::getRadiusAt(float a)
{
  return innerRadius + r*bezier.at(a).y();
}

float BezierTool::getRadiusDaAt(float a)
{
  return r*getProfileDa(a).y();
}

float BezierTool::getSphereCenterHeightAt(float a)
{
  // Ca corresponds to the sphere center height.
  QVector2D bezierAta = getProfile(a); //stretch bezier to fit height and inner radius
  QVector2D bezierNormalAta = getProfileNormal(a);
  float ca = 0;
  if (bezierNormalAta.y() > 1e-9f)
  {
    ca = bezierAta.x() - (bezierAta.y() / bezierNormalAta.y()) * bezierNormalAta.x();
  }
  else
  {
    qDebug() << "Ca divide by zero a = " << a;
  }
  return ca;
}

float BezierTool::getSphereCenterHeightDaAt(float a)
{
  // CaDa corresponds to the sphere center height derivative wrt a.
  QVector2D bezierAt = getProfile(a);
  QVector2D dBezierAt = getProfileDa(a);
  QVector2D normal = getProfileNormal(a);
  QVector2D dNormal = getProfileNormalDa(a);

  float caDa = 0;
  if (abs(normal.y()) > 1e-9f && abs(bezierAt.y()) > 1e-9f)
  {
    caDa = dBezierAt.x() - normal.x() * (normal.y() * dBezierAt.y() - bezierAt.y() * dNormal.y()) / (normal.y() * normal.y()) -
    (bezierAt.y() / normal.y())*dNormal.x() ;
  }
  else
  {
    qDebug() << "CaDa divide by zero a = " << a;
  }
  return caDa;
}

float BezierTool::getSphereRadiusAt(float a)
{
  // Cn corresponds to the sphere radius.
  QVector2D bezierAta = getProfile(a);
  QVector2D bezierNormalAta = getProfileNormal(a);
  float cn = 0;
  if (abs(bezierNormalAta.y()) > 1e-9f)
  {
    cn = bezierAta.y() / bezierNormalAta.y();
  }
  else
  {
    qDebug() << "Cn divide by zero a = " << a;
  }
  return cn;
}

float BezierTool::getSphereRadiusDaAt(float a)
{
  // CaDa corresponds to the sphere radius derivative wrt a.
  QVector2D bezierAt = getProfile(a);
  QVector2D dBezierAt = getProfileDa(a);
  QVector2D normal = getProfileNormal(a);

  float ca = getSphereCenterHeightAt(a);
  float caDa = getSphereCenterHeightDaAt(a);
  float cn = getSphereRadiusAt(a);
  float cnDa = 0;
  if (abs(normal.y()) > 1e-9f)
  {
    //Thought this should be right, but it's apparently wrong.
    //cnDa = (normal.y() * dBezierAt.y() - bezierAt.y() * dNormal.y()) / (normal.y() * normal.y());

    //Vector sphere center to profile point and its derivative
    QVector2D d(bezierAt.x() - ca, bezierAt.y());
    QVector2D dDa(dBezierAt.x() - caDa, dBezierAt.y());

    cnDa = QVector2D::dotProduct(d, dDa) / cn;
  }
  else
  {
    qDebug() << "CnDa divide by zero a = " << a;
  }
  return cnDa;
}

// Profile and its normal

QVector2D BezierTool::getProfile(float a)
{
  return {bezier.at(a).x()*height, getRadiusAt(a)};
}

QVector2D BezierTool::getProfileDa(float a)
{
  QVector2D baseProfileDa = bezier.derivativeAt(a);
  return QVector2D(baseProfileDa.x() * height, baseProfileDa.y() * r);
}

QVector2D BezierTool::getProfileD2a(float a)
{
  QVector2D baseProfileD2a = bezier.secondDerivativeAt(a);
  return QVector2D(baseProfileD2a.x() * height, baseProfileD2a.y() * r);
}

QVector2D BezierTool::getProfileNormal(float a)
{
  QVector2D t = getProfileDa(a);
  return QVector2D(-t.y(), t.x()).normalized();
}

QVector2D BezierTool::getProfileNormalDa(float a)
{
  //QVector2D baseNormalDa = bezier.dNormalAt(a);
  //return QVector2D(baseNormalDa.x()/height, baseNormalDa.y());

  QVector2D T  = getProfileDa(a);
  QVector2D Td = getProfileD2a(a);

  float len = T.length();
  if (len < 1e-6f)
    return QVector2D(0.0f, 0.0f);

  QVector2D perpT(-T.y(), T.x());
  QVector2D perpTd(-Td.y(), Td.x());

  float dot = QVector2D::dotProduct(T, Td);

  return perpTd / len
    - perpT * (dot / (len * len * len));
}
