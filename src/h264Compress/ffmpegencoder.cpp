#include "ffmpegencoder.h"
#include <fstream>
#include <iostream>

extern "C"{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
}

class EncoderPrivate{
public:
    EncoderPrivate(){
        _codec = nullptr;
        _opened =false;
    }
    AVCodec * _codec;
    AVCodecContext * _ctx= NULL;
    size_t _fc;
    AVFrame* _frame;
    std::ofstream _file;
    bool _opened;
};

FfmpegEncoder::FfmpegEncoder(int w, int h, std::string file){
    //avcodec_init();
    avcodec_register_all();
    _p = new EncoderPrivate();

    _p->_codec = avcodec_find_encoder_by_name("cedrus264");

    if (!_p->_codec){
        _p->_codec = avcodec_find_encoder_by_name("libx264");
    }
    if (!_p->_codec) {
        fprintf(stderr, "Codec not found\n");
        return;
    } else {
        fprintf(stderr, "Codec found: %s - %s\n",_p->_codec->name,_p->_codec->long_name);
    }
    _p->_ctx = avcodec_alloc_context3(_p->_codec);
    if (!_p->_ctx) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return;
    }
    _p->_ctx->bit_rate = 4000000; //4Mbit
    _p->_ctx->width = w;
    _p->_ctx->height = h;
    _p->_ctx->time_base = (AVRational){1,25};
    _p->_ctx->max_b_frames = 1;
    _p->_ctx->gop_size = 25;
    _p->_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    av_opt_set(_p->_ctx->priv_data, "preset", "ultrafast", 0);

    if (avcodec_open2(_p->_ctx, _p->_codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return;
    }

    _p->_frame = avcodec_alloc_frame();
    if (!_p->_frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        return;
    }
    _p->_frame->format = _p->_ctx->pix_fmt;
    _p->_frame->width  = _p->_ctx->width;
    _p->_frame->height = _p->_ctx->height;

    auto ret = av_image_alloc(_p->_frame->data, _p->_frame->linesize, _p->_frame->width, _p->_frame->height,
                         _p->_ctx->pix_fmt, 32);
    if (ret>0){
        _p->_opened = true;
    }

    _p->_file = std::ofstream(file,std::ios::out | std::ios::binary | std::ios::app);
}

FfmpegEncoder::~FfmpegEncoder()
{

}

DataContainerPtr FfmpegEncoder::process(DataContainerPtr buf)
{
    AVPacket pkt;
    int got_output;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size=0;
    _p->_frame->pts = _p->_fc++;

    VideoBuffer* vb = static_cast<VideoBuffer*>(buf.get());
    vb->beginReadPtr();

    uint8_t* frameData = vb->getDataPtr();

    //fill NV12 from YUYV
    for (size_t y = 0; y<_p->_ctx->height;y=y+2){
        for (size_t x = 0; x<_p->_ctx->width;x=x+2){
            const uint8_t y00 = frameData[(y+0)*_p->_ctx->width*2 + 2*(x+0)];
            const uint8_t y01 = frameData[(y+0)*_p->_ctx->width*2 + 2*(x+1)];
            const uint8_t y10 = frameData[(y+1)*_p->_ctx->width*2 + 2*(x+0)];
            const uint8_t y11 = frameData[(y+1)*_p->_ctx->width*2 + 2*(x+1)];

            const uint16_t cb0 = frameData[(y+0)*_p->_ctx->width*2 + 2*(x+0)+1];
            const uint16_t cr0 = frameData[(y+0)*_p->_ctx->width*2 + 2*(x+0)+3];

            const uint16_t cb1 = frameData[(y+1)*_p->_ctx->width*2 + 2*(x+0)+1];
            const uint16_t cr1 = frameData[(y+1)*_p->_ctx->width*2 + 2*(x+0)+3];

            _p->_frame->data[0][(y+0)*_p->_frame->linesize[0] + x + 0] = y00;
            _p->_frame->data[0][(y+0)*_p->_frame->linesize[0] + x + 1] = y01;
            _p->_frame->data[0][(y+1)*_p->_frame->linesize[0] + x + 0] = y10;
            _p->_frame->data[0][(y+1)*_p->_frame->linesize[0] + x + 1] = y11;

            _p->_frame->data[1][(y/2)*_p->_frame->linesize[1] + x/2] = (cb0+cb1)/2;
            _p->_frame->data[2][(y/2)*_p->_frame->linesize[2] + x/2] = (cr0+cr1)/2;
        }
    }
    vb->endReadPtr();
    auto ret = avcodec_encode_video2(_p->_ctx, &pkt, _p->_frame, &got_output);
    if (ret < 0) {
        fprintf(stderr, "Error encoding frame\n");
        exit(1);
    }
    if (got_output) {
        _p->_file.write((const char*)pkt.data,pkt.size);
        _p->_file.flush();
        av_free_packet(&pkt);
    }
    return DataContainerPtr();
}

