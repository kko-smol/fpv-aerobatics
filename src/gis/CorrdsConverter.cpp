#include "CorrdsConverter.h"
#include "proj.4-master/src/proj_api.h"

namespace CoordsConverter {

static const std::string mercatorSrs("+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0.0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs +over");
static const std::string longLatSrs("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");

glm::vec3 toMercator(glm::vec3 p)
{

    double x=p.x,y=p.y;

    x=x*DEG_TO_RAD;
    y=y*DEG_TO_RAD;

    projPJ s = pj_init_plus(longLatSrs.c_str());
    projPJ d = pj_init_plus(mercatorSrs.c_str());
    if ((s)&&(d)){
        pj_transform(s, d, 1, 1, &x, &y, NULL);
        pj_free(s);
        pj_free(d);
        return glm::vec3(x, y,p.z);
    } else {
        if (s)
            pj_free(s);
        if (d)
            pj_free(d);
        return glm::vec3(0.0,0.0,0.0);
    }
}

glm::vec3 toLongLat(glm::vec3 p)
{

    double x=p.x,y=p.y;

    projPJ s = pj_init_plus(mercatorSrs.c_str());
    projPJ d = pj_init_plus(longLatSrs.c_str());
    if ((s)&&(d)){
        pj_transform(s, d, 1, 1, &x, &y, NULL);
        x=x*RAD_TO_DEG;
        y=y*RAD_TO_DEG;
        pj_free(s);
        pj_free(d);
        return glm::vec3(x,y,p.z);
    } else {
        if (s)
            pj_free(s);
        if (d)
            pj_free(d);
        return glm::vec3(0.0,0.0,0.0);
    }
}

}
