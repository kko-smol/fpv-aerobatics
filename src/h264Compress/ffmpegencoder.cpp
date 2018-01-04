#include "ffmpegencoder.h"

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
    bool _opened;
};

FfmpegEncoder::FfmpegEncoder(int w, int h){
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
    _p->_ctx->time_base = (AVRational){1,30};
    _p->_ctx->max_b_frames = 1;
    _p->_ctx->pix_fmt = AV_PIX_FMT_YUYV422;
    av_opt_set(_p->_ctx->priv_data, "preset", "realtime", 0);

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
}

FfmpegEncoder::~FfmpegEncoder()
{

}

DataContainerPtr FfmpegEncoder::process(DataContainerPtr)
{
    AVPacket pkt;
    int got_output;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size=0;
    _p->_frame->pts = _p->_fc++;

    auto ret = avcodec_encode_video2(_p->_ctx, &pkt, _p->_frame, &got_output);
    if (ret < 0) {
        fprintf(stderr, "Error encoding frame\n");
        exit(1);
    }
    if (got_output) {
        //printf("Write frame %3d (size=%5d)\n", i, pkt.size);
        //fwrite(pkt.data, 1, pkt.size, f);
        av_free_packet(&pkt);
    }
}

