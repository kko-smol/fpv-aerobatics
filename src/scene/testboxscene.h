#ifndef TESTBOXSCENE_H
#define TESTBOXSCENE_H

#include <scene.h>

class TestBoxScene:public Scene
{
public:
    TestBoxScene();

    // Scene interface
public:
    virtual int initEgl() override;
    virtual void draw(const glm::mat4 &viewMat,const glm::mat4 &projMat) override;

private:
    int _vecVShader;
    int _vecPShader;
    int _vecShaderPrg;
    int _vecmvpMatU;

    float _angle1;
    float _angle2;
    float _angle3;
};

#endif // TESTBOXSCENE_H
