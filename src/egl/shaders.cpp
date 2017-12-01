#include "shaders.h"

Shaders::Shaders()
{

}

std::string Shaders::getTexVertexShader()
{
    return "attribute vec4 aPosition;    \n"
           "attribute vec2 aUV;          \n"
           "                             \n"
           "uniform mat4 viewMat;        \n"
           "uniform mat4 modelMat;       \n"

           "varying vec2 UV;             \n"
           "                             \n"
           "void main()                  \n"
           "{                            \n"
           "    vec4 np = (viewMat*modelMat)*aPosition;\n"
           "    UV = aUV;              \n"
           "    gl_Position.xyz = np.xyz; \n"
           "    gl_Position.w = 1.0; \n"
           "}                            \n";
}

std::string Shaders::getTexFragmentShader()
{
    return 	"#version 100                 \n"
            "precision mediump float;     \n"
            "varying vec2 UV;              \n"
            "uniform sampler2D tex1;\n"
            "                             \n"
            "void main()                  \n"
            "{                            \n"
            "    vec4 px = texture2D(tex1,UV);      \n"
            "    float y=0.5*(px.r+px.b); \n"
            "		 float u=(px.g);           \n"
            "		 float v=(px.a);           \n"

            "    y=1.1643*(y-0.0625);\n"
            "    u=u-0.5 ;\n"
            "    v=v-0.5 ;\n"
            "    float r=y+1.5958*v;\n"
            "    float g=y-0.39173*u-0.81290*v;\n"
            "    float b=y+2.017*u;\n"

            "    gl_FragColor = vec4(r, g, b, 1.0);  \n"
            "}                            \n";
}

std::string Shaders::getVecVertexShader()
{
    return "attribute vec4 aPosition;    \n"
           "attribute vec4 aColor;       \n"
           "                             \n"
           "uniform mat4 viewMat;        \n"
           "uniform mat4 modelMat;       \n"

           "varying vec4 color;             \n"
           "                             \n"
           "void main()                  \n"
           "{                            \n"
           "    vec4 np = (viewMat*modelMat)*aPosition;\n"
           "    color = aColor;              \n"
           "    gl_Position.xyz = np.xyz; \n"
           "    gl_Position.w = 1.0; \n"
           "}                            \n";
}

std::string Shaders::getVecFragmentShader()
{
    return 	"#version 100                 \n"
            "precision mediump float;     \n"
            "varying vec4 color;              \n"
            "                             \n"
            "void main()                  \n"
            "{                            \n"
            "    gl_FragColor = color;  \n"
            "}                            \n";
}
