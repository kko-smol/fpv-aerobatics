#ifndef GROUPSCENE_H
#define GROUPSCENE_H
#include <scene.h>
#include <vector>

class GroupScene : public Scene
{
public:
    GroupScene(std::vector<ScenePtr> parts);

    // Scene interface
public:
    virtual int initEgl() override;
    virtual void draw(const  glm::mat4 &viewMat,const  glm::mat4 &projMat, const glm::mat4 modelMat) override;

private:
    std::vector<ScenePtr> _parts;
};

#endif // GROUPSCENE_H
