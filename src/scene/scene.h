#ifndef SCENE_H
#define SCENE_H

#include <esUtil.h>
#include <Frame.h>
#include <memory>

class Scene {
public:
    Scene();

    int initEgl();
    void updateBgTexture(VideoBufferPtr b);
    void draw(const ESMatrix &viewMat);

private:
    int _texVShader;
    int _texPShader;
    int _texShaderPrg;
    int _texViewMatU;
    int _texModelMatU;
    int _textureU;
    uint _textureId;

    int _vecVShader;
    int _vecPShader;
    int _vecShaderPrg;
    int _vecViewMatU;
    int _vecModelMatU;
};

typedef std::shared_ptr<Scene> ScenePtr;

#endif
