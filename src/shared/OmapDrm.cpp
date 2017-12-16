#include "OmapDrm.h"
#include <cstring>

extern "C" {
#include <cstdio>
}

OmapDrm::OmapDrm()
{

}

std::vector<std::shared_ptr<VideoBuffer> > OmapDrm::createNativeBuffers(int w, int h, int bpp, int count, int fourcc)
{
    std::vector<std::shared_ptr<VideoBuffer>> res;
    for (int i=0; i<count;++i){
        NativeBuffer* vb = new NativeBuffer(w,h,bpp,w*bpp,fourcc);

        res.push_back(std::shared_ptr<VideoBuffer>(vb));
    }
    return res;
}

//std::vector<std::shared_ptr<VideoBuffer>> OmapDrm::createOmapBuffers(int w, int h, int bpp, int count, int fourcc)
//{
//    std::vector<std::shared_ptr<VideoBuffer>> res;
//    for (int i=0; i<count;++i){
//        OmapBuffer* vb = new OmapBuffer(w,h,bpp,w*bpp,fourcc,omap);
//
//        res.push_back(std::shared_ptr<VideoBuffer>(vb));
//
//        printf("Omap BO created %p %p %08x\n",vb,vb->bo(),vb->getDmabuf());
//    }
//    return res;
//}
//
//std::vector<std::shared_ptr<VideoBuffer> > OmapDrm::createCmemBuffers(int w, int h, int bpp, int count, int fourcc)
//{
//    std::vector<std::shared_ptr<VideoBuffer>> res;
//    for (int i=0; i<count;++i){
//        CmemBuffer* vb = new CmemBuffer(w,h,bpp,w*bpp,fourcc,2);
//
//        res.push_back(std::shared_ptr<VideoBuffer>(vb));
//
//        printf("Cmem BO created %p %p %08x\n",vb,vb->getDataPtr(),vb->getDmabuf());
//    }
//    return res;
//}

