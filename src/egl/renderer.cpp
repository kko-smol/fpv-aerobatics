#include "renderer.h"

Renderer::Renderer(ScenePtr scene)
{
    _scene = scene;
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
