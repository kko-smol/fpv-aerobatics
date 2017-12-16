#ifndef BGTEXSCENE_H
#define BGTEXSCENE_H

#include <scene.h>

class BgTexScene : public Scene {
public:
    BgTexScene();

    virtual int initEgl() override;
    void updateBgTexture(VideoBufferPtr b);
    virtual void draw(const  glm::mat4 &viewMat,const  glm::mat4 &projMat) override;

private:
    int _texVShader;
    int _texPShader;
    int _texShaderPrg;
    int _texmvpMatU;
    int _textureU;

    uint _textureId;
};

#endif // BGTEXSCENE_H
