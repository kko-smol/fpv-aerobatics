#include "glutrendefer.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <iostream>

GlutRenderer::GlutRenderer(ScenePtr scene):Renderer(scene)
{

}

void GLUT_DEBUGPROCARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
    GlutRenderer* ptr=(GlutRenderer*)userParam;
    std::cerr << source << ":" << id << ":" <<  message;
}

int GlutRenderer::init()
{
    int n = 0;
    glewExperimental = GL_TRUE;

    _scene->glCheckError();
    if (!glfwInit()){
        return 0;
    }
    _scene->glCheckError();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    _scene->glCheckError();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    _scene->glCheckError();
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    _scene->glCheckError();

    scr_w = 720;
    scr_h = 576;
    _window = glfwCreateWindow(scr_w,scr_h,"Test",NULL,NULL);
    glfwMakeContextCurrent( (GLFWwindow*)_window );
    _scene->glCheckError();

    GLenum err = glewInit();
    _scene->glCheckError();
    if (err){
        printf("glewInit err %u\n",err);
    }

    glEnable( GL_DEPTH_TEST ); /* enable depth-testing */
    _scene->glCheckError();
    glDepthFunc( GL_LESS );
    _scene->glCheckError();

    auto renderer = glGetString( GL_RENDERER ); /* get renderer string */
    auto version = glGetString( GL_VERSION );	 /* version as a string */
    printf( "Renderer: %s\n", renderer );
    printf( "OpenGL version supported %s\n", version );

    _scene->glCheckError();
    return _scene->initEgl();;
}

void GlutRenderer::render()
{
    glfwMakeContextCurrent((GLFWwindow*) _window );

    float aspect = 1/((0.5*scr_w)/scr_h);

    glm::mat4 gProjection = glm::perspective(glm::pi<float>() * 0.3f, 1.0f/aspect, 1.0f, 10.f);
    glm::mat4 gView = _view;

    glViewport(0, 0, scr_w, scr_h);
    _scene->glCheckError();
    static float a = 0.0;
    glClearColor(a, a, a, 1.0);
    _scene->glCheckError();
    a=a+0.01;
    if (a>=1.0){
        a =0.0;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    _scene->glCheckError();
    glViewport(0, 0, scr_w/2, scr_h);
    _scene->glCheckError();
    _scene->draw(gView,gProjection);
    _scene->glCheckError();
    glViewport(scr_w/2, 0, scr_w/2, scr_h);
    _scene->glCheckError();
    _scene->draw(gView,gProjection);
    _scene->glCheckError();
    glfwPollEvents();
    _scene->glCheckError();
    glfwSwapBuffers( (GLFWwindow*)_window );
    _scene->glCheckError();
}

void GlutRenderer::exec()
{
    glfwPollEvents();
}
