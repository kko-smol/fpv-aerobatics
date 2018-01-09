#ifndef OBJSCENE_H
#define OBJSCENE_H

#include <scene.h>

class ObjScene:public Scene
{
public:
    ObjScene(std::string file);

    // Scene interface
public:
    virtual void draw(const glm::mat4 &viewMat,const glm::mat4 &projMat, const glm::mat4 modelMat) override;

    glm::vec4 color() const;
    void setColor(const glm::vec4 &color);

private:

    virtual int initEgl() override;
    int _vecVShader;
    int _vecPShader;
    int _vecShaderPrg;
    int _vecmvpMatU;

    int _vecColorU;
    glm::vec4 _color;

    uint _vao;
    uint _vertBuf;
    uint _colBuf;

    int _vecAttrLoc;
    int _colAttrLoc;

    std::vector<glm::vec3> _verts;
    std::vector<glm::vec3> _uvs;
};

#endif // TESTBOXSCENE_H
