/*
 * P1X DEMO TOOL V2
 * by w84death^P1X
 * (c)2023.04
 * */

#include <iostream>
#include <string>
#include <chrono>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <thread>
#include <math.h>
#include <cstdio>
#include <random>
#include <alsa/asoundlib.h>

#define GLT_IMPLEMENTATION
#include "gltext.h" /* https://github.com/vallentin/glText */

const char* fragmentSource =
#include "glsl/fragment_shader.glsl"
;
const char* vertexSource =
#include "glsl/vertex_shader.glsl"
;
const char* passFragmentSource =
#include "glsl/pass_fragment_shader.glsl"
;
const char* passVertexSource =
#include "glsl/pass_vertex_shader.glsl"
;

int WIDTH = 1280, HEIGHT = 720;
float resScale = .5f;
float demoTime = 0.0f;
const float demoLength = 60.0f;
bool isPlaying = true;
bool isRunning = true;
bool fullscreen = false;
bool showStats = false;
char stats[512];
char demoName[32] = "CODENAME: SHADER C1TY";
XEvent event;
const char* device_name = "default";
snd_pcm_t* handle;
const float PI = std::acos(-1); // Approximation of pi
const int num_channels = 1;
unsigned int sample_rate = 44100;
snd_pcm_uframes_t buffer_frames = 4096;

GLuint createShaderProgram(const char *vertexSource, const char *fragmentSource);
void fill_frequencies(float* frequencies, int num_frequencies, float base_frequency);
void generate_music(float* buffer, int buffer_size, int sample_rate, float* frequencies, int num_frequencies);
void _init(void){};
void playAudio();
int main(int argc, char* argv[]) {

    // Welcome
    std::cout << "Welcome to the -=[" << demoName << "]=- demo experience.\n"<< std::endl;
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

    // Open the audio device

    snd_pcm_open(&handle, device_name, SND_PCM_STREAM_PLAYBACK, 0);

    // Set the audio parameters

    snd_pcm_hw_params_t* hw_params;
    snd_pcm_hw_params_alloca(&hw_params);
    snd_pcm_hw_params_any(handle, hw_params);
    snd_pcm_hw_params_set_access(handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, hw_params, SND_PCM_FORMAT_FLOAT_LE);
    snd_pcm_hw_params_set_channels(handle, hw_params, num_channels);
    snd_pcm_hw_params_set_rate_near(handle, hw_params, &sample_rate, 0);
    snd_pcm_hw_params_set_period_size_near(handle, hw_params, &buffer_frames, 0);
    snd_pcm_hw_params(handle, hw_params);

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

    // Compile shaders
    GLuint shaderProgram = createShaderProgram(vertexSource, fragmentSource);
    GLuint shaderPassProgram = createShaderProgram(passVertexSource, passFragmentSource);

    // Define vertices for the plane
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

    // Bind texture to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, halfResTexture);

    // Play music (in a thread)
    std::thread audioThread(playAudio);

    // Init time
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> previous_time = start_time;

    while (isRunning) {

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
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, halfResTexture);

        glUseProgram(shaderPassProgram);
        glUniform1i(passthroughTextureLocation, 0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Calculate Delta Time
        std::chrono::time_point<std::chrono::high_resolution_clock> current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = current_time - previous_time;
        double deltaTime = elapsed_seconds.count();
        previous_time = current_time;
        double frameMs = deltaTime*1000;

        // Increment and loop demo time
        if(isPlaying) demoTime += deltaTime;
        if(demoTime > demoLength) demoTime = 0.0f;

        // Draw stats
        if(showStats){
            gltBeginDraw();
            gltColor(1.0f, 1.0f, 1.0f, 1.0f);
            if(frameMs>18.0f) gltColor(1.0f, 0.2f, 0.2f, 1.0f);
            sprintf(stats, "%.0fx%.0f // %0.00f ms\nDemo Time %000.0fs", WIDTH*resScale, HEIGHT*resScale, frameMs, demoTime);
            gltSetText(textStats, stats);
            gltDrawText2D(textStats,32.0f,32.0f,1.0f);
            gltEndDraw();
        }

        // Display
        glXSwapBuffers(display, window);

        // Handle keyboard
        while (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == KeyPress) {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if (key == XK_space) {
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
    audioThread.join();
    snd_pcm_close(handle);
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

GLuint createShaderProgram(const char *vertexSource, const char *fragmentSource){
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *source = vertexSource;
    glShaderSource(vertexShader, 1, &source, 0);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragmentSource;
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
void fill_frequencies(float* frequencies, int num_frequencies, float base_frequency)
{
    const float semitone_ratio = std::pow(2.0f, 1.0f / 12.0f); // 12th root of 2
    float frequency = base_frequency;

    for (int i = 0; i < num_frequencies; ++i) {
        frequencies[i] = frequency;
        frequency *= semitone_ratio;
    }
}
void generate_music(float* buffer, int buffer_size, int sample_rate, float* frequencies, int num_frequencies,int index)
{
    for (int i = 0; i < buffer_size; ++i) {
        float frequency = frequencies[index];

        float t = static_cast<float>(i) / sample_rate;
        float value = std::sin(2 * PI * frequency * t);
        buffer[i] = value;
    }
}
void playAudio(){
    const int buffer_size = buffer_frames * num_channels;
    float buffer[buffer_size];
    const int num_frequencies = 48;
    float frequencies[num_frequencies];
    fill_frequencies(frequencies, num_frequencies, 24.50);
    int i = 0;
    float timer = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    float min = .05f, max = 1.0f;
    std::uniform_int_distribution<> dis(0, num_frequencies - 1);
    std::uniform_real_distribution<> len(min,max);

    while (isRunning) {
        if(isPlaying){
            generate_music(buffer, buffer_size, sample_rate, frequencies, num_frequencies, i);
            int frames_written = snd_pcm_writei(handle, buffer, buffer_frames);
            if (frames_written < 0) snd_pcm_recover(handle, frames_written, 0);
            timer+=0.05;
            if (timer>len(gen)) {
                i = dis(gen);
                timer=0;
            }
        }
    }
}
