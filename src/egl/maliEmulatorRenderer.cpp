#include "eglrenderer.h"
#include <iostream>

extern "C" {
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>
#include "esUtil.h"
}


#include "Text.h"
#include "Shader.h"
#include "Timer.h"
#include "Platform.h"
#include "EGLRuntime.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>

class EglPrivate{
public:


};

EglRenderer::EglRenderer(ScenePtr scene):Renderer(scene)
{
    _egl  = new EglPrivate();
}

int EglRenderer::init()
{
    MaliSDK::Platform *platform = MaliSDK::Platform::getInstance();

    /* Initialize windowing system. */
    platform->createWindow(720, 576);

    MaliSDK::EGLRuntime::initializeEGL(MaliSDK::EGLRuntime::OPENGLES2);

    if (!eglMakeCurrent(MaliSDK::EGLRuntime::display, MaliSDK::EGLRuntime::surface, MaliSDK::EGLRuntime::surface, MaliSDK::EGLRuntime::context)) {
        return 0;
    }
    if (!_scene->initEgl()){
        return 0;
    }

    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glFrontFace(GL_CCW);
    //glCullFace(GL_BACK);

    return 1;
}


void EglRenderer::render()
{
    eglMakeCurrent(MaliSDK::EGLRuntime::display, MaliSDK::EGLRuntime::surface, MaliSDK::EGLRuntime::surface, MaliSDK::EGLRuntime::context);

    glViewport(0, 0, scr_w, scr_h);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, scr_w/2, scr_h);
    _scene->draw(_view,_proj);

    glViewport(scr_w/2, 0, scr_w/2, scr_h);
    _scene->draw(_view,_proj);

    glFinish();

    if (!eglSwapBuffers(MaliSDK::EGLRuntime::display, MaliSDK::EGLRuntime::surface)){
        std::cout << "SWap error: " << eglGetError() << std::endl;
    }
}

void EglRenderer::exec()
{

}
