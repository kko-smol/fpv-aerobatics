#ifndef TELEMETRYREADER_H
#define TELEMETRYREADER_H

#include <memory>
#include <list>
#include <vector>

#include "../io/iodevice.h"

#include "mavlink/ardupilotmega/mavlink.h"
#include "mavlink/common/mavlink.h"
#include "mavlink/mavlink_helpers.h"

class TelemetryListener {
public:
    virtual void onAttitude(uint64_t time, double lon, double lat, double alt, double roll, double pitch, double heading) = 0;
};

typedef std::shared_ptr<TelemetryListener> TelemetryListenerPtr;


class TelemetryReader : public IOClient
{
public:
    TelemetryReader();

    void subscribe  (TelemetryListenerPtr s);
    void unsubscribe(TelemetryListenerPtr s);

    uint64_t lastTime() const;

    double lastLon() const;

    double lastLat() const;

    double lastAlt() const;

    double lastRoll() const;

    double lastPitch() const;

    double lastHeading() const;

private:

    void readMessage(mavlink_message_t *msg);

    //mavlink
    uint32_t _sysId;
    uint32_t _compId;

    uint32_t _thisSysId;
    uint32_t _thisCompId;

    //
    std::list<TelemetryListenerPtr> _clients;

    std::vector<uint8_t> _buffer;

    uint64_t _lastTime;
    double _lastLon;
    double _lastLat;
    double _lastAlt;
    double _lastRoll;
    double _lastPitch;
    double _lastHeading;

    void sendMessage(mavlink_message_t *msg);
    void onBufRx(std::vector<uint8_t> buf);
    // IOClient interface
public:
    virtual void onGotData(const std::vector<uint8_t> &buf) override;
};

#endif // TELEMETRYREADER_H
