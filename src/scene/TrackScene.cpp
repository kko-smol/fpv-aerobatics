#include "TrackScene.h"
#include <objscene.h>
#include <renderer.h>

TrackScene::TrackScene(ScenePtr checkpoint, TrackPtr track, std::shared_ptr<TrackController> tc, ScenePtr arrow,std::shared_ptr<TelemetryReader> tel):Scene(),
    _track(track),_checkpoint(checkpoint),_tc(tc),_arrow(arrow),_tel(tel)
{
    _distText = std::make_shared<TextScene>(5);
    _timeText = std::make_shared<TextScene>(5);
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
        glm::mat4 rmr = glm::rotate(glm::mat4(1.0),glm::radians(pt.ang().x),glm::vec3(0.0,1.0,0.0));
        glm::mat4 rmp = glm::rotate(glm::mat4(1.0),glm::radians(pt.ang().y+90.0f),glm::vec3(1.0,0.0,0.0));
        glm::mat4 rmh = glm::rotate(glm::mat4(1.0),glm::radians(pt.ang().z),glm::vec3(0.0,0.0,1.0));
        glm::mat4 m = tm*rmr*rmp*rmh*scale;
        _pointMats.push_back(m);
    }

    _arrow->init(renderer());
    _distText->init(renderer());
    _timeText->init(renderer());
    return _checkpoint->init(renderer());
}

void TrackScene::draw(const glm::mat4 &viewMat, const glm::mat4 &projMat, const glm::mat4 modelMat)
{
    ObjScene* s = static_cast<ObjScene*>(_checkpoint.get());
    { //cur target
        s->setColor(glm::vec4(0.0,1.0,0.0,1.0));
        glm::mat4 m = _pointMats[_tc->targetPointId()];

        /*if (1){
            glm::mat4 pm = m;
            printf("REND VIEV:\n");
            for (int y=0;y<4;++y){
                for (int x=0;x<4;++x){
                    printf(" %f ",glm::value_ptr(pm)[y*4+x]);
                }
                printf("\n");
            }
            printf("\n");
        }*/

        _checkpoint->draw(viewMat,projMat,m*modelMat);
    }
    { //next target
        s->setColor(glm::vec4(1.0,0.0,0.0,1.0));
        glm::mat4 m = _pointMats[_tc->nextTargetPointId()];
        _checkpoint->draw(viewMat,projMat,m*modelMat);
    }
    for (int i=0; i<_pointMats.size();++i){
        if ((i!=_tc->nextTargetPointId())||(i!=_tc->targetPointId())){
            s->setColor(glm::vec4(0.5,0.5,0.5,1.0));
            glm::mat4 m = _pointMats[i];
            _checkpoint->draw(viewMat,projMat,m*modelMat);
        }
    }
    {
        auto v = glm::mat4(1.0f);
        auto p = glm::mat4(1.0f);
        auto m = glm::rotate(glm::mat4(1.0f),-(float)M_PI/2.0f+(_tc->nextPointHdg())+glm::radians((float)_tel->lastHeading()),glm::vec3(0.0f,0.0f,1.0f))*
                glm::translate(glm::mat4(1.0f),glm::vec3(0.0,0.0,0.999))*
                glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,0.0f));
        _arrow->draw(v,p,m);
    }
    _distText->setText(std::to_string(_tc->targetDist()));
    {
        auto tflp = _tc->msFromLastPoint();
        auto tstr = std::to_string((float)tflp.count()/1000.0);
        _timeText->setText(tstr);
    }

    _distText->draw(glm::mat4(1.0f),glm::mat4(1.0f),glm::translate(glm::mat4(1.0f),glm::vec3(-0.65,-0.9,0.0))*
                    glm::scale(glm::mat4(1.0f),glm::vec3(0.6,0.08,1.0)));
    _timeText->draw(glm::mat4(1.0f),glm::mat4(1.0f),glm::translate(glm::mat4(1.0f),glm::vec3(-0.50,-0.75,0.0))*
                    glm::scale(glm::mat4(1.0f),glm::vec3(0.9,0.08,1.0)));
}
