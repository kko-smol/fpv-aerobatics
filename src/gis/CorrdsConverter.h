#ifndef CORRDSCONVERTER_H
#define CORRDSCONVERTER_H

#include <glm/vec3.hpp>
#include <string>


namespace CoordsConverter {

    glm::vec3 toMercator(glm::vec3);
    glm::vec3 toLongLat(glm::vec3);

}

#endif // CORRDSCONVERTER_H
