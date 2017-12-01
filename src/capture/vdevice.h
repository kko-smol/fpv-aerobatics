
#ifndef VDEVICE_H
#define VDEVICE_H

#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <boost/signals2.hpp>
#include <Frame.h>
#include <queue>
#include <condition_variable>

class VDeviceCmd;

class VDevice {
public:
        typedef boost::signals2::signal<void (std::shared_ptr<VideoBuffer>)> SignalFrame;

        VDevice();
        ~VDevice();

        bool open(const std::string& path);
        void close();

        bool init(int w, int h, int n); //hfr - half frame rate

        bool start(bool play = false);
        bool stop();

        bool pause();
        bool resume();

        bool queue(std::shared_ptr<VideoBuffer> buf);

        boost::signals2::connection connectFrame(const SignalFrame::slot_type&);

private:
        static constexpr size_t defaultWidth()  { return 1920; };
        static constexpr size_t defaultHeight() { return 1080; };

        void reinit();

        std::condition_variable _condVar;
        std::mutex _mtx;
        std::queue<VDeviceCmd*> cmds;

        bool ioctl(int request, void* arg);

        void loop();

        struct Buffer {
                std::shared_ptr<VideoBuffer> vb;
                v4l2_buffer v4lbuffer;
        };

        int fd_;

        size_t width_;
        size_t height_;
        bool _playing;
        std::string _path;

        std::vector<Buffer> buffers_;

        std::atomic<bool> exit_;
        std::thread thread_;

        SignalFrame sig_frame_;

        uint8_t skip_;
};



#endif // VDEVICE_H

