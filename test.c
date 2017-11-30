#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "esUtil.h"

#include "video_capture.c"

#define WIDTH 720
#define HEIGHT 576

struct mali_native_window native_window = {
	.width = WIDTH,
	.height = HEIGHT,
};

GLint modelMatrix;
GLint vievMatrix;
GLint texU;

GLint textureID;

ESMatrix lViewMatrix;
ESMatrix rViewMatrix;

GLint tex;

static const char *vertex_shader_source =
	"attribute vec4 aPosition;    \n"
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

static const char *fragment_shader_source =
	"#version 100                 \n"
//	"#extension GL_OES_EGL_image_external : require\n"
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


#define RATIO_K (((float)IM_WIDTH)/IM_HEIGHT)

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

EGLDisplay egl_display;
EGLSurface egl_surface;

void drawScene(){

	ESMatrix modelM;
	esMatrixLoadIdentity(&modelM);
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, &modelM.m[0][0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Redraw(int width, int height, float d) {
	glClearColor(0.0, 0.0, 0.0, 1.0);

	esMatrixLoadIdentity(&rViewMatrix);
	esMatrixLoadIdentity(&lViewMatrix);

	esPerspective(&lViewMatrix,60,0.5*width/height,0.01,10);
	esPerspective(&rViewMatrix,60,0.5*width/height,0.01,10);

	rViewMatrix.m[3][0]=d;
	lViewMatrix.m[3][0]=-d;

	glViewport(0, 0, width/2, height);
	glClear(GL_COLOR_BUFFER_BIT);

  glUniformMatrix4fv(vievMatrix,  1, GL_FALSE, &lViewMatrix.m[0][0]);

	drawScene();

	glViewport(width/2, 0, width/2, height);
	glUniformMatrix4fv(vievMatrix,  1, GL_FALSE, &rViewMatrix.m[0][0]);

  drawScene();

	eglSwapBuffers(egl_display, egl_surface);
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

static EGLint window_attribute_list[] = {
	EGL_NONE
};

static const EGLint context_attribute_list[] = {
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
};

int main(int argc, char *argv[]) {

	EGLint egl_major, egl_minor;
	EGLConfig config;
	EGLint num_config;
	EGLContext context;
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;
	GLint ret;
	GLint width, height;

	egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	if (egl_display == EGL_NO_DISPLAY) {
		fprintf(stderr, "Error: No display found!\n");
		return -1;
	}

	if (!eglInitialize(egl_display, &egl_major, &egl_minor)) {
		fprintf(stderr, "Error: eglInitialise failed!\n");
		return -1;
	}

	printf("EGL Version: \"%s\"\n", eglQueryString(egl_display, EGL_VERSION));
	printf("EGL Vendor: \"%s\"\n", eglQueryString(egl_display, EGL_VENDOR));
	printf("EGL Extensions: \"%s\"\n", eglQueryString(egl_display, EGL_EXTENSIONS));

	eglChooseConfig(egl_display, config_attribute_list, &config, 1,	&num_config);

	context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, context_attribute_list);
	if (context == EGL_NO_CONTEXT) {
		fprintf(stderr, "Error: eglCreateContext failed: 0x%08X\n",
			eglGetError());
		return -1;
	}

	egl_surface = eglCreateWindowSurface(egl_display, config, &native_window, window_attribute_list);

	if (egl_surface == EGL_NO_SURFACE) {
		fprintf(stderr, "Error: eglCreateWindowSurface failed: "
			"0x%08X\n", eglGetError());
		return -1;
	}

	if (!eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &width) ||
	    !eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &height)) {
		fprintf(stderr, "Error: eglQuerySurface failed: 0x%08X\n",
			eglGetError());
		return -1;
	}
	printf("Surface size: %dx%d\n", width, height);

	if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, context)) {
		fprintf(stderr, "Error: eglMakeCurrent() failed: 0x%08X\n",
			eglGetError());
		return -1;
	}

	printf("GL Vendor: \"%s\"\n", glGetString(GL_VENDOR));
	printf("GL Renderer: \"%s\"\n", glGetString(GL_RENDERER));
	printf("GL Version: \"%s\"\n", glGetString(GL_VERSION));
	printf("GL Extensions: \"%s\"\n", glGetString(GL_EXTENSIONS));

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	if (!vertex_shader) {
		fprintf(stderr, "Error: glCreateShader(GL_VERTEX_SHADER) "
			"failed: 0x%08X\n", glGetError());
		return -1;
	}

	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char *log;

		fprintf(stderr, "Error: vertex shader compilation failed!\n");
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			glGetShaderInfoLog(vertex_shader, ret, NULL, log);
			fprintf(stderr, "%s", log);
		}
		return -1;
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!fragment_shader) {
		fprintf(stderr, "Error: glCreateShader(GL_FRAGMENT_SHADER) "
			"failed: 0x%08X\n", glGetError());
		return -1;
	}

	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char *log;

		fprintf(stderr, "Error: fragment shader compilation failed!\n");
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			glGetShaderInfoLog(fragment_shader, ret, NULL, log);
			fprintf(stderr, "%s", log);
		}
		return -1;
	}

	program = glCreateProgram();
	if (!program) {
		fprintf(stderr, "Error: failed to create program!\n");
		return -1;
	}

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &ret);
	if (!ret) {
		char *log;

		fprintf(stderr, "Error: program linking failed!\n");
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			glGetProgramInfoLog(program, ret, NULL, log);
			fprintf(stderr, "%s", log);
		}
		return -1;
	}


	vievMatrix = glGetUniformLocation(program, "viewMat");
	if (vievMatrix==-1) {
		fprintf(stderr, "Error: viewMat "
			"failed: 0x%08X\n", glGetError());
		return -1;
	}
	modelMatrix = glGetUniformLocation(program, "modelMat");
	if (modelMatrix==-1) {
		fprintf(stderr, "Error: modelMat "
			"failed: 0x%08X\n", glGetError());
		return -1;
	}
	texU = glGetUniformLocation(program, "tex1");
	if (texU==-1) {
		fprintf(stderr, "Error: tex1 "
			"failed: 0x%08X\n", glGetError());
		return -1;
	}

	glBindAttribLocation(program, 0, "aPosition");
	glBindAttribLocation(program, 1, "aUV");

	glUseProgram(program);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, vVertices);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, vUV);
	glEnableVertexAttribArray(1);

	glGenTextures(1, &textureID);
	if (textureID==-1){
		fprintf(stderr, "Error: textureID "
			"failed: 0x%08X\n", glGetError());
		return -1;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

  int w = 640;
	int h = 480;
  unsigned char* yuyvData = malloc(w*h*2);

	init_video_capture();

	float d = 0.05;
	float dd = -0.001;
	while(1){
		struct timeval t1, t2;
		double elapsedTime;
		gettimeofday(&t1, NULL);

		video_capture(yuyvData);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, w/2, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, yuyvData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glUniform1i(texU, 0);
		Redraw(width, height, d);
		d=d+dd;
		if (d>0.3){
			dd=-0.001;
		}
		if (d<-0.3){
			dd=0.001;
		}

		gettimeofday(&t2, NULL);

		// compute and print the elapsed time in millisec
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
	  printf("Frame time %f \n",elapsedTime);
	}
    free_video_capture();
	return 0;
}
