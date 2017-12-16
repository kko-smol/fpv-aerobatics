#include "scene.h"
#include <shaders.h>
#include <iostream>
#ifdef __arm__
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#else
#include <GLFW/glfw3.h>
#endif

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::glCheckError(){
    uint  err = 0;
    err = glGetError();
    while (err){
        std::cerr<< "GL ERR" << err << std::endl;
        err = glGetError();
        //GL_INVALID_ENUM
    }
}
