#ifndef APP_H
#define APP_H

#include <vdevice.h>
#include <renderer.h>
#include <bgtexscene.h>
#include <testboxscene.h>
#include <objscene.h>
#include <scene/TrackScene.h>
#include <groupscene.h>
#include <boost/asio.hpp>
#include <telemetryreader.h>
#include "io/serialportio.h"
#include "io/udplistenio.h"
#include <track/trackcontroller.h>
#include <thread>
#include <h264Compress/ffmpegencoder.h>

class App
{
public:
    App();

    bool init();

    void run();

protected:
    void onVideoFrame(VideoBufferPtr b);

private:
    std::string _trackPath;
    std::thread _commThread;

    std::shared_ptr<TelemetryReader> _tel;

    std::shared_ptr<SerialPortIo> _serial;
    std::shared_ptr<UdpListenIo> _udp;

    std::shared_ptr<VDevice> _video;

    std::shared_ptr<GroupScene>  _scene;
    std::shared_ptr<TrackScene> _trackScene;
    TrackPtr _track;
    std::shared_ptr<TrackController> _trackController;


    std::shared_ptr<BgTexScene>  _bgScene;

    std::shared_ptr<TestBoxScene>  _boxScene;
    std::shared_ptr<Renderer> _renderer;

    boost::asio::io_service _io;

    std::shared_ptr<FfmpegEncoder> _encoder;
    std::shared_ptr<ProcessThread> _compress;
};

#endif // APP_H
