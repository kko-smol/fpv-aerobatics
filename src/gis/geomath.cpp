#include "geomath.h"
#include <glm/glm.hpp>


double GeoMath::distance(const glm::vec2& p1, const glm::vec2& p2)
{
    glm::vec2 pr1 = glm::radians(p1);
    glm::vec2 pr2 = glm::radians(p2);
    double dLat = (pr2.y-pr1.y); //lat
    double dLon = (pr2.x-pr1.x); //lon
    double a = sin(dLat/2.0) * sin(dLat/2.0) +
               cos(p1.y) *  cos(p2.y) *
               sin(dLon/2.0) * sin(dLon/2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0-a));
    double d = WGS_84_RADIUS_EQUATOR * c;
return d;
}

double GeoMath::distance(glm::vec3 p1, glm::vec3 p2)
{
    double d2d = distance(glm::vec2(p1),glm::vec2(p2));
    double da = p2.z - p1.z;
    return sqrt(d2d*d2d+da*da);
}
