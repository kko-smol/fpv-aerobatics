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

    virtual const glm::mat4 screenMatrix() const = 0;

    void setProjMat(const glm::mat4& p);
    void setViewMat(const glm::mat4& m);
    void setEyeMats(const glm::mat4& l, const glm::mat4& r );

    virtual void exec() = 0;
    virtual void render() = 0;

    virtual const glm::mat4& currentEyeMat() const= 0;

    int getScr_w() const;
    float getEyeW() const;
    int getScr_h() const;
    float getEyeH() const;

protected:
    glm::mat4 _view;
    glm::mat4 _proj;
    glm::mat4 _leye;
    glm::mat4 _reye;

    int _xBorder;
    int _yBorder;
    int scr_w, scr_h;
    ScenePtr _scene;
};

#endif // RENDERER_H

