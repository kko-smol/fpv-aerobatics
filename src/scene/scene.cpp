#include "scene.h"
#include <shaders.h>
#include <iostream>

#ifdef __arm__
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#else
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#endif

Scene::Scene():_renderer(nullptr)
{

}

Scene::~Scene()
{

}

int Scene::init(Renderer * r)
{
    _renderer = r;
    initEgl();
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

Renderer *Scene::renderer() const
{
    return _renderer;
}
