#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <glm/vec3.hpp>
#include <vector>

bool loadOBJ(const char * path,
        std::vector<glm::vec3> & out_vertices,
        std::vector<glm::vec3> &out_uvs
);



#endif
