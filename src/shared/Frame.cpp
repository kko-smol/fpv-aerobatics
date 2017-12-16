#include "Frame.h"
#include <cstdio>
extern "C"{
#include <sys/mman.h>
}
#include <assert.h>
#include <string.h>

VideoBuffer::VideoBuffer(int w, int h, int bpp, int pitch, int fourcc, int dmabuf):dataPtr(0),dmabuf(dmabuf),bpp(bpp),width(w),height(h),length(w*h*bpp),
    pitch(pitch),fourcc(fourcc)
{

}

VideoBuffer::~VideoBuffer()
{

}

uint8_t *VideoBuffer::getDataPtr() const
{
    return dataPtr;
}

int VideoBuffer::getDmabuf() const
{
    return dmabuf;
}

uint32_t VideoBuffer::getBpp() const
{
    return bpp;
}

uint32_t VideoBuffer::getWidth() const
{
    return width;
}

uint32_t VideoBuffer::getHeight() const
{
    return height;
}

size_t VideoBuffer::getLength() const
{
    return length;
}

uint32_t VideoBuffer::getPitch() const
{
    return pitch;
}

uint32_t VideoBuffer::getFourcc() const
{
    return fourcc;
}

uint64_t VideoBuffer::getUserData(void *key) const
{
    if (_userData.count(key)){
        return _userData.at(key);
    } else {
        return 0;
    }
}

void VideoBuffer::setUserData(void *key, const uint64_t &value)
{
    _userData.emplace(key,value);
}


std::shared_ptr<VideoBuffer> VideoBuffer::getSlaveBuffer() const
{
    return slaveBuffer_;
}

void VideoBuffer::setSlaveBuffer(const std::shared_ptr<VideoBuffer> &slaveBuffer)
{
    slaveBuffer_ = slaveBuffer;
}

//OmapBuffer::OmapBuffer(int w, int h, int bpp, int pitch, int fourcc,struct omap_device* omap ):VideoBuffer(w,h,bpp,pitch,fourcc,0),_omap(omap)
//{
//    uint32_t bo_flags = 0;
//    bo_flags |= OMAP_BO_WC;
//    bo_ = omap_bo_new(_omap,w*h*bpp,bo_flags);
//    assert(bo_ != nullptr);
//    length = omap_bo_size(bo_);
//    dmabuf = omap_bo_dmabuf(bo_);
//}
//
//OmapBuffer::~OmapBuffer()
//{
//    omap_bo_del(bo_);
//    dataPtr = 0;
//    dmabuf  = 0;
//    bpp     = 0;
//    width   = 0;
//    height  = 0;
//    length  = 0;
//    pitch   = 0;
//    fourcc  = 0;
//}
//
//void *OmapBuffer::map()
//{
//    assert(bo_ != nullptr);
//    if (!dataPtr) {
//        dataPtr = (uint8_t*)omap_bo_map(bo());
//    }
//    return dataPtr;
//}
//
//uint32_t OmapBuffer::handle()
//{
//    assert(bo_ != nullptr);
//    return omap_bo_handle(bo_);
//}
//
//void OmapBuffer::beginReadPtr()
//{
//    omap_bo_cpu_prep(bo(),(enum omap_gem_op)(OMAP_GEM_READ));
//    if (dataPtr==nullptr){
//        dataPtr = (uint8_t*)omap_bo_map(bo());
//    }
//}
//
//void OmapBuffer::endReadPtr()
//{
//    omap_bo_cpu_fini(bo(),(enum omap_gem_op)(OMAP_GEM_READ));
//}
//
//void OmapBuffer::beginWritePtr()
//{
//    omap_bo_cpu_prep(bo(),(enum omap_gem_op)(OMAP_GEM_WRITE));
//    if (dataPtr==nullptr){
//        dataPtr = (uint8_t*)omap_bo_map(bo());
//    }
//}
//
//void OmapBuffer::endWritePtr()
//{
//    omap_bo_cpu_fini(bo(),(enum omap_gem_op)(OMAP_GEM_WRITE));
//}
//
//omap_bo *OmapBuffer::bo() const
//{
//    return bo_;
//}
//
//CmemBuffer::CmemBuffer(int w, int h, int bppp, int p, int fourc, uint8_t block):VideoBuffer(w,h,bppp,p,fourcc,0)
//{
//    bpp = bppp;
//    width = w;
//    height = h;
//    length = w*h*bpp;
//    pitch = p;
//    fourcc = fourc;
//
//    CMEM_AllocParams cfg;
//    cfg.alignment = 0;
//    cfg.flags = CMEM_CACHED;
//    cfg.type = CMEM_POOL;
//    dataPtr = (uint8_t*)CMEM_alloc2(block,length,&cfg);
//    memset(dataPtr,0,length);
//    dmabuf = CMEM_export_dmabuf(dataPtr);
//    _paddr = (uint64_t)CMEM_getPhys(dataPtr);
//
//    _isAlloced = true;
//}
//
//CmemBuffer::CmemBuffer(int w, int h, int bppp, int p, int fourc, uint64_t pddr, bool import):VideoBuffer(w,h,bpp,p,fourcc,0)
//{
//    bpp = bppp;
//    width = w;
//    height = h;
//    length = w*h*bpp;
//    pitch = p;
//    fourcc = fourc;
//    _paddr = pddr;
//    printf("Import cmem %p\n",_paddr);
//    dataPtr = (uint8_t*)CMEM_registerAlloc((off_t)_paddr);
//    dmabuf = CMEM_export_dmabuf(dataPtr);
//    _isAlloced = false;
//}
//
//CmemBuffer::~CmemBuffer()
//{
//    CMEM_AllocParams cfg;
//    cfg.alignment = 64;
//    cfg.flags = CMEM_CACHED;
//    cfg.type = CMEM_POOL;
//    CMEM_free(dataPtr,&cfg);
//}
//
//uint64_t CmemBuffer::getPaddr() const
//{
//    return _paddr;
//}
//
//void CmemBuffer::beginReadPtr()
//{
//    CMEM_cacheInv(dataPtr,length);
//}
//
//void CmemBuffer::endReadPtr()
//{
//
//}
//
//void CmemBuffer::beginWritePtr()
//{
//    CMEM_cacheInv(dataPtr,length);
//}
//
//void CmemBuffer::endWritePtr()
//{
//    CMEM_cacheWb(dataPtr,length);
//}

NativeBuffer::NativeBuffer(int w, int h, int bpp, int pitch, int fourc):VideoBuffer(w,h,bpp,w*bpp,fourc,0)
{
    dataPtr = (uint8_t*)malloc(w*h*bpp);
}

NativeBuffer::~NativeBuffer()
{
    free(dataPtr);
}

void NativeBuffer::beginReadPtr()
{

}

void NativeBuffer::endReadPtr()
{

}

void NativeBuffer::beginWritePtr()
{

}

void NativeBuffer::endWritePtr()
{

}

MmapBuffer::MmapBuffer(int w, int h, int bpp, int pitch, int fourc, int fd, int offset):VideoBuffer(w,h,bpp,w*bpp,fourc,0)
{
    _fd = fd;
    _offset = offset;
    dataPtr = (uint8_t*)mmap(NULL,w*h*bpp,PROT_READ|PROT_WRITE,MAP_SHARED,fd,offset);
}

MmapBuffer::~MmapBuffer()
{

}

void MmapBuffer::beginReadPtr()
{

}

void MmapBuffer::endReadPtr()
{

}

void MmapBuffer::beginWritePtr()
{

}

void MmapBuffer::endWritePtr()
{

}
