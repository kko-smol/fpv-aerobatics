#ifndef FFMPEGENCODER_H
#define FFMPEGENCODER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <Frame.h>

#include <ProcessThread.h>

class EncoderPrivate;
class FfmpegEncoder : public Processor
{
public:
    FfmpegEncoder(int w, int h);
    ~FfmpegEncoder();

    // Processor interface
public:
    virtual DataContainerPtr process(DataContainerPtr) override;

private:
    EncoderPrivate* _p;
};

#endif // FFMPEGENCODER_H
