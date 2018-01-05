#include "app.h"
#include <shaders.h>
#include <iostream>
#include <chrono>
#ifdef __arm__
#include <eglrenderer.h>
#else
#include <glutrendefer.h>
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include <gis/CorrdsConverter.h>
#include "track/TrackReader.h"

App::App()
{

}

bool App::init()
{

#ifdef __arm__
    _serial = std::make_shared<SerialPortIo>(_io,"/dev/ttyS1",115200);
    _tel = std::make_shared<TelemetryReader>();
    _serial->listen(std::static_pointer_cast<IOClient>(_tel));
#else
    _serial = std::make_shared<SerialPortIo>(_io,"/dev/ttyACM0",115200);
    _tel = std::make_shared<TelemetryReader>();
    _serial->listen(std::static_pointer_cast<IOClient>(_tel));
#endif

    //// video capture
    std::string dev = "/dev/video1";
    int vw = 640;
    int vh = 480;
    std::cout << "1" << std::endl;
    _video = std::make_shared<VDevice>();
    if (!_video->open(dev)){
        return false;
    }

    if (!_video->init(vw,vh,4)){
        return false;
    }

    _video->connectFrame([this](VideoBufferPtr buf){
        _io.post([this,buf](){
            this->onVideoFrame(buf);
        });
    });

    _bgScene = std::make_shared<BgTexScene>();
    _boxScene = std::make_shared<TestBoxScene>();

    // Track load
    _trackPath = "/home/kest/track.trk";
    _track = TrackReader::readTrack(_trackPath);
    auto checkpoint = std::make_shared<ObjScene>("/home/kest/test.obj");
    auto arrow = std::make_shared<ObjScene>("/home/kest/arrow.obj");
    _trackController = std::make_shared<TrackController>(_track);

    _trackScene = std::make_shared<TrackScene>(checkpoint,_track,_trackController,arrow, _tel);

    _scene = std::shared_ptr<GroupScene>(new GroupScene({_bgScene,_trackScene}));
    //// egl render
#ifdef __arm__
    _renderer = std::make_shared<EglRenderer>(_scene);
#else
    _renderer = std::make_shared<GlutRenderer>(_scene);
#endif

    std::cout << "7" << std::endl;
    if(!_renderer->init()){
        return false;
    }

    _tel->subscribe(std::static_pointer_cast<TelemetryListener>(_trackController));

    glm::mat4 proj = glm::perspective(glm::radians(40.0),0.5*720.0/576.0,10.0,10000.0);
    //proj = glm::tweakedInfinitePerspective(glm::radians(45.0),0.5*720.0/576.0,1.0);
    _renderer->setProjMat(proj);

    _encoder = std::make_shared<FfmpegEncoder>(vw,vh);
    _compress =std::make_shared<ProcessThread>(_encoder);

    return true;
}

void App::run()
{
    std::cout << "run" << std::endl;
    if (!_video->start(true)){
        std::cout << "Video run fail" << std::endl;
        return;
    }

    boost::asio::signal_set signals(_io, SIGINT, SIGTERM);
    signals.async_wait(
        boost::bind(&boost::asio::io_service::stop, &_io));
    _io.run();
}

void App::onVideoFrame(VideoBufferPtr b)
{
    _bgScene->updateBgTexture(b);

    glm::vec3 base = (_track->trackBase());
    glm::vec3 pos = CoordsConverter::toMercator(
                glm::vec3(
                    _tel->lastLon(),
                    _tel->lastLat(),
                    _tel->lastAlt())) - base;

    auto tr = glm::rotate(glm::mat4(1.0),glm::radians((float)_tel->lastRoll()),glm::vec3(0.0,1.0,0.0));
    auto tp = glm::rotate(glm::mat4(1.0),glm::radians((float)_tel->lastPitch()),glm::vec3(1.0,0.0,0.0));
    auto th = glm::rotate(glm::mat4(1.0),glm::radians((float)_tel->lastHeading()),glm::vec3(0.0,0.0,-1.0));

    auto tm = th*tp*tr;

    glm::vec4 los(0.0,1.0,0.0,0.0);
    glm::vec4 top(0.0,0.0,1.0,0.0);

    glm::vec4 losR = tm*los;
    glm::vec4 topR = tm*top;

    glm::vec3 from(pos);

    auto rrm = glm::lookAt(from,from+glm::vec3(losR),glm::vec3(topR));//glm::translate(glm::mat4(1.0f),lp);

    glm::mat4 view = rrm;

    _renderer->setViewMat(view);
    _renderer->render();
    _video->queue(b);
    static auto last = std::chrono::high_resolution_clock::now();
    static int frameCounter = 0;
    ++frameCounter;
    auto newFrameTime = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(newFrameTime-last).count()>5000){
        std::cout << "FPS" << 1000.0*(float)frameCounter/std::chrono::duration_cast<std::chrono::milliseconds>(newFrameTime-last).count() << std::endl;
        frameCounter = 0;
        last = newFrameTime;
    }
    _renderer->exec();
}
