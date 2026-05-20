//
// Created by david on 5/19/26.
//

#include "bezierTool.h"

#include <complex>
#include <QMatrix2x2>

bezierTool::bezierTool()
  : Tool(ToolType::Tool_Bezier), innerRadius(0.0f)
{
}

bezierTool::bezierTool(CubicBezier2D bezier, float innerRadius, float height, int sectors, QVector3D position)
  : Tool(ToolType::Tool_Cylinder, sectors, height, position), innerRadius(innerRadius)
{
}

float bezierTool::getCa(float a)
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

float bezierTool::getCaDa(float a)
{
  QVector2D bezierAt = bezier.at(a);
  QVector2D dBezierAt = bezier.derivativeAt(a);
  QVector2D normal = bezier.unitNormalAt(a);
  QVector2D dNormal = bezier.dNormalAt(a);

  return dBezierAt.x() + (normal.y() * dBezierAt.y() - bezierAt.y() * dNormal.y()) / (normal.y() * normal.y()) -
    normal.y() * normal.x() / bezierAt.y();
}

float bezierTool::getCn(float a)
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

float bezierTool::getCnDa(float a)
{
  QVector2D bezierAt = bezier.at(a);
  QVector2D dBezierAt = bezier.derivativeAt(a);
  QVector2D normal = bezier.unitNormalAt(a);
  QVector2D dNormal = bezier.dNormalAt(a);
  return (normal.y() * dBezierAt.y() - bezierAt.y() * dNormal.y()) / (normal.y() * normal.y());
}

float bezierTool::getRadiusAt(float a)
{
  return innerRadius + bezier.at(a).y();
}

float bezierTool::getRadiusDaAt(float a)
{
  return bezier.derivativeAt(a).y();
}

float bezierTool::getSphereCenterHeightAt(float a)
{
  return getCa(a);
}

float bezierTool::getSphereCenterHeightDaAt(float a)
{
  return getCaDa(a);
}

float bezierTool::getSphereRadiusAt(float a)
{
  return getCn(a);
}

float bezierTool::getSphereRadiusDaAt(float a)
{
  return getCnDa(a);
}

QVector3D bezierTool::computeNormal(QVector3D axis, QVector3D sNormal, QVector3D pathTangent, float a)
{
  QVector2D bNormal = bezier.unitNormalAt(a);
  bNormal.setX(bNormal.x() * height); //scale since bezier is normalized for height=1.0
  bNormal.normalize();
  QVector3D envelopeNormal = bNormal.x() * axis + bNormal.y() * sNormal;
  return envelopeNormal.normalized();
}

Vertex bezierTool::getToolSurfaceAt(float a, float tRad)
{
  float toolRad = getRadiusAt(a);
  QVector3D p(
    toolRad * cos(tRad),
    toolRad * sin(tRad),
    a * height
  );
  QVector3D c(1, 0, 0);
  return Vertex(p, c);
}
