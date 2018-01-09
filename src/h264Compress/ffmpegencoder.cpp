#include "ffmpegencoder.h"
#include <fstream>
#include <iostream>
#include <map>

extern "C"{
#include "h264bitstream-master/h264_stream.h"
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
}

struct FrameTelemetry
{
    uint32_t _frameNum;
    glm::vec3 _gps;
    glm::vec3 _att;
};

class EncoderPrivate{
public:
    EncoderPrivate(){
        _codec = nullptr;
        _opened =false;
        _h264 = h264_new();
        _h264->num_seis=1;
        _h264->seis = new sei_t*;
        _h264->seis[0] = new sei_t;
    }
    AVCodec * _codec;
    AVCodecContext * _ctx= NULL;
    size_t _fc;
    AVFrame* _frame;
    std::ofstream _file;
    bool _opened;
    std::map<size_t,FrameTelemetry> _tq;
    h264_stream_t* _h264;
    std::chrono::high_resolution_clock::time_point _start;
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
    _p->_start = std::chrono::high_resolution_clock::now();
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

    std::shared_ptr<EncodeContainer> ec = std::static_pointer_cast<EncodeContainer>(buf);
    VideoBuffer* vb = ec->_frameData.get();
    vb->beginReadPtr();

    uint8_t* frameData = vb->getDataPtr();

    auto cscs = std::chrono::high_resolution_clock::now();
    uint32_t pts = std::chrono::duration_cast<std::chrono::milliseconds>(cscs-_p->_start).count();
    _p->_frame->pts = pts/(1000.0/30.0);

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
    FrameTelemetry ft;
    ft._frameNum = _p->_frame->pts;
    ft._att = ec->_angles;
    ft._gps = ec->_pos;
    _p->_tq.insert({_p->_frame->pts,ft});
    auto ret = avcodec_encode_video2(_p->_ctx, &pkt, _p->_frame, &got_output);
    if (ret < 0) {
        fprintf(stderr, "Error encoding frame\n");
        exit(1);
    }
    if (got_output) {
        FrameTelemetry ft = _p->_tq[pkt.pts];
        _p->_tq.erase(pkt.pts);
        _p->_file.write((const char*)pkt.data,pkt.size);

        av_free_packet(&pkt);
        //write telemetry frame
        _p->_h264->seis[0]->payloadType = SEI_TYPE_USER_DATA_UNREGISTERED;
        _p->_h264->seis[0]->payloadSize = sizeof(FrameTelemetry);
        _p->_h264->seis[0]->data = (uint8_t*)&ft;
        _p->_h264->nal->forbidden_zero_bit = 0;
        _p->_h264->nal->nal_ref_idc = 0;
        _p->_h264->nal->nal_unit_type = NAL_UNIT_TYPE_SEI;

        uint8_t seibuf[128];

        //3 not 4 that write_nal_unit add 0x00 before data
        int n = write_nal_unit(_p->_h264,&seibuf[3],128-4);
        seibuf[0] = 0x00;
        seibuf[1] = 0x00;
        seibuf[2] = 0x00;
        seibuf[3] = 0x01;
        _p->_file.write((const char*)seibuf,n+3);

        _p->_file.flush();
    }

    return DataContainerPtr();
}

