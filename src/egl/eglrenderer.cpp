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

EglRenderer::EglRenderer(ScenePtr scene):_scene(scene)
{
    _egl  = new EglPrivate();
}

int EglRenderer::init()
{
    int stage = 0;
    std::cout << stage << std::endl;stage++;

    _egl->_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    std::cout << "DISP" << _egl->_display;
    if (_egl->_display == EGL_NO_DISPLAY) {
        return 0;
    }
    std::cout << stage << std::endl;stage++;
    EGLint egl_major, egl_minor;
    if (!eglInitialize(_egl->_display, &egl_major, &egl_minor)) {
        return 0;
    }
    std::cout << stage << std::endl;stage++;
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
    std::cout << stage << std::endl;stage++;
    static const EGLint context_attribute_list[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    std::cout << stage << std::endl;stage++;
    _egl->_context = eglCreateContext(_egl->_display, _egl->_config, EGL_NO_CONTEXT, context_attribute_list);
    if (_egl->_context == EGL_NO_CONTEXT) {
        fprintf(stderr, "Error: eglCreateContext failed: 0x%08X\n",
            eglGetError());
        return 0;
    }
    std::cout << stage << std::endl;stage++;
    static EGLint window_attribute_list[] = {
        EGL_NONE
    };
    struct mali_native_window native_window = {
        .width = 720,
        .height = 576,
    };
    _egl->_surface = eglCreateWindowSurface(_egl->_display, _egl->_config, &native_window, window_attribute_list);

    std::cout << stage << std::endl;stage++;
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

    std::cout << stage << std::endl;stage++;
    _scene->initEgl();
    std::cout << stage << std::endl;stage++;
    return 1;
}

void EglRenderer::setEyeMats(const ESMatrix &l, const ESMatrix &r)
{
    _lEye = l;
    _rEye = r;
}

const ESMatrix &EglRenderer::leftEye() const
{
    return _lEye;
}

const ESMatrix &EglRenderer::rightEye() const
{
    return _rEye;
}

void EglRenderer::render()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, scr_w/2, scr_h);
    _scene->draw(_lEye);

    glViewport(scr_w/2, 0, scr_w/2, scr_h);
    _scene->draw(_rEye);

    eglSwapBuffers(_egl->_display, _egl->_surface);
}
