#ifndef TRACKSCENE_H
#define TRACKSCENE_H
#include <scene.h>
#include <vector>

#include <mavlink/telemetryreader.h>
#include <track/Track.h>
#include <track/trackcontroller.h>
#include <textscene.h>

class TrackScene : public Scene
{
public:
    TrackScene(ScenePtr checkpoint, TrackPtr track, std::shared_ptr<TrackController> tc,ScenePtr arrow, std::shared_ptr<TelemetryReader> tel);

    // Scene interface
public:
    virtual void draw(const  glm::mat4 &viewMat,const  glm::mat4 &projMat, const glm::mat4 modelMat) override;

private:
    virtual int initEgl() override;
    TrackPtr _track;
    ScenePtr _checkpoint;
    ScenePtr _arrow;
    std::shared_ptr<TrackController> _tc;
    std::vector<glm::mat4> _pointMats;
    std::shared_ptr<TelemetryReader> _tel;

    std::shared_ptr<TextScene> _distText;
    std::shared_ptr<TextScene> _timeText;
};

#endif
