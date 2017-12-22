#ifndef TRACKSCENE_H
#define TRACKSCENE_H
#include <scene.h>
#include <vector>

#include <track/Track.h>

class TrackScene : public Scene
{
public:
    TrackScene(ScenePtr checkpoint, TrackPtr track);

    // Scene interface
public:
    virtual int initEgl() override;
    virtual void draw(const  glm::mat4 &viewMat,const  glm::mat4 &projMat, const glm::mat4 modelMat) override;

private:
    TrackPtr _track;
    ScenePtr _checkpoint;
    std::vector<glm::mat4> _pointMats;
};

#endif
