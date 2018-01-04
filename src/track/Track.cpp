#include "Track.h"
#include <gis/CorrdsConverter.h>

glm::vec3 TrackPoint::pos() const {
    return _pos;
}

glm::vec3 TrackPoint::ang() const {
    return _ang;
}

TrackPoint::TrackPoint(glm::vec3 pos, glm::vec3 ang):_pos(pos),_ang(ang) {

}

Track::Track(std::vector<TrackPoint> points, std::string name, float posAccuracy, float rollAccuracy, float pitchAccuracy, float hdgAccyracy):_name(name)
{
    _posAccuracy = posAccuracy;
    _rollAccuracy = rollAccuracy;
    _pitchAccuracy = pitchAccuracy;
    _hdgAccuracy = hdgAccyracy;

    _gisPoints = points;
    glm::dvec3 origin = glm::dvec3(0.0,0.0,0.0);
    for (size_t i=0; i<points.size();++i){
        origin+=glm::dvec3(points.at(i).pos());
    }
    origin=origin/((double)points.size());
    _trackBase = glm::vec3(origin);

    std::vector<TrackPoint> newPoints;
    for (size_t i=0; i<points.size();++i){
        newPoints.push_back(TrackPoint(points.at(i).pos()-_trackBase,points.at(i).ang()));
    }
    _localPoints = newPoints;
}

const std::vector<TrackPoint>& Track::points() const
{
    return _localPoints;
}

glm::vec3 Track::longLatToTrack(glm::vec3 ll) const
{
    glm::vec3 pos = CoordsConverter::toMercator(ll);
    return pos - _trackBase;
}

float Track::posAccuracy() const
{
    return _posAccuracy;
}

float Track::rollAccuracy() const
{
    return _rollAccuracy;
}

float Track::pitchAccuracy() const
{
    return _pitchAccuracy;
}

float Track::hdgAccuracy() const
{
    return _hdgAccuracy;
}

glm::vec3 Track::trackBase() const
{
    return _trackBase;
}

std::string Track::name() const
{
    return _name;
}
