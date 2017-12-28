#include "logplayer.h"
#include <sstream>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

LogPlayer::LogPlayer(std::string path):_file(path)
{

}

void LogPlayer::readNewAttitude(double *lon, double *lat, double *alt, double *roll, double *pitch, double *hdg)
{
    bool haveGps = false;
    bool haveAngles = false;

    std::string line;
    while (std::getline(_file, line))
    {
        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(","));
        if (strs.size()>1){
            if (strs.at(0)=="GPS"){
                haveGps = true;
                auto lons = strs.at(7);
                auto lats = strs.at(8);
                auto alts = strs.at(10);
                *lon = std::stod(lons);
                *lat = std::stod(lats);
                *alt = std::stod(alts);
            }
            if (strs.at(0)=="ATT"){
                auto r = strs.at(3);
                auto p = strs.at(5);
                auto h = strs.at(7);
                *roll = std::stod(r);
                *pitch = std::stod(p);
                *hdg = std::stod(h);
                haveAngles = true;
            }
        }
        if (haveGps && haveGps){
            return;
        }
    }

}
