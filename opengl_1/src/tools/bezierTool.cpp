//
// Created by david on 5/19/26.
//

#include "bezierTool.h"

#include <complex>
#include <QMatrix2x2>

BezierTool::BezierTool()
  : Tool(ToolType::Tool_Bezier), innerRadius(0.0f)
{
  qDebug() << bezier.getP0() << bezier.getP1() << bezier.getP2() << bezier.getP3();
}

BezierTool::BezierTool(CubicBezier2D bezier, float innerRadius, float height, int sectors, QVector3D position)
  : Tool(ToolType::Tool_Bezier, sectors, height, position), innerRadius(innerRadius)
{
}

float BezierTool::getCa(float a)
{
  QVector2D bezierAta = bezier.at(a);
  QVector2D bezierNormalAta = bezier.unitNormalAt(a);
  float ca = 0;
  if (bezierNormalAta.y() > 1e-9f)
  {
    ca = bezierAta.x() - (bezierAta.y() / bezierNormalAta.y()) * bezierNormalAta.x();
  }
  else
  {
    qDebug() << "Ca divide by zero";
  }
  return ca;
}

float BezierTool::getCaDa(float a)
{
  QVector2D bezierAt = bezier.at(a);
  QVector2D dBezierAt = bezier.derivativeAt(a);
  QVector2D normal = bezier.unitNormalAt(a);
  QVector2D dNormal = bezier.dNormalAt(a);

  return dBezierAt.x() + (normal.y() * dBezierAt.y() - bezierAt.y() * dNormal.y()) / (normal.y() * normal.y()) -
    normal.y() * normal.x() / bezierAt.y();
}

float BezierTool::getCn(float a)
{
  QVector2D bezierAta = bezier.at(a);
  QVector2D bezierNormalAta = bezier.unitNormalAt(a);
  float cn = 0;
  if (bezierNormalAta.y() > 1e-9f)
  {
    cn = bezierAta.y() / bezierNormalAta.y();
  }
  else
  {
    qDebug() << "Cn divide by zero";
  }
  return cn;
}

float BezierTool::getCnDa(float a)
{
  QVector2D bezierAt = bezier.at(a);
  QVector2D dBezierAt = bezier.derivativeAt(a);
  QVector2D normal = bezier.unitNormalAt(a);
  QVector2D dNormal = bezier.dNormalAt(a);
  return (normal.y() * dBezierAt.y() - bezierAt.y() * dNormal.y()) / (normal.y() * normal.y());
}

float BezierTool::getRadiusAt(float a)
{
  return innerRadius + bezier.at(a).y();
}

float BezierTool::getRadiusDaAt(float a)
{
  return bezier.derivativeAt(a).y();
}

float BezierTool::getSphereCenterHeightAt(float a)
{
  return getCa(a);
}

float BezierTool::getSphereCenterHeightDaAt(float a)
{
  return getCaDa(a);
}

float BezierTool::getSphereRadiusAt(float a)
{
  return getCn(a);
}

float BezierTool::getSphereRadiusDaAt(float a)
{
  return getCnDa(a);
}

Vertex BezierTool::getToolSurfaceAt(float a, float tRad)
{
  float toolRad = getRadiusAt(a);
  QVector3D p(
    toolRad * cos(tRad),
    toolRad * sin(tRad),
    bezier.at(a).x() * height
  );
  QVector3D c(1, 0, 0);
  return Vertex(p, c);
}

QVector2D BezierTool::getProfile(float a)
{
  return {bezier.at(a).x()*height, getRadiusAt(a)};
}

QVector2D BezierTool::getProfileNormal(float a)
{
  QVector2D normal = bezier.unitNormalAt(a);
  return {normal.x(), normal.y()};
}
