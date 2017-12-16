#ifndef RENDERER_H
#define RENDERER_H

#include <scene.h>
#include <glm/mat4x4.hpp>

class Renderer
{
public:
    Renderer(ScenePtr scene);
    ~Renderer();

    virtual int init() = 0;

    void setViewMat(const glm::mat4& m);
    void setEyeMats(const glm::mat4& l, const glm::mat4& r );

    virtual void exec() = 0;
    virtual void render() = 0;
protected:
    glm::mat4 _view;

    glm::mat4 _leye;
    glm::mat4 _reye;

    int scr_w, scr_h;
    ScenePtr _scene;
};

#endif // RENDERER_H
