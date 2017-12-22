#include "testboxscene.h"
#include <shaders.h>

#include <iostream>
extern "C" {

#ifdef __arm__
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif
}

TestBoxScene::TestBoxScene():_angle1(0),_angle2(0),_angle3(0)
{

}

int TestBoxScene::initEgl()
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



void TestBoxScene::draw(const glm::mat4 &viewMat, const glm::mat4 &projMat, const glm::mat4 modelMat)
{


#define P000 -0.5,-0.5,-0.5
#define P100  0.5,-0.5,-0.5
#define P010 -0.5, 0.5,-0.5
#define P110  0.5, 0.5,-0.5
#define P001 -0.5,-0.5, 0.5
#define P101  0.5,-0.5, 0.5
#define P011 -0.5, 0.5, 0.5
#define P111  0.5, 0.5, 0.5



    static float box[] = {
        P000,P100,P110,
        P110,P010,P000,
        P001,P101,P111,
        P111,P011,P001,

        P000,P100,P101,
        P000,P101,P001,
        P010,P110,P111,
        P010,P111,P011,

        P000,P010,P011,
        P000,P011,P001,
        P100,P110,P111,
        P100,P111,P101,
    };

    static float colors[] = {
        1.0,1.0,0.0,
        1.0,1.0,0.0,
        1.0,1.0,0.0,
        1.0,1.0,0.0,
        1.0,1.0,0.0,
        1.0,1.0,0.0,

        1.0,1.0,1.0,
        1.0,1.0,1.0,
        1.0,1.0,1.0,
        1.0,1.0,1.0,
        1.0,1.0,1.0,
        1.0,1.0,1.0,

        0.0,1.0,0.0,
        0.0,1.0,0.0,
        0.0,1.0,0.0,
        0.0,1.0,0.0,
        0.0,1.0,0.0,
        0.0,1.0,0.0,

        0.0,0.0,1.0,
        0.0,0.0,1.0,
        0.0,0.0,1.0,
        0.0,0.0,1.0,
        0.0,0.0,1.0,
        0.0,0.0,1.0,

        1.0,0.0,0.0,
        1.0,0.0,0.0,
        1.0,0.0,0.0,
        1.0,0.0,0.0,
        1.0,0.0,0.0,
        1.0,0.0,0.0,

        0.0,1.0,1.0,
        0.0,1.0,1.0,
        0.0,1.0,1.0,
        0.0,1.0,1.0,
        0.0,1.0,1.0,
        0.0,1.0,1.0
    };

    glCheckError();
    glUseProgram(_vecShaderPrg);
    glCheckError();

    glVertexAttribPointer(_vecAttrLoc, 3, GL_FLOAT, GL_FALSE, 0, box);
    glVertexAttribPointer(_colAttrLoc, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(_vecAttrLoc);
    glEnableVertexAttribArray(_colAttrLoc);

    auto v = viewMat;
    auto p = projMat;
    auto m = glm::scale(glm::mat4(1.0),glm::vec3(0.5,0.5,0.5));

    auto mvp = p*v*m;

    //printf("\n");
    //for (int y=0;y<4;++y){
    //    for (int x=0;x<4;++x){
    //        printf(" %f ",glm::value_ptr(mvp)[y*4+x]);
    //    }
    //    printf("\n");
    //}
    //printf("\n");

    glUniformMatrix4fv(_vecmvpMatU, 1, GL_FALSE, glm::value_ptr(mvp));

    glDrawArrays(GL_TRIANGLES, 0, 3*4*3);
    _angle1 +=0.05;
    _angle2 +=0.1;
    _angle3 +=1;

}
