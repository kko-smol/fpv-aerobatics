#ifndef BGTEXSCENE_H
#define BGTEXSCENE_H

#include <scene.h>

class BgTexScene : public Scene {
public:
    BgTexScene();

    void updateBgTexture(VideoBufferPtr b);
    virtual void draw(const  glm::mat4 &viewMat,const  glm::mat4 &projMat, const glm::mat4 model) override;

private:

    virtual int initEgl() override;
    int _texVShader;
    int _texPShader;
    int _texShaderPrg;
    int _texmvpMatU;
    int _textureU;

    uint _vao;
    uint _vertBuf;
    uint _textBuf;

    int _vecAttrLoc;
    int _texAttrLoc;

    uint _textureId;
};

#endif // BGTEXSCENE_H
