#include "trackcontroller.h"
#include <gis/geomath.h>
#include <gis/CorrdsConverter.h>
#include <iostream>
#include <glm/geometric.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/polar_coordinates.hpp>

TrackController::TrackController(TrackPtr track):_track(track),_nextPoint(0)
{

}

float TrackController::nextPointHdg() const
{
    float ph = atan2(_dpos.y,_dpos.x);
    //std::cout << "dpos:" << _dpos.x << " " << _dpos.y << " a:" << glm::degrees(ph) << std::endl;
    return ph;
}

int TrackController::targetPointId() const
{
    return _nextPoint;
}

int TrackController::nextTargetPointId() const
{
    int r = _nextPoint+1;
    if (r>=_track->points().size()){
        r = 0;
    }
    return r;
}

void TrackController::onAttitude(uint64_t time, double lon, double lat, double alt, double roll, double pitch, double heading)
{
    glm::vec3 pos = CoordsConverter::toMercator(glm::vec3(lon,lat,alt)) - _track->trackBase();
    glm::vec3 target = _track->points().at(targetPointId()).pos();
    float pointR = _track->posAccuracy();

    _dang = glm::radians(glm::vec3(roll,pitch,heading));
    _dpos = target-pos;

    float d = glm::length(_dpos);
    std::cout << "TD: " << d << std::endl;

    float dar = 0.0;
    float dap = 0.0;
    float dah = 0.0;

    if ((d<pointR)&&(abs(dar)<_track->rollAccuracy())
            &&(abs(dap)<_track->pitchAccuracy())
            &&(abs(dah)<_track->hdgAccuracy())){
        _nextPoint = nextTargetPointId();
    }

    _target = _track->points().at(targetPointId()).pos();
}
