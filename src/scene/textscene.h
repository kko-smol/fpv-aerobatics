#ifndef TEXTSCENE_H
#define TEXTSCENE_H

#include "scene.h"

class TextScene : public Scene
{
public:
    TextScene(int width);

    void setText(std::string);

    virtual int initEgl() override;
    virtual void draw(const  glm::mat4 &viewMat,const  glm::mat4 &projMat, const glm::mat4 model) override;

private:
    int _texVShader;
    int _texPShader;
    int _texShaderPrg;
    int _texmvpMatU;
    int _textureU;

    uint _vao;
    uint _vertBuf;
    uint _textBuf;

    std::vector<glm::vec4> _vertices;
    std::vector<glm::vec2> _UVs;

    int _vecAttrLoc;
    int _texAttrLoc;

    uint _textureId;

    int _maxW;
    std::string _text;
    bool _textChanged;
    std::string getTexVertexShader();
    std::string getTexFragmentShader();
};

#endif // TEXTSCENE_H
