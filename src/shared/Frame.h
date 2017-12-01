#ifndef FRAME_H
#define FRAME_H

#include <inttypes.h>
#include <stdlib.h>
#include <linux/videodev2.h>
#include <memory>
#include <vector>
#include <map>

struct omap_bo;

class VideoBuffer {
public:
    VideoBuffer(int w, int h, int bpp, int pitch, int fourcc, int dmabuf);
    virtual ~VideoBuffer();

    virtual void beginReadPtr() = 0;
    virtual void endReadPtr() = 0;

    virtual void beginWritePtr() = 0;
    virtual void endWritePtr() = 0;

    uint8_t *getDataPtr() const;

    int getDmabuf() const;

    uint32_t getBpp() const;

    uint32_t getWidth() const;

    uint32_t getHeight() const;

    size_t getLength() const;

    uint32_t getPitch() const;

    uint32_t getFourcc() const;

    uint64_t getUserData(void* key) const;
    void setUserData(void* key,const uint64_t &value);

    std::shared_ptr<VideoBuffer> getSlaveBuffer() const;
    void setSlaveBuffer(const std::shared_ptr<VideoBuffer> &slaveBuffer);

protected:

    uint8_t* dataPtr;
    int dmabuf;

    uint32_t bpp, width, height;
    size_t length;
    uint32_t pitch;
    uint32_t fourcc;

    std::map<void*,uint64_t> _userData;
    std::shared_ptr<VideoBuffer> slaveBuffer_;
};

class MmapBuffer : public VideoBuffer {
public:
    MmapBuffer(int w, int h, int bpp, int pitch, int fourc, int fd, int offset);
    ~MmapBuffer();

    // VideoBuffer interface
public:
    void beginReadPtr();
    void endReadPtr();
    void beginWritePtr();
    void endWritePtr();

private:
    int _fd;
    int _offset;
};

class NativeBuffer : public VideoBuffer {
public:
    NativeBuffer(int w, int h, int bpp, int pitch, int fourc);
    ~NativeBuffer();

    // VideoBuffer interface
public:
    virtual void beginReadPtr() override;
    virtual void endReadPtr() override;
    virtual void beginWritePtr() override;
    virtual void endWritePtr() override;

};


//class OmapBuffer : public VideoBuffer {
//public:
//    OmapBuffer(int w, int h, int bpp, int pitch, int fourc, struct omap_device* omap);
//    ~OmapBuffer();
//
//    operator bool();
//
//    void* map();
//    uint32_t handle();
//
//    // VideoBuffer interface
//public:
//    virtual void beginReadPtr() override;
//    virtual void endReadPtr() override;
//    virtual void beginWritePtr() override;
//    virtual void endWritePtr() override;
//
//    omap_bo *bo() const;
//
//private:
//    struct omap_device* _omap;
//    struct omap_bo* bo_;
//};

//class CmemBuffer : public VideoBuffer {
//
//    // VideoBuffer interface
//public:
//    CmemBuffer(int w, int h, int bppp, int p, int fourc, uint8_t block);
//    CmemBuffer(int w, int h, int bppp, int p, int fourc, uint64_t pddr, bool import); //bool import only for make different in constructors
//    ~CmemBuffer();
//
//    uint64_t getPaddr() const;
//
//    virtual void beginReadPtr() override;
//    virtual void endReadPtr() override;
//    virtual void beginWritePtr() override;
//    virtual void endWritePtr() override;
//private:
//    bool _isAlloced;
//    uint64_t _paddr;
//};

typedef std::shared_ptr<VideoBuffer> VideoBufferPtr;
typedef std::vector<VideoBufferPtr> VideoBuffers;

#endif
