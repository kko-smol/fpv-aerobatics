#ifndef TRACKCONTROLLER_H
#define TRACKCONTROLLER_H

#include "telemetryreader.h"
#include "Track.h"

class TrackController : public TelemetryListener
{
public:
    TrackController(TrackPtr track);

    float nextPointHdg() const; //in radians

    int targetPointId() const;
    int nextTargetPointId() const;

private:

    glm::vec3 _target;

    TrackPtr _track;
    size_t _nextPoint;

    float _targetDist;
    glm::vec3 _dpos;
    glm::vec3 _dang;

    // TelemetryListener interface
public:
    virtual void onAttitude(uint64_t time,
                            double lon, double lat, double alt,
                            double roll, double pitch, double heading) override;
    float targetDist() const;
};

typedef std::shared_ptr<TrackController> TrackControllerPtr;

#endif // TRACKCONTROLLER_H
