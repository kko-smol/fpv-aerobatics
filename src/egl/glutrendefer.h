#ifndef GLUTRENDEFER_H
#define GLUTRENDEFER_H

#include <renderer.h>

class GlutRenderer : public Renderer
{
public:
    GlutRenderer(ScenePtr scene);

    // Renderer interface
public:
    virtual int init() override;
    virtual void render() override;
private:
    void* _window;
    int _eyeNum;
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

#endif // GLUTRENDEFER_H
