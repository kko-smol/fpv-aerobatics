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

    // Renderer interface
public:
    virtual void exec() override;
};

#endif // GLUTRENDEFER_H
