#include "app.h"
#include <shaders.h>
#include <iostream>

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
        this->onVideoFrame(buf);
    });


    std::cout << "5" << std::endl;
    _scene = std::make_shared<Scene>();
    //// egl render
    std::cout << "6" << std::endl;
    _renderer = std::make_shared<EglRenderer>(_scene);

    std::cout << "7" << std::endl;
    if(!_renderer->init()){
        return false;
    }
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
    std::cout << "Frame" << std::endl;
    //_scene->updateBgTexture(b);
    //_renderer->render();
    _video->queue(b);
}
