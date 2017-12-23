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

#include <gis/CorrdsConverter.h>
#include "track/TrackReader.h"

App::App()
{

}

bool App::init()
{

    //// video capture
    std::string dev = "/dev/video0";
    int vw = 640;
    int vh = 480;
    std::cout << "1" << std::endl;
    _video = std::make_shared<VDevice>();
    if (!_video->open(dev)){
        return false;
    }

    std::cout << "3" << std::endl;
    if (!_video->init(vw,vh,4)){
        return false;
    }

    std::cout << "4" << std::endl;
    _video->connectFrame([this](VideoBufferPtr buf){
        _io.post([this,buf](){
            this->onVideoFrame(buf);
        });
    });


    std::cout << "5" << std::endl;
    _bgScene = std::make_shared<BgTexScene>();
    _boxScene = std::make_shared<TestBoxScene>();

    // Track load
    _trackPath = "/home/kest/track.trk";
    _track = TrackReader::readTrack(_trackPath);
    _obj = std::make_shared<ObjScene>("/home/kest/test.obj");
    _trackScene = std::make_shared<TrackScene>(_obj,_track);

    _scene = std::shared_ptr<GroupScene>(new GroupScene({_bgScene,/*_obj,*/_trackScene}));
    //// egl render
    std::cout << "6" << std::endl;
#ifdef __arm__
    _renderer = std::make_shared<EglRenderer>(_scene);
#else
    _renderer = std::make_shared<GlutRenderer>(_scene);
#endif

    std::cout << "7" << std::endl;
    if(!_renderer->init()){
        return false;
    }

    _serial = std::make_shared<SerialPortIo>(_io,"/dev/ttyACM0",115200);
#ifndef __arm__
    _tel = std::make_shared<TelemetryReaderMock>(_track);
#else
    _tel = std::make_shared<TelemetryReader>();
#endif
    _serial->listen(std::static_pointer_cast<IOClient>(_tel));

    glm::mat4 proj = glm::perspective(glm::radians(50.0),0.5*720.0/576.0,1.0,2000.0);
    //proj = glm::tweakedInfinitePerspective(glm::radians(45.0),0.5*720.0/576.0,1.0);
    _renderer->setProjMat(proj);
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

    glm::vec3 pos = CoordsConverter::toMercator(
                glm::vec3(
                    _tel->lastLon(),
                    _tel->lastLat(),
                    _tel->lastAlt()));

    glm::mat4 tr = glm::rotate(glm::mat4(1.0f),glm::radians(-(float)_tel->lastRoll()),glm::vec3   (0.0f,1.0f,0.0f));
    glm::mat4 tp = glm::rotate(glm::mat4(1.0f),glm::radians(-(float)_tel->lastPitch()),glm::vec3  (1.0f,0.0f,0.0f));
    glm::mat4 th = glm::rotate(glm::mat4(1.0f),glm::radians(-(float)_tel->lastHeading()),glm::vec3(0.0f,0.0f,1.0f));
    glm::mat4 tm = th*tp*tr;
    glm::vec4 vv(0.0,1.0,0.0,0.0);
    glm::vec4 tv(0.0,0.0,1.0,0.0);

    glm::vec3 base = (_track->trackBase());
    glm::vec3 from(pos - base);
    glm::vec3 to = glm::vec3(tm*vv);
    tv = tm*tv;

    std::cout << "IV" << to.x << "," << to.y << "," << to.z << std::endl;
    std::cout << "OV" << vv.x << "," << vv.y << "," << vv.z << std::endl << std::endl;

    to = from - to;

    glm::vec3 lp = pos-_track->trackBase();

    auto rrm = glm::lookAt(from,to,glm::vec3(tv));//glm::translate(glm::mat4(1.0f),lp);

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
