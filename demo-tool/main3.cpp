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


const char* vertexShaderSource = "attribute vec2 position;"
"void main() {"
"  gl_Position = vec4(position, 0.0, 1.0);"
"}";
const char* fragmentShaderSource =
"uniform float u_time;"
"void main() {"
"  float r = 0.5 * (1.0 + sin(u_time));"
"  gl_FragColor = vec4(r, 0.0, 1.0 - r, 1.0);"
"}";

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */

char demo_name[32] = "CODENAME: SHADER C1TY";
float demo_time = 0.0f;
const float demo_length = 60.0f;
bool application_running = true;

int window_width = 1280, window_height = 720;

int main() {
  Display *display = XOpenDisplay(NULL);
  int screen = DefaultScreen(display);
  Window rootWindow = RootWindow(display, screen);

  GLint glxAttribs[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 24, GLX_STENCIL_SIZE, 8, None};
  XVisualInfo *visualInfo = glXChooseVisual(display, screen, glxAttribs);

  Window window = XCreateWindow(display, rootWindow, 10, 10, window_width, window_height, 0, visualInfo->depth, InputOutput, visualInfo->visual, 0, NULL);
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

  struct timespec startTime, endTime,  initialTime;
  clock_gettime(CLOCK_MONOTONIC, &initialTime);
  long frameTimeNanoseconds;
  double frameTimeMilliseconds,  elapsedTime;
  XEvent event;
  float resolution[2] = {static_cast<float>(window_width), static_cast<float>(window_height)};

  while (application_running) {
    while (XPending(display)) {
      XNextEvent(display, &event);

      switch (event.type) {
        case KeyPress: {
          KeySym keysym = XLookupKeysym(&event.xkey, 0);

          if (keysym == XK_Escape) {
            application_running = false;
           }
            break;
         }
        default:
          break;
       }
     }

    clock_gettime(CLOCK_MONOTONIC, &startTime);
    elapsedTime = (endTime.tv_sec - initialTime.tv_sec) + (endTime.tv_nsec - initialTime.tv_nsec) / 1000000000.0;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    GLint timeUniformLocation = glGetUniformLocation(shaderProgram, "u_time");
    glUniform1f(timeUniformLocation, elapsedTime);

    GLint resolutionUniformLocation = glGetUniformLocation(shaderProgram, "u_resolution");
    glUniform2fv(resolutionUniformLocation, 1, resolution);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    GLint positionAttribLocation = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttribLocation);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(positionAttribLocation);

    glXSwapBuffers(display, window);

    clock_gettime(CLOCK_MONOTONIC, &endTime);

    frameTimeNanoseconds = (endTime.tv_sec - startTime.tv_sec) * 1000000000L + (endTime.tv_nsec - startTime.tv_nsec);
    frameTimeMilliseconds = frameTimeNanoseconds / 1000000.0;

    // Print frame time
    printf("Frame time: %ld ns (%.2f ms)\n", frameTimeNanoseconds, frameTimeMilliseconds);

   }

  /*
   * ---10--------20--------30--------40--------50--------60--------70-------80
   */
  glXDestroyContext(display, glxContext);
  XDestroyWindow(display, window);
  XCloseDisplay(display);


  return 0;
 }

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
