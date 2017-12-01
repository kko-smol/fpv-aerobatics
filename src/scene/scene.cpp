#include "scene.h"
#include <shaders.h>

extern "C" {
#include <EGL/egl.h>
#include <GLES2/gl2.h>
}

Scene::Scene()
{

}

int Scene::initEgl()
{
    {
        _texVShader = glCreateShader(GL_VERTEX_SHADER);
        if (!_texVShader) {
            return -1;
        }

        {
            auto texVShader = Shaders().getTexVertexShader();
            const char* strSrc = texVShader.c_str();
            const EGLint size = texVShader.length();
            glShaderSource(_texVShader, 1, &strSrc, &size);
            glCompileShader(_texVShader);
        }

        GLint ret;
        glGetShaderiv(_texVShader, GL_COMPILE_STATUS, &ret);
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: vertex shader compilation failed!\n");
            glGetShaderiv(_texVShader, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetShaderInfoLog(_texVShader, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return -1;
        }

        _texPShader = glCreateShader(GL_FRAGMENT_SHADER);
        if (!_texPShader) {
            return -1;
        }

        {
            auto texPShader = Shaders().getTexFragmentShader();
            const char* strSrc = texPShader.c_str();
            const EGLint size = texPShader.length();
            glShaderSource(_texPShader, 1, &strSrc, &size);
            glCompileShader(_texPShader);
        }

        glGetShaderiv(_texPShader, GL_COMPILE_STATUS, &ret);
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: fragment shader compilation failed!\n");
            glGetShaderiv(_texPShader, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetShaderInfoLog(_texPShader, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return -1;
        }

        _texShaderPrg = glCreateProgram();
        if (!_texShaderPrg) {
            return -1;
        }

        glAttachShader(_texShaderPrg, _texVShader);
        glAttachShader(_texShaderPrg, _texPShader);
        glLinkProgram(_texShaderPrg);
        glGetProgramiv(_texShaderPrg, GL_LINK_STATUS, &ret);
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: program linking failed!\n");
            glGetProgramiv(_texShaderPrg, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetProgramInfoLog(_texShaderPrg, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return -1;
        }
        glBindAttribLocation(_texShaderPrg, 0, "aPosition");
        glBindAttribLocation(_texShaderPrg, 1, "aUV");

        _texViewMatU = glGetUniformLocation(_texShaderPrg, "viewMat");
        if (_texViewMatU==-1) {
            return -1;
        }
        _texModelMatU = glGetUniformLocation(_texShaderPrg, "modelMat");
        if (_texModelMatU==-1) {
            return -1;
        }
        _textureU = glGetUniformLocation(_texShaderPrg, "tex1");
        if (_textureU==-1) {
            return -1;
        }

        glGenTextures(1, &_textureId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _textureId);
    }


    {
        _vecVShader = glCreateShader(GL_VERTEX_SHADER);
        if (!_vecVShader) {
            return -1;
        }

        {
            auto vecVShader = Shaders().getVecVertexShader();
            const char* strSrc = vecVShader.c_str();
            const EGLint size = vecVShader.length();
            glShaderSource(_vecVShader, 1, &strSrc, &size);
            glCompileShader(_vecVShader);
        }

        GLint ret;
        glGetShaderiv(_vecVShader, GL_COMPILE_STATUS, &ret);
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: vertex shader compilation failed!\n");
            glGetShaderiv(_vecVShader, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetShaderInfoLog(_vecVShader, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return -1;
        }

        _vecPShader = glCreateShader(GL_FRAGMENT_SHADER);
        if (!_vecPShader) {
            return -1;
        }

        {
            auto texPShader = Shaders().getVecFragmentShader();
            const char* strSrc = texPShader.c_str();
            const EGLint size = texPShader.length();
            glShaderSource(_vecPShader, 1, &strSrc, &size);
            glCompileShader(_vecPShader);
        }

        glGetShaderiv(_vecPShader, GL_COMPILE_STATUS, &ret);
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: fragment shader compilation failed!\n");
            glGetShaderiv(_vecPShader, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetShaderInfoLog(_vecPShader, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return -1;
        }

        _vecShaderPrg = glCreateProgram();
        if (!_vecShaderPrg) {
            return -1;
        }

        glAttachShader(_vecShaderPrg, _vecVShader);
        glAttachShader(_vecShaderPrg, _vecPShader);
        glLinkProgram(_vecShaderPrg);
        glGetProgramiv(_vecShaderPrg, GL_LINK_STATUS, &ret);
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: program linking failed!\n");
            glGetProgramiv(_vecShaderPrg, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetProgramInfoLog(_vecShaderPrg, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return -1;
        }
        glBindAttribLocation(_vecShaderPrg, 0, "aPosition");
        glBindAttribLocation(_vecShaderPrg, 1, "aColor");

        _vecViewMatU = glGetUniformLocation(_vecShaderPrg, "viewMat");
        if (_vecViewMatU==-1) {
            return -1;
        }
        _vecModelMatU = glGetUniformLocation(_vecShaderPrg, "modelMat");
        if (_vecModelMatU==-1) {
            return -1;
        }
    }

    return 0;
}

void Scene::updateBgTexture(VideoBufferPtr b)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, b->getWidth()/2, b->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, b->getDataPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Scene::draw(const ESMatrix &viewMat)
{
#define RATIO_K (640.0/480.0)
    static GLfloat vVertices[] = {
        0.5f*RATIO_K,  -0.5f, 0.0f, 1.0f,
        0.5f*RATIO_K,   0.5f, 0.0f, 1.0f,
        -0.5f*RATIO_K,  -0.5f, 0.0f, 1.0f,
        -0.5f*RATIO_K,   0.5f, 0.0f, 1.0f
    };

    static GLfloat vUV[] = {
        0.0,0.0,
        0.0,1.0,
        1.0,0.0,
        1.0,1.0
    };

    glUseProgram(_texShaderPrg);
    glUniform1i(_textureU, 0);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, vUV);
    glEnableVertexAttribArray(1);


}
