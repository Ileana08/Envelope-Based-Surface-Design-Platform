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
  float innerRadius;

public:
  BezierTool();
  BezierTool(CubicBezier2D bezier, float innerRadius, float height, int sectors, QVector3D position);

  inline void setInnerRadius(float innerRadius) { this->innerRadius = innerRadius; }
  inline float getInnerRadius() { return innerRadius; }

  float getCa(float a);
  float getCaDa(float a);
  float getCn(float a);
  float getCnDa(float a);


  float getRadiusAt(float a) override;
  float getRadiusDaAt(float a) override;
  float getSphereCenterHeightAt(float a) override;
  float getSphereCenterHeightDaAt(float a) override;
  float getSphereRadiusAt(float a) override;
  float getSphereRadiusDaAt(float a) override;

  QVector2D getProfile(float a) override;
  QVector2D getProfileDa(float a);
  QVector2D getProfileNormal(float a) override;
  QVector2D getProfileNormalDa(float a);
};


#endif //OPENGL_1_BEZIERTOOL_H
