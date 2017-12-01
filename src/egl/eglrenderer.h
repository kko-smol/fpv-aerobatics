#ifndef EGLRENDERER_H
#define EGLRENDERER_H

#include "shaders.h"
#include <scene.h>
#include <esUtil.h>

class Renderer {

};

class EglPrivate;
class EglRenderer : public Renderer
{
public:
    EglRenderer(ScenePtr scene);

    int init();

    void setEyeMats(const ESMatrix& l, const ESMatrix& r );
    const ESMatrix& leftEye() const;
    const ESMatrix& rightEye() const;

    void render();
private:
    EglPrivate* _egl;
    ESMatrix _lEye;
    ESMatrix _rEye;
    int scr_w, scr_h;
    ScenePtr _scene;
};

#endif // EGLRENDERER_H
