#ifndef EGLRENDERER_H
#define EGLRENDERER_H

#include "shaders.h"
#include <scene.h>

#include <renderer.h>

class EglPrivate;
class EglRenderer : public Renderer
{
public:
    EglRenderer(ScenePtr scene);

    virtual int init() override;

    virtual void render() override;
private:
    EglPrivate* _egl;
    glm::mat4 _screen;
    int _currentEye;
    // Renderer interface
public:
    virtual void exec() override;


    // Renderer interface
public:
    virtual const glm::mat4 screenMatrix() const override;

    // Renderer interface
public:
    virtual const glm::mat4 &currentEyeMat() const override;
};

#endif // EGLRENDERER_H
