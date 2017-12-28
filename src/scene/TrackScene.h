#ifndef TRACKSCENE_H
#define TRACKSCENE_H
#include <scene.h>
#include <vector>

#include <mavlink/telemetryreader.h>
#include <track/Track.h>
#include <track/trackcontroller.h>

class TrackScene : public Scene
{
public:
    TrackScene(ScenePtr checkpoint, TrackPtr track, std::shared_ptr<TrackController> tc,ScenePtr arrow, std::shared_ptr<TelemetryReader> tel);

    // Scene interface
public:
    virtual int initEgl() override;
    virtual void draw(const  glm::mat4 &viewMat,const  glm::mat4 &projMat, const glm::mat4 modelMat) override;

private:
    TrackPtr _track;
    ScenePtr _checkpoint;
    ScenePtr _arrow;
    std::shared_ptr<TrackController> _tc;
    std::vector<glm::mat4> _pointMats;
    std::shared_ptr<TelemetryReader> _tel;
};

#endif
