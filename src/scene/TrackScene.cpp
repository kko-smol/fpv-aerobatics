#include "TrackScene.h"

TrackScene::TrackScene(ScenePtr checkpoint, TrackPtr track):Scene(),_track(track),_checkpoint(checkpoint)
{

}

int TrackScene::initEgl()
{
    glCheckError();

    _pointMats.clear();
    for (size_t i=0; i<_track->points().size();++i){
        const TrackPoint& pt = _track->points().at(i);
        float checkPointSize = 20.0;
        glm::mat4 scale = glm::scale(glm::mat4(1.0),glm::vec3(checkPointSize,checkPointSize,checkPointSize));
        glm::mat4 tm = glm::translate(glm::mat4(1.0),pt.pos());
        glm::mat4 rmr = glm::mat4(1.0);//glm::rotate(glm::mat4(1.0),pt.ang().x,glm::vec3(0.0,1.0,0.0));
        glm::mat4 rmp = glm::mat4(1.0);//glm::rotate(glm::mat4(1.0),pt.ang().y,glm::vec3(1.0,0.0,0.0));
        glm::mat4 rmh = glm::mat4(1.0);//glm::rotate(glm::mat4(1.0),pt.ang().z,glm::vec3(0.0,0.0,1.0));
        glm::mat4 m = rmh*rmp*rmr*tm*scale;
        _pointMats.push_back(m);
    }

    return _checkpoint->initEgl();;
}

void TrackScene::draw(const glm::mat4 &viewMat, const glm::mat4 &projMat, const glm::mat4 modelMat)
{
    for (size_t i=0; i<_pointMats.size();++i){
        glm::mat4 m = _pointMats[i];

        /*printf("\nObj %u \n",i);
        for (int y=0;y<4;++y){
            for (int x=0;x<4;++x){
                printf(" %f ",glm::value_ptr(m)[y*4+x]);
            }
            printf("\n");
        }
        printf("\n");*/

        _checkpoint->draw(viewMat,projMat,m*modelMat);
    }
}
