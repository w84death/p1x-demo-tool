/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 *
 * P1X DEMO TOOL V2
 * by w84death^P1X
 *
 * https://github.com/w84death/p1x-demo-tool
 * (c) 2023.04
 *
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
#include <iostream>
#include <string>
#include <chrono>
#include <math.h>
#include <cstdio>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include "synthesizer.h"
#define GLT_IMPLEMENTATION
#include "gltext.h" /* https://github.com/vallentin/glText */
const char* fragment_source =
#include "glsl/fragment_shader.glsl"
;
const char* vertex_source =
#include "glsl/vertex_shader.glsl"
;
const char* pass_fragment_sourcec =
#include "glsl/pass_fragment_shader.glsl"
;
const char* pass_vertex_sourcec =
#include "glsl/pass_vertex_shader.glsl"
;

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
char demo_name[32] = "CODENAME: SHADER C1TY";
float demo_time = 0.0f;
const float demo_length = 60.0f;

int window_width = 1280, window_height = 720;
bool window_fullscreen = false;
float resolution_scale = .5f;
bool application_running = true;
bool debug_show_stats = false;
char stats[512];
XEvent event;

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
GLuint create_shader_program(const char *vertex_source, const char *fragment_source){
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *source = vertex_source;
    glShaderSource(vertexShader, 1, &source, 0);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragment_source;
    glShaderSource(fragmentShader, 1, &source, 0);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);

    return shaderProgram;
};

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
void print_help() {
    std::cout << "Usage: ./demo [options]\n\n"
              << "Options:\n"
              << "  --width <value>           Set window width\n"
              << "  --height <value>          Set window height\n"
              << "  --resolution-scale <value> Set resolution scale\n"
              << "  --stats                   Show statistics\n"
              << "  --window_fullscreen       Enable fullscreen mode\n"
              << "  --help                    Display this help message\n"
              << std::endl;
}

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
int main(int argc, char* argv[]) {

    std::cout << "P1X DEMO TOOL V2 by w84death^P1X // (c) 2023.04\n"<< std::endl;
    std::cout << "DEVELOPMENT VERSION. BUGS ARE EXPECTED. CTRL+C TO KILL THE DEMO.\n"<< std::endl;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help") {
            print_help();
            return 0;
        } else if (arg == "--width" && i + 1 < argc) {
            window_width = std::stof(argv[++i]);
        } else if (arg == "--height" && i + 1 < argc) {
            window_height = std::stof(argv[++i]);
        } else if (arg == "--resolution-scale" && i + 1 < argc) {
            resolution_scale = std::stof(argv[++i]);
        } else if (arg == "--stats") {
            debug_show_stats = true;
            debug_show_notes = true;
        } else if (arg == "--window_fullscreen") {
            window_fullscreen = true;
        } else {
            std::cout << "> Wrong argument: " << arg << std::endl;
            print_help();
            return 0;
        }
    }

    std::cout << "> Initializing engine with:\n  - window resolution " << window_width << "x" << window_height << "\n  - internal rendering resolution " << window_width*resolution_scale << "x" << window_height*resolution_scale << " (scale " << resolution_scale << ")."<< std::endl;
/*
 *
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
    synthStart();

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
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
        0, 0, window_width, window_height,
        0,
        visual_info->depth,
        InputOutput,
        visual_info->visual,
        CWColormap | CWEventMask,
        &window_attribs
    );
    XStoreName(display, window, demo_name);

    GLXContext context = glXCreateContext(display, visual_info, NULL, GL_TRUE);
    glXMakeCurrent(display, window, context);
    XMapWindow(display, window);


/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, window_width, window_height);

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
    gltInit();
    GLTtext *textStats = gltCreateText();

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
    GLuint shaderProgram = create_shader_program(vertex_source, fragment_source);
    GLuint shaderPassProgram = create_shader_program(pass_vertex_sourcec, pass_fragment_sourcec);

    GLfloat vertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };
    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLuint halfResTexture;
    glGenTextures(1, &halfResTexture);
    glBindTexture(GL_TEXTURE_2D, halfResTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width*resolution_scale, window_height*resolution_scale, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, halfResTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, halfResTexture);

    GLint timeLocation = glGetUniformLocation(shaderProgram, "time");
    GLint widthLocation = glGetUniformLocation(shaderProgram, "width");
    GLint heightLocation = glGetUniformLocation(shaderProgram, "height");
    GLint passthroughTextureLocation = glGetUniformLocation(shaderPassProgram, "u_texture");

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> previous_time = start_time;

    while (application_running) {

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, window_width*resolution_scale, window_height*resolution_scale);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1f(timeLocation, demo_time);
        glUniform1f(widthLocation, static_cast<float>(window_width*resolution_scale));
        glUniform1f(heightLocation, static_cast<float>(window_height*resolution_scale));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, halfResTexture);

        glUseProgram(shaderPassProgram);
        glUniform1i(passthroughTextureLocation, 0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        std::chrono::time_point<std::chrono::high_resolution_clock> current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = current_time - previous_time;
        double deltaTime = elapsed_seconds.count();
        previous_time = current_time;
        double frameMs = deltaTime*1000;

        if(!pause_playback) demo_time += deltaTime;
        if(demo_time > demo_length) demo_time = 0.0f;

        if(debug_show_stats){
            gltBeginDraw();
            gltColor(1.0f, 1.0f, 1.0f, 1.0f);
            if(frameMs>18.0f) gltColor(1.0f, 0.2f, 0.2f, 1.0f);
            sprintf(stats, "%.0fx%.0f // %0.00f ms\nDemo Time %000.0fs", window_width*resolution_scale, window_height*resolution_scale, frameMs, demo_time);
            gltSetText(textStats, stats);
            gltDrawText2D(textStats,32.0f,32.0f,1.0f);
            gltEndDraw();
        }

        glXSwapBuffers(display, window);

        while (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == KeyPress) {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if (key == XK_space) {
                    pause_playback.store(!pause_playback.load());
                }else
                if (key == XK_Up || key == XK_Down || key == XK_Left || key == XK_Right) {
                    // Arrow key was pressed

                    if (event.xkey.state & ShiftMask) {
                        // Arrow + Shift key was pressed

                    }
                }else{
                    application_running = false;
                    quit_playback = true;
                }
            }
        }
    }

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
    synthQuit();
    gltDeleteText(textStats);
    gltTerminate();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, context);
    XFree(visual_info);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
