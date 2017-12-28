#ifndef GEOMATH_H
#define GEOMATH_H

#include <glm/common.hpp>

namespace GeoMath {

const double WGS_84_RADIUS_EQUATOR = 6378137.0;
const double WGS_84_RADIUS_POLAR = 6356752.3142;

double distance(const glm::vec2 &p1, const glm::vec2 &p2);
double distance(glm::vec3 p1, glm::vec3 p2);


}

#endif // GEOMATH_H
