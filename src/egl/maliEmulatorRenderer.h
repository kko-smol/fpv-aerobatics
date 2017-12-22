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
    int scr_w, scr_h;

    // Renderer interface
public:
    virtual void exec() override;
};

#endif // EGLRENDERER_H
