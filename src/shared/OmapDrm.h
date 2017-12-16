#ifndef OMAPDRM_H
#define OMAPDRM_H

#include "Frame.h"
#include <vector>
#include <memory>

struct omap_device;
struct gbm_device;

class OmapDrm
{
public:
    OmapDrm();

    std::vector<std::shared_ptr<VideoBuffer>> createNativeBuffers(int w, int h, int bpp, int count, int fourcc);

    int drmFd;

};

#endif // OMAPDRM_H
