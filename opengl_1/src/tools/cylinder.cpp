#include "cylinder.h"

/**
 * @brief Cylinder::Cylinder Default constructor builds cylinder to preset size
 */
Cylinder::Cylinder()
    : Tool(ToolType::Tool_Cylinder), r(0.5), angle(0.0) {}

/**
 * @brief Cylinder::Cylinder Cylinder constructor
 * @param baseRadius radius of the smaller end
 * @param angle opening angle
 * @param height cylinder height
 * @param sectors number of horizontal and vertical sectors for the cylinder
 * @param position position of the center of the base of the cylinder
 */
Cylinder::Cylinder(float baseRadius, float angle, float height, int sectors, QVector3D position)
    : Tool(ToolType::Tool_Cylinder, sectors, height, position), r(baseRadius), angle(angle)
{}

QVector2D Cylinder::getProfile(float a)
{
    return {a*height, getRadiusAt(a)};
}

QVector2D Cylinder::getProfileNormal(float a)
{
    return {-sinf(angle), cosf(angle)};
}
