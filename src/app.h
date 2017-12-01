#ifndef APP_H
#define APP_H

#include <vdevice.h>
#include <eglrenderer.h>
#include <scene.h>
#include <boost/asio.hpp>

class App
{
public:
    App();

    bool init();

    void run();

protected:
    void onVideoFrame(VideoBufferPtr b);

private:
    std::shared_ptr<VDevice> _video;
    std::shared_ptr<Scene>  _scene;
    std::shared_ptr<EglRenderer> _renderer;

    boost::asio::io_service _io;
};

#endif // APP_H
