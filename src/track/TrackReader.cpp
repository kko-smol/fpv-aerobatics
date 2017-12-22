#include "TrackReader.h"
#include <json.h>
#include <fstream>
#include <iostream>
#include <boost/optional.hpp>
#include <gis/CorrdsConverter.h>

using json = nlohmann::json;


namespace TrackReader {

std::shared_ptr<Track> readTrack(std::string path)
{
    std::ifstream file(path,std::ios::in);
    std::string data((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    json f = json::parse(data);

    std::string name;
    std::vector<TrackPoint> points;

    for (json::iterator it = f.begin(); it != f.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n";
        if(it.key()=="name"){
            name = it.value();
        } else if (it.key()=="points"){
            for (json::iterator pit = it.value().begin(); pit != it.value().end(); ++pit) {
                boost::optional<double> lon;
                boost::optional<double> lat;
                boost::optional<double> alt;
                boost::optional<double> r,p,h;
                for (json::iterator ppit = pit.value().begin(); ppit != pit.value().end(); ++ppit) {
                    if (ppit.key()=="lon"){
                        lon = ppit.value();
                    } else if (ppit.key()=="lat"){
                        lat = ppit.value();
                    } else if (ppit.key()=="alt"){
                        alt = ppit.value();
                    } else if (ppit.key()=="roll"){
                        r = ppit.value();
                    } else if (ppit.key()=="pitch") {
                        p = ppit.value();
                    } else if (ppit.key()=="heading") {
                        h = ppit.value();
                    } else {
                        std::cout << "Unknown point attribute: " << ppit.key() << " : " << ppit.value() << "\n";
                    }
                }
                if (lat.is_initialized() && lon.is_initialized() && alt.is_initialized() &&
                        p.is_initialized() && r.is_initialized() && h.is_initialized()){
                    glm::vec3 pos = CoordsConverter::toMercator(glm::vec3(lon.get(),lat.get(),alt.get()));
                    points.push_back(TrackPoint(pos,glm::vec3(r.get(),p.get(),h.get())));
                }
            }
        }
    }
    return std::make_shared<Track>(points,name);
}
}

