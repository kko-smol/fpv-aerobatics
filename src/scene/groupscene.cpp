#include "groupscene.h"


GroupScene::GroupScene(std::vector<ScenePtr> parts)
{
    _parts = parts;
}

int GroupScene::initEgl()
{
    glCheckError();
    for (size_t i=0; i<_parts.size();++i){
        if (!_parts[i]->initEgl()){
            return 0;
        }
        glCheckError();
    }
    return 1;
}

void GroupScene::draw(const glm::mat4 &viewMat, const glm::mat4 &projMat, const glm::mat4 modelMat)
{
    for (size_t i=0; i<_parts.size();++i){
        _parts[i]->draw(viewMat,projMat,modelMat);
    }
}
