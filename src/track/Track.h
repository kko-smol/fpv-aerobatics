#ifndef TRACK_H
#define TRACK_H

#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

class TrackPoint{
public:
    TrackPoint(glm::vec3 pos, glm::vec3 ang);
    glm::vec3 pos() const;

    glm::vec3 ang() const;

private:
    glm::vec3 _pos;
    glm::vec3 _ang;
};

class Track {
public:


    Track(std::vector<TrackPoint> points, std::string name);

    std::string name() const;

    const std::vector<TrackPoint>& points() const;

    glm::vec3 trackBase() const;

    glm::vec3 longLatToTrack(glm::vec3 ll) const;

private:
    std::string _name;
    std::vector<TrackPoint> _points;
    glm::vec3 _trackBase;
};

typedef std::shared_ptr<Track> TrackPtr;

#endif // TRACK_H
