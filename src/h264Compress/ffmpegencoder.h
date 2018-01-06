#ifndef FFMPEGENCODER_H
#define FFMPEGENCODER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <Frame.h>
#include <glm/vec3.hpp>
#include <ProcessThread.h>

class EncoderPrivate;
class FfmpegEncoder : public Processor
{
public:
    FfmpegEncoder(int w, int h, std::string file);
    ~FfmpegEncoder();

    class EncodeContainer {
    public:
        VideoBufferPtr _frameData;
        glm::vec3 _angles;
        glm::vec3 _pos;
    };

    // Processor interface
public:
    virtual DataContainerPtr process(DataContainerPtr buf) override;

private:
    EncoderPrivate* _p;
};

#endif // FFMPEGENCODER_H
