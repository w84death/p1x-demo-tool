/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 *
 * P1X DEMO TOOL V3
 * by w84death^P1X
 *
 * https://github.com/w84death/p1x-demo-tool
 * (c) 2023.05
 *
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

// #include "synthesizer.h"

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */

typedef GLuint (*PFNGLCREATESHADERPROC)(GLenum type);
typedef void (*PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef GLuint (*PFNGLCREATEPROGRAMPROC)(void);
typedef void (*PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (*PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (*PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (*PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (*PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (*PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef GLint (*PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const GLchar* name);
typedef void (*PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
typedef void (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (*PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef GLint (*PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (*PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
typedef void (*PFNGLUNIFORM2FVPROC) (GLint location, GLsizei count, const GLfloat *value);

PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLUNIFORM2FVPROC glUniform2fv = NULL;

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */

const char* vertexShaderSource =
"attribute vec2 position;"
"varying vec2 TexCoords;"
"uniform vec2 u_resolution;"
"void main() {"
"  gl_Position = vec4(position, 0.0, 1.0);"
"  float aspectRatio = u_resolution.x / u_resolution.y;"
"  TexCoords = vec2(position.x * aspectRatio, position.y);"
"}";

const char* fragmentShaderSource =
#include "shader.glsl"
;


/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */

const double demo_length = 30.0f;

bool demo_running = true;
bool demo_paused = false;
bool window_fullscreen = false;
float resolution_scale = 1.0f;
bool debug_show_stats = false;
int window_width = 1280/2, window_height = 720/2;

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
int main(int argc, char* argv[]) {

  std::cout << "P1X DEMO TOOL V3 by w84death^P1X // (c) 2023.05\n"<< std::endl;
  std::cout << "> Initializing engine with:\n  - window resolution " << window_width << "x" << window_height << "\n  - internal rendering resolution " << window_width*resolution_scale << "x" << window_height*resolution_scale << " (scale " << resolution_scale << ")."<< std::endl;
  std::cout << "DEVELOPMENT VERSION. BUGS ARE EXPECTED. CTRL+C TO KILL THE DEMO.\n"<< std::endl;
/*
 *
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */

  Display *display = XOpenDisplay(NULL);
  int screen = DefaultScreen(display);
  Window rootWindow = RootWindow(display, screen);
  int visualAttr[] = {GLX_RGBA,GLX_DEPTH_SIZE, 24,GLX_DOUBLEBUFFER,None};
  XVisualInfo* visualInfo = glXChooseVisual(display, screen, visualAttr);
  Colormap colormap = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);
  XSetWindowAttributes window_attribs;
  window_attribs.colormap = colormap;
  window_attribs.event_mask = ExposureMask | KeyPressMask;
  Window window = XCreateWindow(display, rootWindow, 0, 0, window_width, window_height, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWColormap | CWEventMask, &window_attribs);
  XMapWindow(display, window);
  XFlush(display);

  GLXContext glxContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
  glXMakeCurrent(display, window, glxContext);

  /*
   * ---10--------20--------30--------40--------50--------60--------70-------80
   */
  glCreateShader = (PFNGLCREATESHADERPROC)glXGetProcAddress((const GLubyte*)"glCreateShader");
  glShaderSource = (PFNGLSHADERSOURCEPROC)glXGetProcAddress((const GLubyte*)"glShaderSource");
  glCompileShader = (PFNGLCOMPILESHADERPROC)glXGetProcAddress((const GLubyte*)"glCompileShader");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC)glXGetProcAddress((const GLubyte*)"glCreateProgram");
  glAttachShader = (PFNGLATTACHSHADERPROC)glXGetProcAddress((const GLubyte*)"glAttachShader");
  glLinkProgram = (PFNGLLINKPROGRAMPROC)glXGetProcAddress((const GLubyte*)"glLinkProgram");
  glUseProgram = (PFNGLUSEPROGRAMPROC)glXGetProcAddress((const GLubyte*)"glUseProgram");
  glGenBuffers = (PFNGLGENBUFFERSPROC)glXGetProcAddress((const GLubyte*)"glGenBuffers");
  glBindBuffer = (PFNGLBINDBUFFERPROC)glXGetProcAddress((const GLubyte*)"glBindBuffer");
  glBufferData = (PFNGLBUFFERDATAPROC)glXGetProcAddress((const GLubyte*)"glBufferData");
  glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)glXGetProcAddress((const GLubyte*)"glGetAttribLocation");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glXGetProcAddress((const GLubyte*)"glVertexAttribPointer");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress((const GLubyte*)"glEnableVertexAttribArray");
  glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress((const GLubyte*)"glDisableVertexAttribArray");
  glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glXGetProcAddress((const GLubyte*)"glGetUniformLocation");
  glUniform1f = (PFNGLUNIFORM1FPROC)glXGetProcAddress((const GLubyte*)"glUniform1f");
  glUniform2fv = (PFNGLUNIFORM2FVPROC)glXGetProcAddress((const GLubyte*)"glUniform2fv");

  float quadVertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

  /*
   * ---10--------20--------30--------40--------50--------60--------70-------80
   */
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

/*
 * ---10--------20--------30--------40--------50--------60--------70-------80
 */

  float demo_time;
  XEvent event;
  float resolution[2] = {static_cast<float>(window_width), static_cast<float>(window_height)};
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now();
  std::chrono::time_point<std::chrono::high_resolution_clock> previous_time = start_time;

  // synthStart();

  while (demo_running) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    GLint timeUniformLocation = glGetUniformLocation(shaderProgram, "u_time");
    glUniform1f(timeUniformLocation, demo_time);

    GLint resolutionUniformLocation = glGetUniformLocation(shaderProgram, "u_resolution");
    glUniform2fv(resolutionUniformLocation, 1, resolution);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    GLint positionAttribLocation = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttribLocation);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(positionAttribLocation);

    glXSwapBuffers(display, window);

    std::chrono::time_point<std::chrono::high_resolution_clock> current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = current_time - previous_time;
    double deltaTime = elapsed_seconds.count();
    previous_time = current_time;
    double frameMs = deltaTime*1000;

    if(!demo_paused) demo_time += deltaTime;
    if(demo_time > demo_length) demo_running = false;

    if(debug_show_stats){
      printf("%.0fx%.0f // %0.00f ms\nDemo Time %000.0fs\n", window_width*resolution_scale, window_height*resolution_scale, frameMs, demo_time);
      debug_show_stats = false;
     }

    while (XPending(display)) {
      XNextEvent(display, &event);
      if (event.type == KeyPress){
        KeySym key = XLookupKeysym(&event.xkey, 0);
        if (key == XK_space) {
          // pause_playback.store(!pause_playback.load());
          demo_paused = !demo_paused;
         }else
        if (key == XK_Escape){
          demo_running = false;
          // quit_playback = true;
         }
       }
     }
   }

  /*
   * ---10--------20--------30--------40--------50--------60--------70-------80
   */
  // synthQuit();
  glXDestroyContext(display, glxContext);
  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return 0;
 }

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
