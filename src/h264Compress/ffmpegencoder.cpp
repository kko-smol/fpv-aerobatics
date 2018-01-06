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
    _p->_ctx->time_base = (AVRational){1,30};
    _p->_ctx->max_b_frames = 1;
    _p->_ctx->gop_size = 30;
    _p->_ctx->pix_fmt = AV_PIX_FMT_NV12;
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

    EncodeContainer* ec = static_cast<EncodeContainer*>(buf.get());
    VideoBuffer* vb = ec->_frameData.get();
    vb->beginReadPtr();

    uint8_t* frameData = vb->getDataPtr();

    auto cscs = std::chrono::high_resolution_clock::now();

    //fill NV12 from YUYV
    for (size_t y = 0; y<_p->_ctx->height;y=y+2){
        const uint8_t* const readLine1Start = frameData + (y+0)*_p->_ctx->width*2;
        const uint8_t* const readLine2Start = frameData + (y+1)*_p->_ctx->width*2;
        uint8_t* const writeYLine1Start = _p->_frame->data[0] + (y+0)*_p->_frame->linesize[0];
        uint8_t* const writeYLine2Start = _p->_frame->data[0] + (y+1)*_p->_frame->linesize[0];
        uint8_t* const writeCbCrLineStart = _p->_frame->data[1] + (y/2)*_p->_frame->linesize[1];

        for (size_t x = 0; x<_p->_ctx->width;x=x+4){
            writeYLine1Start[x + 0] = readLine1Start[2*(x+0)];
            writeYLine1Start[x + 1] = readLine1Start[2*(x+1)];
            writeYLine1Start[x + 2] = readLine1Start[2*(x+2)];
            writeYLine1Start[x + 3] = readLine1Start[2*(x+3)];
            writeYLine2Start[x + 0] = readLine2Start[2*(x+0)];
            writeYLine2Start[x + 1] = readLine2Start[2*(x+1)];
            writeYLine2Start[x + 2] = readLine2Start[2*(x+2)];
            writeYLine2Start[x + 3] = readLine2Start[2*(x+3)];

            writeCbCrLineStart[x+0]=(readLine1Start[2*(x+0)+1]+readLine2Start[2*(x+0)+1])/2;
            writeCbCrLineStart[x+1]=(readLine1Start[2*(x+0)+3]+readLine2Start[2*(x+0)+3])/2;
            writeCbCrLineStart[x+2]=(readLine1Start[2*(x+2)+1]+readLine2Start[2*(x+2)+1])/2;
            writeCbCrLineStart[x+3]=(readLine1Start[2*(x+2)+3]+readLine2Start[2*(x+2)+3])/2;
        }
    }
    auto csce = std::chrono::high_resolution_clock::now();
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
    auto ee = std::chrono::high_resolution_clock::now();

    std::cout << "\nH264: " << std::chrono::duration_cast<std::chrono::microseconds>(csce-cscs).count() << " " <<
                 std::chrono::duration_cast<std::chrono::microseconds>(ee-csce).count() << "\n";

    return DataContainerPtr();
}

