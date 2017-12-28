#ifndef LOGPLAYER_H
#define LOGPLAYER_H

#include <string>
#include <fstream>
#include <iostream>

class LogPlayer
{
public:
    LogPlayer(std::string path);

    void readNewAttitude(double* lon, double* lat, double* alt, double* roll, double* pitch, double* hdg);

    std::ifstream _file;
};

#endif // LOGPLAYER_H
