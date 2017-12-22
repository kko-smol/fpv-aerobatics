#ifndef TRACKREADER_H
#define TRACKREADER_H

#include "Track.h"
namespace TrackReader
{
    std::shared_ptr<Track> readTrack(std::string path);
}

#endif // TRACKREADER_H
