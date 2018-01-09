#include "renderer.h"

Renderer::Renderer(ScenePtr scene)
{
    _scene = scene;
    _proj = glm::mat4(1.0f);
    _view = glm::mat4(1.0f);
    _leye = glm::mat4(1.0f);
    _reye = glm::mat4(1.0f);
    _xBorder = 0;
    _yBorder = 0;
}

Renderer::~Renderer()
{

}

void Renderer::setProjMat(const glm::mat4 &p)
{
    _proj = p;
}

void Renderer::setViewMat(const  glm::mat4 &m)
{
    _view = m;
}

void Renderer::setEyeMats(const glm::mat4 &l, const glm::mat4 &r)
{
    _leye = l;
    _reye = r;
}

int Renderer::getScr_w() const
{
    return scr_w - 2*_xBorder;
}

float Renderer::getEyeW() const
{
    return getScr_w()/2.0;
}

int Renderer::getScr_h() const
{
    return scr_h - 2*_yBorder;
}

float Renderer::getEyeH() const
{
    return getScr_h();
}

