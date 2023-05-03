/*
* -----10--------20--------30--------40--------50--------60--------70-------80
*
* P1X DEMO TOOL V3
* by w84death^P1X
*
* https://github.com/w84death/p1x-demo-tool
* (c) 2023.04
*
* -----10--------20--------30--------40--------50--------60--------70-------80
*/

#include <iostream>
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>

const char* fragment_source =
#include "glsl/fragment_shader.glsl"
;
const char* vertex_source =
#include "glsl/vertex_shader.glsl"
;


/*
* -----10--------20--------30--------40--------50--------60--------70-------80
*/
int main() {
    int width = 1280/2;
    int height = 800/2;
    const char *title = "P1X DEMO TOOL V3";

  /*
  * ---10--------20--------30--------40--------50--------60--------70-------80
  */
    // Open an X11 display connection
    Display *display = XOpenDisplay(nullptr);

    // Get the screen number
    int screen = DefaultScreen(display);

    // Choose a framebuffer configuration
    static int visualAttributes[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        None
    };

    int fbCount;
    GLXFBConfig *fbConfigs = glXChooseFBConfig(display, screen, visualAttributes, &fbCount);

    // Get the X Visual info
    GLXFBConfig fbConfig = fbConfigs[0];
    XVisualInfo *visualInfo = glXGetVisualFromFBConfig(display, fbConfig);

    // Create a colormap and set the window attributes
    Window rootWindow = RootWindow(display, screen);
    Colormap colormap = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);

    XSetWindowAttributes windowAttributes;
    windowAttributes.colormap = colormap;
    windowAttributes.background_pixel = WhitePixel(display, screen);
    windowAttributes.border_pixel = 0;
    windowAttributes.event_mask = ExposureMask | KeyPressMask;

    Window window = XCreateWindow(
        display, rootWindow,
        0, 0, width, height, 0,
        visualInfo->depth, InputOutput,
        visualInfo->visual, CWColormap | CWBackPixel | CWBorderPixel | CWEventMask,
        &windowAttributes
    );

    XStoreName(display, window, title);
    XMapWindow(display, window);

    // Create the GLX context
    GLXContext context = glXCreateNewContext(display, fbConfig, GLX_RGBA_TYPE, nullptr, True);

    XFree(visualInfo);

    // Make the GLX context current
    glXMakeCurrent(display, window, context);

  /*
  * ---10--------20--------30--------40--------50--------60--------70-------80
  */
  bool application_running = true;
  XEvent event;
  while (application_running) {
    while (XPending(display)) {
      XNextEvent(display, &event);
      if (event.type == KeyPress) {
        KeySym key = XLookupKeysym(&event.xkey, 0);
        if (key == XK_Escape){
          application_running = false;
        }
      }
    }
  }


  /*
  * ---10--------20--------30--------40--------50--------60--------70-------80
  */
  glXDestroyContext(display, context);
  XCloseDisplay(display);
  return 0;
}

/*
  * -----10--------20--------30--------40--------50--------60--------70-------80
  */
