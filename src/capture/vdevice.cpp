
#include <string>
#include <memory>
#include <iostream>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#include "vdevice.h"

using namespace std;

VDevice::VDevice()
    : fd_(-1)
    , width_(defaultWidth())
    , height_(defaultHeight())
    , _playing(false)
    , skip_(false)
    , exit_(false)
{

}

VDevice::~VDevice()
{
    close();
}

bool VDevice::open(const string& path)
{
    std::unique_lock<std::mutex> lock(_mtx);
    _path = path;
    fd_ = ::open(path.c_str(), O_RDWR | O_NONBLOCK );
    if (fd_ < 0) {
        std::cout << "Fail open" << std::endl;
        return false;
    }

    return true;
}

void VDevice::close()
{
    std::unique_lock<std::mutex> lock(_mtx);
    if (fd_ < 0) {
        return;
    }

    ::close(fd_);
    fd_ = -1;
}

bool VDevice::init(int w, int h, int n)
{
    std::unique_lock<std::mutex> lock(_mtx);

    width_  = w;
    height_ = h;

    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));

    if (!ioctl(VIDIOC_QUERYCAP, &cap)) {
        std::cout << "QC F" << errno << std::endl;
        return false;
    }

    std::cout << "QC OK" << std::endl;

    struct v4l2_format fmt = {
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
                .fmt = {
            .pix_mp = {
                .width = width_,
                .height = height_,
                .pixelformat = V4L2_PIX_FMT_NV12,
                .field = V4L2_FIELD_ANY,
                .colorspace = V4L2_COLORSPACE_RAW,
            },
        },
    };

    if (!ioctl(VIDIOC_S_FMT, &fmt)) {
        std::cout << "VIDIOC_S_FMT failed (" << errno << ")" << std::endl;
        return false;
    }

    struct v4l2_requestbuffers req;
    req.count = n;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    /* Initiate Memory Mapping */
    if (!ioctl(VIDIOC_REQBUFS, &req)) {
        return false;
    }

    buffers_.clear();
    for (size_t i = 0; i < n; i++){
        printf("Init %u buffer\n",i);
        v4l2_buffer v4lbuffer;
        memset(&v4lbuffer,0,sizeof(v4lbuffer));
        v4lbuffer.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4lbuffer.memory = V4L2_MEMORY_MMAP;
        v4lbuffer.index  = i;
        v4lbuffer.m.fd= 0;

        if (!ioctl(VIDIOC_QUERYBUF, &v4lbuffer)){
            std::cout << "VIDIOC_QUERYBUF failed " << errno << " on" << _path << std::endl;
            return false;
        }

        auto nb = std::make_shared<MmapBuffer>(w,h,2,2*w,V4L2_PIX_FMT_NV12,fd_,v4lbuffer.m.offset);
        Buffer buffer{nb,v4lbuffer};
        nb->setUserData(this,i);
        buffers_.push_back(buffer);
    }

    std::cout << "SET_FMT OK" << std::endl;
    return true;
}

bool VDevice::start(bool play)
{
    _playing = play;

    if (_playing){
        resume();
    }


    thread_ = thread([this]() {
        loop();
    });

    return true;
}


bool VDevice::stop()
{
    std::unique_lock<std::mutex> lock(_mtx);
    exit_ = true;

    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (!ioctl(VIDIOC_STREAMOFF, &type)) {
    }

    _condVar.notify_all();

    if (thread_.joinable()) {
        thread_.join();
    }
    return true;
}

bool VDevice::pause()
{
    std::unique_lock<std::mutex> lock(_mtx);

    _playing = false;
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (!ioctl(VIDIOC_STREAMOFF, &type)) {
        return false;
    }
    return true;
}

bool VDevice::resume()
{
    {
        for (int i=0; i<buffers_.size();++i){
            queue(buffers_.at(i).vb);
        }
    }

    {
        std::unique_lock<std::mutex> lock(_mtx);
        v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (!ioctl(VIDIOC_STREAMON, &type)) {
            std::cout << "FAIL STREAMON" << errno << std::endl;
            return false;
        }

        _condVar.notify_one();
    }

    return true;
}

bool VDevice::ioctl(int request, void* arg)
{
    if (fd_ < 0) {
        return false;
    }

    int r;
    //do {
    r = ::ioctl(fd_, request, arg);
    //} while ( r < 0 && errno == EINTR);

    return (r == 0);
}



void VDevice::loop()
{
    exit_ = false;

    int errcnt = 0;
    while (!exit_) {

        if (!_playing){
            std::unique_lock<std::mutex> lock(_mtx);
            {
                while((!_playing)&&(!exit_)){
                    _condVar.wait(lock);
                }
            }
        }

        {
            fd_set fds;
            {
                FD_ZERO(&fds);
                FD_SET(fd_, &fds);
            }

            struct timeval tv;
            tv.tv_sec  = 1;
            tv.tv_usec = 0;

            int r = select(fd_ + 1, &fds, nullptr, nullptr, &tv);

            if ((r > 0)) {
                struct v4l2_buffer buf;
                memset(&buf, 0, sizeof(buf));
                buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;

                if (ioctl(VIDIOC_DQBUF, &buf)) {
                    buffers_.at(buf.index).vb->setUserData(this+1,0);
                    if (_playing & !exit_){
                        //emit frame
                        sig_frame_(buffers_.at(buf.index).vb);
                    } else {
                        //release buffer
                        if (!exit_){
                            queue(buffers_.at(buf.index).vb);
                        }
                    }
                } else {
                    errcnt++;
                    std::cout << "VD E1 " << errno << std::endl;
                    std::this_thread::sleep_for(100ms);
                }
            } else if (r == 0) {
                std::this_thread::sleep_for(100ms);
            } else {
                errcnt++;
                std::cout << "VD E2 " << errno << std::endl;
                std::this_thread::sleep_for(100ms);
            }
            if ((errcnt>1000)&&(!exit_)){
                std::cout << "REINIT" << std::endl;
                reinit();
                errcnt=0;
            }
        }
    }
}



boost::signals2::connection VDevice::connectFrame(const SignalFrame::slot_type& slot)
{
    return sig_frame_.connect(slot);
}

void VDevice::reinit()
{
    bool p = _playing;
    pause();
    close();
    open(_path);
    VideoBuffers buffs;
    for (int i=0; i<buffers_.size();++i){
        buffs.push_back(buffers_.at(i).vb);
    }
    init(width_,height_,buffers_.size());
    if (p){
        resume();
    }
}

bool VDevice::queue(std::shared_ptr<VideoBuffer> buf)
{
    std::unique_lock<std::mutex> lock(_mtx);
    if (!exit_){
        int bufId = buf->getUserData(this);

        struct v4l2_buffer buffer;
        memset(&buffer, 0, sizeof(buffer));
        buffer.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index  = bufId;
        if (!ioctl(VIDIOC_QBUF, &buffer)){
            std::cout << "VIDIOC_QBUF failed " << errno << " on" << _path << std::endl;
            return false;
        }
    }
    return true;
}

