#include "textscene.h"
#include <shaders.h>

#include <iostream>
extern "C" {
#ifdef __arm__
#include <EGL/egl.h>
//#include <GLES/gl.h>
//#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif
}

#include "cp1251.h"

TextScene::TextScene(int width):Scene(),_maxW(width),_textChanged(true)
{
}

void TextScene::setText(std::string t)
{
//    if (t.size()<_maxW)
    {
        _text = t;
        _textChanged = true;
    }
}

int TextScene::initEgl()
{
    {
        _texVShader = glCreateShader(GL_VERTEX_SHADER);
        glCheckError();
        if (!_texVShader) {
            std::cout << "Create texVShader fail\n";
            return 0;
        }

        {
            auto texVShader = getTexVertexShader();
            const char* strSrc = texVShader.c_str();
            const int size = texVShader.length();
            glShaderSource(_texVShader, 1, &strSrc, &size);
            glCompileShader(_texVShader);
        }

        GLint ret;
        glGetShaderiv(_texVShader, GL_COMPILE_STATUS, &ret);
        glCheckError();
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: vertex shader compilation failed!\n");
            glGetShaderiv(_texVShader, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetShaderInfoLog(_texVShader, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return 0;
        }

        _texPShader = glCreateShader(GL_FRAGMENT_SHADER);
        glCheckError();
        if (!_texPShader) {
            std::cout << "Create texPShader fail\n";
            return 0;
        }

        {
            auto texPShader = getTexFragmentShader();
            const char* strSrc = texPShader.c_str();
            const int size = texPShader.length();
            glShaderSource(_texPShader, 1, &strSrc, &size);
            glCompileShader(_texPShader);
        }

        glGetShaderiv(_texPShader, GL_COMPILE_STATUS, &ret);
        glCheckError();
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: fragment shader compilation failed!\n");
            glGetShaderiv(_texPShader, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetShaderInfoLog(_texPShader, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return 0;
        }

        _texShaderPrg = glCreateProgram();
        glCheckError();
        if (!_texShaderPrg) {
            std::cout << "Create texShader program fail\n";
            return 0;
        }

        glAttachShader(_texShaderPrg, _texVShader);
        glCheckError();
        glAttachShader(_texShaderPrg, _texPShader);
        glCheckError();
        glLinkProgram(_texShaderPrg);
        glCheckError();
        glGetProgramiv(_texShaderPrg, GL_LINK_STATUS, &ret);
        glCheckError();
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: program linking failed!\n");
            glGetProgramiv(_texShaderPrg, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetProgramInfoLog(_texShaderPrg, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return 0;
        }

        _texmvpMatU = glGetUniformLocation(_texShaderPrg, "mvpMat");
        glCheckError();
        if (_texmvpMatU==-1) {
            std::cout << "Get _texViewMatU fail\n";
            return 0;
        }

        _textureU = glGetUniformLocation(_texShaderPrg, "tex1");
        glCheckError();
        if (_textureU==-1) {
            std::cout << "Get _textureU fail\n";
            return 0;
        }

        float dx = 1.0/_maxW;
        float z = 0.99;
        float w = 1.0;

        /*
         * 1   3,4,7   9 ...
         * *----|*|----* ...
         * | \  | | \  | ...
         * |  \ | |  \ | ...
         * *____|*|____* ...
         * 0   2,5,6   8 ...
         *
        */
        for (int i=0;i<=_maxW;++i){
            _vertices.push_back({-0.5f+ (i  )*dx,-0.5f,z,w}); //0 + 6*i
            _vertices.push_back({-0.5f+ (i  )*dx, 0.5f,z,w}); //1 + 6*i
            _vertices.push_back({-0.5f+ (i+1)*dx,-0.5f,z,w}); //2 + 6*i
            _vertices.push_back({-0.5f+ (i+1)*dx, 0.5f,z,w}); //3 + 6*i
            _vertices.push_back({-0.5f+ (i+1)*dx, 0.5f,z,w}); //4 + 6*i
            _vertices.push_back({-0.5f+ (i+1)*dx,-0.5f,z,w}); //5 + 6*i

            _UVs.push_back({0.0f,0.0f});//0 + 6*i
            _UVs.push_back({0.0f,1.0f});//1 + 6*i
            _UVs.push_back({1.0f,0.0f});//2 + 6*i
            _UVs.push_back({1.0f,1.0f});//3 + 6*i
            _UVs.push_back({1.0f,1.0f});//4 + 6*i
            _UVs.push_back({1.0f,0.0f});//5 + 6*i
        }

        glGenBuffers(1,&_vertBuf);
        glBindBuffer(GL_ARRAY_BUFFER,_vertBuf);
        glBufferData(GL_ARRAY_BUFFER,_vertices.size()*sizeof(glm::vec4),_vertices.data(),GL_STATIC_DRAW);

        glGenBuffers(1,&_textBuf);
        glBindBuffer(GL_ARRAY_BUFFER,_textBuf);
        glBufferData(GL_ARRAY_BUFFER,_UVs.size()*2*sizeof(float),_UVs.data(),GL_DYNAMIC_DRAW);


#ifndef __arm__
        glGenVertexArrays(1,&_vao);
#endif

        _vecAttrLoc = glGetAttribLocation(_texShaderPrg,"aPosition");
        glCheckError();
        _texAttrLoc = glGetAttribLocation(_texShaderPrg,"aUV");
        glCheckError();

        glGenTextures(1, &_textureId);
        glCheckError();
        glActiveTexture(GL_TEXTURE0);
        glCheckError();
        glBindTexture(GL_TEXTURE_2D, _textureId);
        glCheckError();
        std::cout << "Tex shader done!\n";

        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, font_tex.width, font_tex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, font_tex.pixel_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    return 1;
}

void TextScene::draw(const glm::mat4 &viewMat, const glm::mat4 &projMat, const glm::mat4 model)
{
    glCheckError();
    glUseProgram(_texShaderPrg);
    glCheckError();

    glActiveTexture(GL_TEXTURE0);
    glCheckError();
    glBindTexture(GL_TEXTURE_2D, _textureId);

#ifndef __arm__
    glBindVertexArray(_vao);
#endif

    if (_textChanged){

        float chW = 1.0/17.0 - 6.0/512.0;
        float chH = 1.0/17.0 - 14.0/512.0;
        float chxs = 1.0/17.0;
        float chys = 1.0/17.0;
        float chx0 = chxs + 3.0/512.0;
        float chy0 = chys + 6.0/512.0;

        for (int i=0;i<=_maxW;++i){
            float chx = 0.0;
            float chy = 0.0;
            if (i<_text.size()){
                uint8_t ch = _text.data()[i];
                int y = (ch&0xF0)>>4;
                int x = (ch&0x0F);
                chx = x*chxs;
                chy = y*chys;
            }

            _UVs[i*6+0]=glm::vec2{chx0+chx+chW*0.0f,(chy0+chy+chH*1.0f)};//0 + 6*i
            _UVs[i*6+1]=glm::vec2{chx0+chx+chW*0.0f,(chy0+chy+chH*0.0f)};//1 + 6*i
            _UVs[i*6+2]=glm::vec2{chx0+chx+chW*1.0f,(chy0+chy+chH*1.0f)};//2 + 6*i
            _UVs[i*6+3]=glm::vec2{chx0+chx+chW*1.0f,(chy0+chy+chH*0.0f)};//3 + 6*i
            _UVs[i*6+4]=glm::vec2{chx0+chx+chW*1.0f,(chy0+chy+chH*0.0f)};//4 + 6*i
            _UVs[i*6+5]=glm::vec2{chx0+chx+chW*1.0f,(chy0+chy+chH*1.0f)};//5 + 6*i
        }

        _textChanged = false;
    }

    glBindBuffer(GL_ARRAY_BUFFER,_vertBuf);
    glCheckError();
    glEnableVertexAttribArray(_vecAttrLoc);
    glCheckError();
    glVertexAttribPointer(_vecAttrLoc, 4, GL_FLOAT, false, 0, 0);
    glCheckError();

    glBindBuffer(GL_ARRAY_BUFFER,_textBuf);
    glBufferData(GL_ARRAY_BUFFER,_UVs.size()*2*sizeof(float),_UVs.data(),GL_DYNAMIC_DRAW);
    glCheckError();
    glEnableVertexAttribArray(_texAttrLoc);
    glCheckError();
    glVertexAttribPointer(_texAttrLoc, 2, GL_FLOAT, false, 0, 0);
    glCheckError();
    glUniform1i(_textureU, 0);
    glCheckError();
    auto mvp = projMat*viewMat*model;

    glUniformMatrix4fv(_texmvpMatU, 1, GL_FALSE, glm::value_ptr(mvp));
    glCheckError();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, _maxW*6);
    glCheckError();

}

std::string TextScene::getTexVertexShader()
{
    return
           "attribute vec4 aPosition;    \n"
           "attribute vec2 aUV;          \n"
           "                             \n"
           "uniform mat4 mvpMat;         \n"
           "varying vec2 UV;             \n"
           "                             \n"
           "void main()                  \n"
           "{                            \n"
           "    vec4 np = mvpMat*aPosition;\n"
           "    UV = aUV;                \n"
           "    gl_Position = np; \n"
           "}                            \n";
}

std::string TextScene::getTexFragmentShader()
{
    return
        #ifdef __arm__
            "precision mediump float;     \n"
        #endif
            "varying vec2 UV;             \n"
            "uniform sampler2D tex1;      \n"
            "                             \n"
            "void main()                  \n"
            "{                            \n"
            "    gl_FragColor = texture2D(tex1,UV);  \n"
            "}                            \n";
}
