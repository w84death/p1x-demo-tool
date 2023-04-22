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
#include <thread>
#include <math.h>
#include <cstdio>
#include <random>
#include <vector>
#include <atomic>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <alsa/asoundlib.h>
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
bool demo_playing = true;
bool debug_show_stats = false;
char stats[512];
std::atomic<bool> pause_playback{false};
std::atomic<bool> quit_playback{false};
XEvent event;

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
struct Note {
    int pitch;      // MIDI note number
    float duration; // duration in seconds
};


/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
float sine_wave(float frequency, float time) {
    const float PI = 3.14159265;
    return sin(2 * PI * frequency * time);
}

float midi_note_to_frequency(int note) {
    return 440.0f * pow(2.0f, (note - 69) / 12.0f);
}

void play_note(Note note, snd_pcm_t *handle, int sample_rate) {
    int buffer_size = sample_rate * note.duration;
    float buffer[buffer_size];

    for (int i = 0; i < buffer_size; ++i) {
        buffer[i] = sine_wave(midi_note_to_frequency(note.pitch), static_cast<float>(i) / sample_rate);
    }

    snd_pcm_writei(handle, buffer, buffer_size);
}

void playback_thread(std::vector<Note> &notes, int sample_rate) {
    snd_pcm_t *handle;
    snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(handle, SND_PCM_FORMAT_FLOAT, SND_PCM_ACCESS_RW_INTERLEAVED, 1, sample_rate, 1, 100000);

    size_t current_note = 0;
    bool is_paused = false;

    while (!quit_playback) {
        if (!pause_playback) {
            if (is_paused) {
                snd_pcm_pause(handle, 0); // Resume ALSA playback
                is_paused = false;
            }

            if (current_note < notes.size()) {
                play_note(notes[current_note], handle, sample_rate);
                current_note++;
            } else {
                current_note = 0;
            }
        } else {
            if (!is_paused) {
                snd_pcm_pause(handle, 1); // Pause ALSA playback
                is_paused = true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    snd_pcm_close(handle);
}

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
GLuint createShaderProgram(const char *vertex_source, const char *fragment_source){
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

    std::cout << "Welcome to the -=[" << demo_name << "]=- demo experience.\n"<< std::endl;
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
        } else if (arg == "--window_fullscreen") {
            window_fullscreen = true;
        } else {
            std::cout << "> Wrong argument: " << arg << std::endl;
            print_help();
            return 0;
        }
    }

    std::cout << "> Initializing engine with window resolution " << window_width << "x" << window_height << ", internal rendering resolution " << window_width*resolution_scale << "x" << window_height*resolution_scale << " (scale " << resolution_scale << ")."<< std::endl;

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
    std::vector<Note> notes = {
        {60, 0.5f},
        {62, 0.5f},
        {64, 0.5f},
        {65, 0.5f},
        {67, 0.5f},
        {69, 0.5f},
        {71, 0.5f},
        {72, 0.5f},
    };

    int sample_rate = 44100;
    std::thread player(playback_thread, std::ref(notes), sample_rate);

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

    // Creating text
    GLTtext *textDemoName = gltCreateText();
    GLTtext *textStats = gltCreateText();
    gltSetText(textDemoName, demo_name);

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
    GLuint shaderProgram = createShaderProgram(vertex_source, fragment_source);
    GLuint shaderPassProgram = createShaderProgram(pass_vertex_sourcec, pass_fragment_sourcec);

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width*resolution_scale, window_height*resolution_scale, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
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

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> previous_time = start_time;

    while (application_running) {

        // Render to half-resolution framebuffer
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

        // Render to screen framebuffer
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

        // Calculate Delta Time
        std::chrono::time_point<std::chrono::high_resolution_clock> current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = current_time - previous_time;
        double deltaTime = elapsed_seconds.count();
        previous_time = current_time;
        double frameMs = deltaTime*1000;

        // Increment and loop demo time
        if(demo_playing) demo_time += deltaTime;
        if(demo_time > demo_length) demo_time = 0.0f;

        // Draw stats
        if(debug_show_stats){
            gltBeginDraw();
            gltColor(1.0f, 1.0f, 1.0f, 1.0f);
            if(frameMs>18.0f) gltColor(1.0f, 0.2f, 0.2f, 1.0f);
            sprintf(stats, "%.0fx%.0f // %0.00f ms\nDemo Time %000.0fs", window_width*resolution_scale, window_height*resolution_scale, frameMs, demo_time);
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
                    demo_playing = !demo_playing;
                    pause_playback.store(!pause_playback.load());
                }else
                if (key == XK_Up || key == XK_Down || key == XK_Left || key == XK_Right) {
                    // Arrow key was pressed

                    if (event.xkey.state & ShiftMask) {
                        // Arrow + Shift key was pressed

                    }
                }else
                    application_running = false;
            }
        }
    }

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
    quit_playback = true;
    player.join();
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

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
