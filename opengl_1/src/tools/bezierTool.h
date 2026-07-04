//
// Created by david on 5/19/26.
//

#ifndef OPENGL_1_BEZIERTOOL_H
#define OPENGL_1_BEZIERTOOL_H
#include "CubicBezier2D.h"
#include "tool.h"


class BezierTool : public Tool
{
  CubicBezier2D bezier;
  float r; // Stretches the bezier curve in the y-direction.
  float innerRadius; // Does not stretch the bezier curve, but acts as an offset for the bezier y-coordinate.

public:
  BezierTool();
  BezierTool(CubicBezier2D bezier, float radius, float innerRadius, float height, int sectors, QVector3D position);

  inline void setRadius(float radius) {this->r = radius;}
  inline float getRadius() {return r;}
  inline void setInnerRadius(float innerRadius) { this->innerRadius = innerRadius; }
  inline float getInnerRadius() { return innerRadius; }
  inline CubicBezier2D getBezier() { return bezier; }
  void setPoints(QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3);
  inline void setBezier(CubicBezier2D newBezier) {bezier = newBezier;};

  float getRadiusAt(float a) override;
  float getRadiusDaAt(float a) override;
  float getSphereCenterHeightAt(float a) override;
  float getSphereCenterHeightDaAt(float a) override;
  float getSphereRadiusAt(float a) override;
  float getSphereRadiusDaAt(float a) override;

  QVector2D getProfile(float a) override;
  QVector2D getProfileDa(float a);
  QVector2D getProfileD2a(float a);
  QVector2D getProfileNormal(float a) override;
  QVector2D getProfileNormalDa(float a);
};


#endif //OPENGL_1_BEZIERTOOL_H
