#include <cstdio>
#include <iostream>
#include <string>
#include <ctime>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <mikmod.h>
#define GLT_IMPLEMENTATION
#include "gltext.h" /* https://github.com/vallentin/glText */

const std::string fragmentSource =
#include "glsl/fragment_shader.glsl"
;
const std::string vertexSource =
#include "glsl/vertex_shader.glsl"
;
const std::string passFragmentSource =
#include "glsl/pass_fragment_shader.glsl"
;
const std::string passVertexSource =
#include "glsl/pass_vertex_shader.glsl"
;

int WIDTH = 1280, HEIGHT = 720;
float resScale = .5f;
float demoTime = 0.0f;
const float demoLength = 60.0f;
bool isPlaying = true;
bool isRunning = true;
bool fullscreen = false;
float lastConsoleOut = -0.1f;
bool showStats = false;
char stats[512];
char demoName[32] = "SHADER C1TY.";
XEvent event;
MODULE *module;

void _init(void){};
int main(int argc, char* argv[]) {

    // Welcome
    std::cout << "Welcome to the -=[" << demoName << "]=- demo expeience.\n"<< std::endl;
    std::cout << "DEVELOPMENT VERSION. BUGS ARE EXPECTED. CTRL+C TO KILL THE DEMO.\n"<< std::endl;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--width" && i + 1 < argc) {
            WIDTH = std::stof(argv[++i]);
        } else if (arg == "--height" && i + 1 < argc) {
            HEIGHT = std::stof(argv[++i]);
        } else if (arg == "--resolution-scale" && i + 1 < argc){
            resScale = std::stof(argv[++i]);
        }else if (arg == "--stats") {
            showStats = true;
        }else if (arg == "--fullscreen") {
            fullscreen = true;
        }else {
            std::cout << "> Wrong pargument: " << arg << "\n\n> Usage:\n$ ./demo --width 640 --height 360 --resolution-scale 0.25\n\nFor statistics use --stats, for fullscreen use --fullscreen.\n" << std::endl;
            return 0;
       }
    }

    std::cout << "> Initializing engine with window resolution " << WIDTH << "x" << HEIGHT << ", internal rendering resolution " << WIDTH*resScale << "x" << HEIGHT*resScale << " (scale " << resScale << ")."<< std::endl;

    // Mod music
    MikMod_RegisterAllDrivers();
    // MikMod_RegisterLoader(&load_mod);
    MikMod_RegisterLoader(&load_xm);
    MikMod_Init("");
    module = Player_Load("music.xm", 64, 0);

    // Init Xwindow
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
        0, 0, WIDTH, HEIGHT,
        0,
        visual_info->depth,
        InputOutput,
        visual_info->visual,
        CWColormap | CWEventMask,
        &window_attribs
    );
    XStoreName(display, window, demoName);

    GLXContext context = glXCreateContext(display, visual_info, NULL, GL_TRUE);
    glXMakeCurrent(display, window, context);
    XMapWindow(display, window);


    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // Set viewport
    glViewport(0, 0, WIDTH, HEIGHT);

    // Initialize glText
    gltInit();

    // Creating text
    GLTtext *textDemoName = gltCreateText();
    GLTtext *textStats = gltCreateText();
    gltSetText(textDemoName, demoName);

    // Load shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *source = (const GLchar *)vertexSource.c_str();
    glShaderSource(vertexShader, 1, &source, 0);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    source = (const GLchar *)fragmentSource.c_str();
    glShaderSource(fragmentShader, 1, &source, 0);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);

    GLuint vertexPassShader = glCreateShader(GL_VERTEX_SHADER);
    source = (const GLchar *)passVertexSource.c_str();
    glShaderSource(vertexPassShader, 1, &source, 0);
    glCompileShader(vertexPassShader);

    GLuint fragmentPassShader = glCreateShader(GL_FRAGMENT_SHADER);
    source = (const GLchar *)passFragmentSource.c_str();
    glShaderSource(fragmentPassShader, 1, &source, 0);
    glCompileShader(fragmentPassShader);

    GLuint shaderPassProgram = glCreateProgram();
    glAttachShader(shaderPassProgram, vertexPassShader);
    glAttachShader(shaderPassProgram, fragmentPassShader);
    glLinkProgram(shaderPassProgram);
    glDetachShader(shaderPassProgram, vertexPassShader);
    glDetachShader(shaderPassProgram, fragmentPassShader);

    // Define vertices for the plane
    GLfloat vertices[] = {
        // Positions      // Texture Coords
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

    // Vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Create half-resolution framebuffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create half-resolution texture
    GLuint halfResTexture;
    glGenTextures(1, &halfResTexture);
    glBindTexture(GL_TEXTURE_2D, halfResTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH*resScale, HEIGHT*resScale, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, halfResTexture, 0);

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Uniforms
    GLint timeLocation = glGetUniformLocation(shaderProgram, "time");
    GLint widthLocation = glGetUniformLocation(shaderProgram, "width");
    GLint heightLocation = glGetUniformLocation(shaderProgram, "height");
    GLint passthroughTextureLocation = glGetUniformLocation(shaderPassProgram, "u_texture");

    float lastTime = (float)std::time(0);
    Player_Start(module);

    while (isRunning) {
        MikMod_Update();

        // Render to half-resolution framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, WIDTH*resScale, HEIGHT*resScale);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1f(timeLocation, demoTime);
        glUniform1f(widthLocation, static_cast<float>(WIDTH*resScale));
        glUniform1f(heightLocation, static_cast<float>(HEIGHT*resScale));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Render to screen framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, WIDTH, HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderPassProgram);
        glUniform1i(passthroughTextureLocation, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, halfResTexture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        float nowTime = (float)std::time(0);
        float deltaTime = nowTime - lastTime;
        lastTime = nowTime;
        int fps = static_cast<int>(1.0f / deltaTime);
        int frameMs = static_cast<int>(deltaTime * 1000);

        if(isPlaying) demoTime += .0333;

        // Text drawing
        gltBeginDraw();

        gltColor(1.0f, 1.0f, 1.0f, 1.0f);

        if(demoTime > 2.0f && demoTime < 5.0f)
            gltDrawText2DAligned(textDemoName,
                (GLfloat)(WIDTH*.5f),
                (GLfloat)(HEIGHT*.5f),
                5.0f,
                GLT_CENTER, GLT_CENTER);

        if(showStats){
            sprintf(stats, "%.0fx%.0f - %d fps, %d ms\nDemo Time %000.0fs", WIDTH*resScale, HEIGHT*resScale, fps, frameMs, demoTime);
            gltSetText(textStats, stats);
            gltDrawText2DAligned(textStats,
                (GLfloat)(WIDTH*.5f),
                32.0f,
                1.0f,
                GLT_CENTER, GLT_CENTER);
        }

        gltEndDraw();
        glXSwapBuffers(display, window);
        while (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == KeyPress) {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if (key == XK_space) {
                    Player_TogglePause();
                    isPlaying = !isPlaying;
                }else
                if (key == XK_Up || key == XK_Down || key == XK_Left || key == XK_Right) {
                    // Arrow key was pressed

                    if (event.xkey.state & ShiftMask) {
                        // Arrow + Shift key was pressed


                    }
                }else
                    isRunning = false;
            }
        }
    }

    // Clean up
    Player_Stop();
    Player_Free(module);
    MikMod_Exit();
    gltDeleteText(textDemoName);
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
