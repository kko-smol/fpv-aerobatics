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

Track::Track(std::vector<TrackPoint> points, std::string name):_name(name)
{
    glm::dvec3 origin = glm::dvec3(0.0,0.0,0.0);
    for (size_t i=0; i<points.size();++i){
        origin+=points.at(i).pos();
    }
    origin=origin/((double)points.size());
    _trackBase = origin;

    std::vector<TrackPoint> newPoints;
    for (size_t i=0; i<points.size();++i){
        newPoints.push_back(TrackPoint(points.at(i).pos()-_trackBase,points.at(i).ang()));
    }
    _points = newPoints;
}

const std::vector<TrackPoint>& Track::points() const
{
    return _points;
}

glm::vec3 Track::longLatToTrack(glm::vec3 ll) const
{
    glm::vec3 pos = CoordsConverter::toMercator(ll);
    return pos - _trackBase;
}

glm::vec3 Track::trackBase() const
{
    return _trackBase;
}

std::string Track::name() const
{
    return _name;
}
