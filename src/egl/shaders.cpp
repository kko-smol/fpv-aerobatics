#include "shaders.h"

Shaders::Shaders()
{

}

std::string Shaders::getTexVertexShader()
{
    return
           "attribute vec4 aPosition;    \n"
           "attribute vec2 aUV;          \n"
           "                             \n"
           "uniform mat4 mvpMat;        \n"

           "varying vec2 UV;             \n"
           "                             \n"
           "void main()                  \n"
           "{                            \n"
           "    vec4 np = mvpMat*aPosition;\n"
           "    UV = aUV;              \n"
           "    gl_Position = np; \n"
           "}                            \n";
}

std::string Shaders::getTexFragmentShader()
{
    return
        #ifdef __arm__
            "precision mediump float;     \n"
        #endif
            "varying vec2 UV;              \n"
            "uniform sampler2D tex1;\n"
            "                             \n"
            "void main()                  \n"
            "{                            \n"
            "    vec4 px = texture2D(tex1,UV);      \n"
            "    float y=0.5*(px.r+px.b); \n"
            "	 float u=(px.g);           \n"
            "	 float v=(px.a);           \n"

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
    return
           "attribute vec3 aPosition;    \n"
           "attribute vec3 aColor;       \n"
           "                             \n"
           "uniform mat4 mvpMat;        \n"

           "varying vec4 color;             \n"
           "                             \n"
           "void main()                  \n"
           "{                            \n"
           "    vec4 pp = vec4(aPosition,1.0);     \n"
           "    color.rgb = aColor;       \n"
           "    color.a = 1.0;            \n"
           "    gl_Position = mvpMat*pp; \n"
           "}                             \n";
}

std::string Shaders::getVecFragmentShader()
{
    return
        #ifdef __arm__
            "precision mediump float;     \n"
        #endif
            "varying vec4 color;              \n"
            "                             \n"
            "void main()                  \n"
            "{                            \n"
            "    gl_FragColor.rgb = color.rgb;  \n"
            "    gl_FragColor.a = 1.0;  \n"
            "}                            \n";
}
