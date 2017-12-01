#ifndef SHADERS_H
#define SHADERS_H

#include <string>

class Shaders
{
public:
    Shaders();

    std::string getTexVertexShader();
    std::string getTexFragmentShader();

    std::string getVecVertexShader();
    std::string getVecFragmentShader();

};

#endif //
