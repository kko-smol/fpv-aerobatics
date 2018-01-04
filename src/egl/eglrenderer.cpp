#include "eglrenderer.h"
#include <iostream>

extern "C" {
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>
}

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>

class EglPrivate{
public:
    EGLDisplay _display;
    EGLSurface _surface;
    EGLConfig _config;
    EGLContext _context;

    //shader IDs
    GLint _modelMatrix;
    GLint _viewMatrix;
};

EglRenderer::EglRenderer(ScenePtr scene):Renderer(scene)
{
    _egl  = new EglPrivate();
}

int EglRenderer::init()
{
    _egl->_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (_egl->_display == EGL_NO_DISPLAY) {
        return 0;
    }
    EGLint egl_major, egl_minor;
    if (!eglInitialize(_egl->_display, &egl_major, &egl_minor)) {
        return 0;
    }
    static EGLint const config_attribute_list[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_BUFFER_SIZE, 32,

        EGL_STENCIL_SIZE, 0,
        EGL_DEPTH_SIZE, 0,

        EGL_SAMPLES, 4,

        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,

        EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PIXMAP_BIT,

        EGL_NONE
    };
    EGLint num_config;
    eglChooseConfig(_egl->_display, config_attribute_list, &_egl->_config, 1,	&num_config);

    static const EGLint context_attribute_list[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    _egl->_context = eglCreateContext(_egl->_display, _egl->_config, EGL_NO_CONTEXT, context_attribute_list);
    if (_egl->_context == EGL_NO_CONTEXT) {
        fprintf(stderr, "Error: eglCreateContext failed: 0x%08X\n", eglGetError());
        return 0;
    }

    static EGLint window_attribute_list[] = {
        EGL_NONE
    };
    static struct mali_native_window native_window = {
        .width = 720,
                .height = 576,
    };
    _egl->_surface = eglCreateWindowSurface(_egl->_display, _egl->_config, &native_window, window_attribute_list);
    if (_egl->_surface == EGL_NO_SURFACE) {
        return 0;
    }

    if (!eglQuerySurface(_egl->_display, _egl->_surface, EGL_WIDTH, &scr_w) ||
            !eglQuerySurface(_egl->_display, _egl->_surface, EGL_HEIGHT, &scr_h)) {
        return 0;
    }
    std::cout << "Surface size: " <<  scr_w << "x" << scr_h << std::endl;

    if (!eglMakeCurrent(_egl->_display, _egl->_surface, _egl->_surface, _egl->_context)) {
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
    eglMakeCurrent(_egl->_display, _egl->_surface, _egl->_surface, _egl->_context);

    glViewport(0, 0, scr_w, scr_h);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, scr_w/2, scr_h);

    glm::mat4 m(1.0f);

    _scene->draw(_view,_proj,m);

    glViewport(scr_w/2, 0, scr_w/2, scr_h);
    _scene->draw(_view,_proj,m);

    glFinish();
    if (!eglSwapBuffers(_egl->_display, _egl->_surface)){
        std::cout << "SWap error: " << eglGetError() << std::endl;
    }
}

void EglRenderer::exec()
{

}
