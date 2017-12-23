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
#ifndef  __arm__
#include "../tests/Mocks/telemetrysourcemock.h"
#else
#include <telemetryreader.h>
#endif
#include "io/serialportio.h"
#include <thread>

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
#ifndef __arm__
    std::shared_ptr<TelemetryReaderMock> _tel;
#else
    std::shared_ptr<TelemetryReader> _tel;
#endif
    std::shared_ptr<SerialPortIo> _serial;

    std::shared_ptr<VDevice> _video;

    std::shared_ptr<GroupScene>  _scene;
    std::shared_ptr<TrackScene> _trackScene;
    TrackPtr _track;


    std::shared_ptr<BgTexScene>  _bgScene;
    std::shared_ptr<ObjScene>    _obj;
    std::shared_ptr<TestBoxScene>  _boxScene;
    std::shared_ptr<Renderer> _renderer;

    boost::asio::io_service _io;
};

#endif // APP_H
