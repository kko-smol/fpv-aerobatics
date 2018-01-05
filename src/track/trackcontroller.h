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

    std::chrono::milliseconds msFromLastPoint() const;

private:

    glm::vec3 _target;

    TrackPtr _track;
    size_t _nextPoint;
    std::chrono::high_resolution_clock::time_point _lastPointTs;

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
