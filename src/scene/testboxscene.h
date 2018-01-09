#ifndef TESTBOXSCENE_H
#define TESTBOXSCENE_H

#include <scene.h>

class TestBoxScene:public Scene
{
public:
    TestBoxScene();

    // Scene interface
public:
    virtual void draw(const glm::mat4 &viewMat,const glm::mat4 &projMat, const glm::mat4 modelMat) override;

private:
    virtual int initEgl() override;

    int _vecVShader;
    int _vecPShader;
    int _vecShaderPrg;
    int _vecmvpMatU;

    int _vecAttrLoc;
    int _colAttrLoc;

    float _angle1;
    float _angle2;
    float _angle3;
};

#endif // TESTBOXSCENE_H
