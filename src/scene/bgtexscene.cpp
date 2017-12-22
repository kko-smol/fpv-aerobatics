#include "bgtexscene.h"
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

BgTexScene::BgTexScene()
{

}

int BgTexScene::initEgl()
{
    {
        _texVShader = glCreateShader(GL_VERTEX_SHADER);
        glCheckError();
        if (!_texVShader) {
            std::cout << "Create texVShader fail\n";
            return 0;
        }

        {
            auto texVShader = Shaders().getTexVertexShader();
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
            auto texPShader = Shaders().getTexFragmentShader();
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

#define RATIO_K (640.0/480.0)

        static const GLfloat vVertices[] = {
            0.5f*RATIO_K,  -0.5f,  -1.0f, 1.0f,
            0.5f*RATIO_K,   0.5f,  -1.0f, 1.0f,
            -0.5f*RATIO_K,  -0.5f, -1.0f, 1.0f,
            -0.5f*RATIO_K,   0.5f, -1.0f, 1.0f
        };

        static const GLfloat vUV[] = {
            0.0,0.0,
            0.0,1.0,
            1.0,0.0,
            1.0,1.0
        };

        glGenBuffers(1,&_vertBuf);
        glBindBuffer(GL_ARRAY_BUFFER,_vertBuf);
        glBufferData(GL_ARRAY_BUFFER,4*4*sizeof(float),vVertices,GL_STATIC_DRAW);

        glGenBuffers(1,&_textBuf);
        glBindBuffer(GL_ARRAY_BUFFER,_textBuf);
        glBufferData(GL_ARRAY_BUFFER,4*2*sizeof(float),vUV,GL_STATIC_DRAW);

        GLuint vao;
        glGenVertexArrays(1,&vao);

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
    }

    return 1;
}

void BgTexScene::updateBgTexture(VideoBufferPtr b)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, b->getWidth()/2, b->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, b->getDataPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void BgTexScene::draw(const glm::mat4 &viewMat, const glm::mat4 &projMat, const glm::mat4 model)
{
    glCheckError();
    glUseProgram(_texShaderPrg);
    glCheckError();

    //glVertexAttribPointer(_vecAttrLoc, 4, GL_FLOAT, GL_FALSE, 0, vVertices);
    glBindBuffer(GL_ARRAY_BUFFER,_vertBuf);
    glCheckError();
    glEnableVertexAttribArray(_vecAttrLoc);
    glCheckError();
    glVertexAttribPointer(_vecAttrLoc, 4, GL_FLOAT, false, 0, 0);
    glCheckError();

    //glVertexAttribPointer(_texAttrLoc, 2, GL_FLOAT, GL_FALSE, 0, vUV);
    glBindBuffer(GL_ARRAY_BUFFER,_textBuf);
    glCheckError();
    glEnableVertexAttribArray(_texAttrLoc);
    glCheckError();
    glVertexAttribPointer(_texAttrLoc, 2, GL_FLOAT, false, 0, 0);
    glCheckError();
    glUniform1i(_textureU, 0);
    glCheckError();
    auto v = glm::mat4(1.0f);
    float w =720.0/2;
    float h= 576.0;
    auto p = glm::mat4(1.0f);

    auto m = glm::scale(glm::mat4(1.0f),glm::vec3(2.0f*h/w,2.0f,1.0f));

    auto mvp = p*v*m;

    glUniformMatrix4fv(_texmvpMatU, 1, GL_FALSE, glm::value_ptr(mvp));
    glCheckError();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glCheckError();

    glClearColor(0,0,-1000,-1000);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER,0);

}
