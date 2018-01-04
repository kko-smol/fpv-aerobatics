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

ObjScene::ObjScene(std::string path):_color(0.0,0.0,0.0,0.0)
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

        _vecColorU = glGetUniformLocation(_vecShaderPrg, "overrideColor");
        glCheckError();
        if (_vecColorU==-1) {
            std::cout << "Find _vecColorU fail!\n";
            return 0;
        }


        glGenBuffers(1,&_vertBuf);
        glBindBuffer(GL_ARRAY_BUFFER,_vertBuf);
        glBufferData(GL_ARRAY_BUFFER,3*sizeof(float)*_verts.size(),_verts.data(),GL_STATIC_DRAW);

        glGenBuffers(1,&_colBuf);
        glBindBuffer(GL_ARRAY_BUFFER,_colBuf);
        glBufferData(GL_ARRAY_BUFFER,3*sizeof(float)*_uvs.size(),_uvs.data(),GL_STATIC_DRAW);

#ifndef __arm__
        glGenVertexArrays(1,&_vao);
#endif

        std::cout << "Vec shader done\n";

    }
    return 1;
}



void ObjScene::draw(const glm::mat4 &viewMat, const glm::mat4 &projMat, const glm::mat4 modelMat)
{

    glCheckError();
    glUseProgram(_vecShaderPrg);
    glCheckError();

#ifndef __arm__
    glBindVertexArray(_vao);
#endif

    glBindBuffer(GL_ARRAY_BUFFER,_vertBuf);
    glCheckError();
    glEnableVertexAttribArray(_vecAttrLoc);
    glCheckError();
    glVertexAttribPointer(_vecAttrLoc, 3, GL_FLOAT, false, 0, 0);
    glCheckError();

    glBindBuffer(GL_ARRAY_BUFFER,_colBuf);
    glCheckError();
    glEnableVertexAttribArray(_colAttrLoc);
    glCheckError();
    glVertexAttribPointer(_colAttrLoc, 3, GL_FLOAT, false, 0, 0);
    glCheckError();

    auto v = viewMat;
    auto p = projMat;
    auto m = modelMat;

    auto mvp = p*v*m;
    /*
    printf("MAT:\n");
    for (int y=0;y<4;++y){
        for (int x=0;x<4;++x){
            printf(" %f ",glm::value_ptr(mvp)[y*4+x]);
        }
        printf("\n");
    }
    printf("\n");*/

    auto tv1 = mvp*glm::vec4(_verts.at(0),1.0f);
    auto tv2 = mvp*glm::vec4(1.0,1.0,1.0,1.0);
//    printf(" (%f;%f;%f), (%f;%f;%f) \n",tv1.x/tv1.w,tv1.y/tv1.w,tv1.z/tv1.w,
//           tv2.x/tv2.w,tv2.y/tv2.w,tv2.z/tv2.w);

    glUniformMatrix4fv(_vecmvpMatU, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform4fv(_vecColorU, 1, glm::value_ptr(_color));

    glDrawArrays(GL_TRIANGLES, 0, _verts.size());

}

glm::vec4 ObjScene::color() const
{
    return _color;
}

void ObjScene::setColor(const glm::vec4 &color)
{
    _color = color;
}
