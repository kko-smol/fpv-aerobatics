#include "objscene.h"
#include <shaders.h>
#include "objloader.hpp"
#include <iostream>
extern "C" {
#ifdef __arm__
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/glew.h>
#endif
}

ObjScene::ObjScene(std::string path)
{

    bool res = loadOBJ(path.c_str(),_verts,_uvs);
    std::cerr << "OBJ LOAD:" << res << " : " << _verts.size() << " " <<
                 _uvs.size() <<  std::endl;

}

int ObjScene::initEgl()
{

    {
        _vecVShader = glCreateShader(GL_VERTEX_SHADER);
        glCheckError();
        if (!_vecVShader) {
            std::cout << "Create vecVShader fail!\n";
            return 0;
        }

        {
            auto vecVShader = Shaders().getVecVertexShader();
            const char* strSrc = vecVShader.c_str();
            const int size = vecVShader.length();
            glShaderSource(_vecVShader, 1, &strSrc, &size);
            glCompileShader(_vecVShader);
        }

        GLint ret;
        glGetShaderiv(_vecVShader, GL_COMPILE_STATUS, &ret);
        glCheckError();
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: vertex shader compilation failed!\n");
            glGetShaderiv(_vecVShader, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetShaderInfoLog(_vecVShader, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return 0;
        }

        _vecPShader = glCreateShader(GL_FRAGMENT_SHADER);
        glCheckError();
        if (!_vecPShader) {
            std::cout << "Create vecPShader fail!\n";
            return 0;
        }

        {
            auto texPShader = Shaders().getVecFragmentShader();
            const char* strSrc = texPShader.c_str();
            const int size = texPShader.length();
            glShaderSource(_vecPShader, 1, &strSrc, &size);
            glCompileShader(_vecPShader);
        }

        glGetShaderiv(_vecPShader, GL_COMPILE_STATUS, &ret);
        glCheckError();
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: fragment shader compilation failed!\n");
            glGetShaderiv(_vecPShader, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetShaderInfoLog(_vecPShader, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return 0;
        }

        _vecShaderPrg = glCreateProgram();
        glCheckError();
        if (!_vecShaderPrg) {
            std::cout << "Create vecShader program fail!\n";
            return 0;
        }

        glAttachShader(_vecShaderPrg, _vecVShader);
        glCheckError();
        glAttachShader(_vecShaderPrg, _vecPShader);
        glCheckError();
        glLinkProgram(_vecShaderPrg);
        glCheckError();
        glGetProgramiv(_vecShaderPrg, GL_LINK_STATUS, &ret);
        glCheckError();
        if (!ret) {
            char *log;

            fprintf(stderr, "Error: program linking failed!\n");
            glGetProgramiv(_vecShaderPrg, GL_INFO_LOG_LENGTH, &ret);

            if (ret > 1) {
                log = (char*)malloc(ret);
                glGetProgramInfoLog(_vecShaderPrg, ret, NULL, log);
                fprintf(stderr, "%s", log);
            }
            return 0;
        }

        _vecAttrLoc = glGetAttribLocation(_vecShaderPrg,"aPosition");
        glCheckError();
        _colAttrLoc = glGetAttribLocation(_vecShaderPrg,"aColor");
        glCheckError();

        _vecmvpMatU = glGetUniformLocation(_vecShaderPrg, "mvpMat");
        glCheckError();
        if (_vecmvpMatU==-1) {
            std::cout << "Find _vecModelMatU fail!\n";
            return 0;
        }
        std::cout << "Vec shader done\n";

    }
    return 1;
}



void ObjScene::draw(const glm::mat4 &viewMat, const glm::mat4 &projMat)
{

    glCheckError();
    glUseProgram(_vecShaderPrg);
    glCheckError();


    glVertexAttribPointer(_vecAttrLoc, 3, GL_FLOAT, GL_FALSE, 0, _verts.data());
    glVertexAttribPointer(_colAttrLoc, 3, GL_FLOAT, GL_FALSE, 0, _uvs.data());

    glEnableVertexAttribArray(_vecAttrLoc);
    glEnableVertexAttribArray(_colAttrLoc);

    auto v = viewMat;
    auto p = projMat;
    auto m = glm::mat4(1.0f);

    auto mvp = p*v*m;

    glUniformMatrix4fv(_vecmvpMatU, 1, GL_FALSE, glm::value_ptr(mvp));

    glDrawArrays(GL_TRIANGLES, 0, _verts.size());
    glFinish();
}
