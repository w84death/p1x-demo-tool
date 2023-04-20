#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <iostream>

// g++ main.cpp -o demo -std=c++11 -lX11 -lGLEW -lGL -lmikmod && upx --best demo && echo "Done; Run ./demo --stats"

int main() {

    Display *display = XOpenDisplay(NULL);
    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);
    int visual_attribs[] = {
        GLX_RGBA,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER,
        None
    };
    XVisualInfo *visual_info = glXChooseVisual(display, screen, visual_attribs);
    Colormap colormap = XCreateColormap(display, root, visual_info->visual, AllocNone);
    XSetWindowAttributes window_attribs;
    window_attribs.colormap = colormap;
    window_attribs.event_mask = ExposureMask | KeyPressMask;
    Window window = XCreateWindow(
        display,
        root,
        0, 0, 640, 480,
        0,
        visual_info->depth,
        InputOutput,
        visual_info->visual,
        CWColormap | CWEventMask,
        &window_attribs
    );
    XStoreName(display, window, "OpenGL X11 Window");

    GLXContext context = glXCreateContext(display, visual_info, NULL, GL_TRUE);
    glXMakeCurrent(display, window, context);
    XMapWindow(display, window);


    while (true) {
        XEvent event;
        XNextEvent(display, &event);

        if (event.type == Expose) {
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glXSwapBuffers(display, window);
        } else if (event.type == KeyPress) {
            break;
        }
    }

    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, context);
    XFree(visual_info);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
