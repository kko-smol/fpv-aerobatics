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
    //_obj = std::make_shared<ObjScene>("/home/kest/test.obj");
    _scene = std::shared_ptr<GroupScene>(new GroupScene({/*_bgScene,*/_boxScene/*,_obj*/}));
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
    _tel = std::make_shared<TelemetryReader>();
    _serial->listen(std::static_pointer_cast<IOClient>(_tel));

    std::cout << "8" << std::endl;
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

    auto rt = glm::rotate(glm::mat4(1.0f),-(float)(_tel->lastRoll()   *M_PI/180.0),glm::vec3(0.0,0.0,1.0));
    auto pt = glm::rotate(glm::mat4(1.0f),-(float)(_tel->lastPitch()  *M_PI/180.0),glm::vec3(1.0,0.0,0.0));
    auto ht = glm::rotate(glm::mat4(1.0f),-(float)(_tel->lastHeading()*M_PI/180.0),glm::vec3(0.0,1.0,0.0));

    auto rrm = rt*pt*ht;

    //std::cout << _tel->lastRoll() << std::endl;

    glm::mat4 proj = glm::perspective(glm::radians(45.0),0.5*720.0/576.0,1.0,1000.0);
    //glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -100.0f, 100.0f);;
    glm::mat4 view = glm::lookAt(glm::vec3(0.0,0.0,-10),glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,1.0,0.0))*rrm;
    //view = glm::rotate(view, 0.0, glm::vec3(-1.0f, 0.0f, 0.0f));
    //view = glm::rotate(view, 0.0, glm::vec3(0.0f, 1.0f, 0.0f));

    _renderer->setProjMat(proj);

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
