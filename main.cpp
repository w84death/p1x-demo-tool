#include <cstdio>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#define GLT_IMPLEMENTATION
#include "gltext.h" /* https://github.com/vallentin/glText */
#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_FLAC
#define MA_NO_DSOUND
#define MA_NO_WEBAUDIO
#define MA_NO_WASAPI
#define MA_NO_OPENSL
#define MA_NO_WAV
#define MA_DEBUG_OUTPUT
#define MA_NO_AAUDIO
#define MA_NO_PULSEAUDIO
#define MA_NO_JACK
#define MA_NO_AUDIO4
#define MA_NO_OSS
#define MA_NO_SNDIO
#define MA_NO_COREAUDIO
#define MA_NO_WINMM
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_OPENSL
#define MA_NO_NODE_GRAPH
#define MA_NO_ENGINE
#define MA_NO_ENCODING
#define MA_NO_DEBUG
#include "miniaudio.h" /* https://miniaud.io/ */

int WIDTH = 1280, HEIGHT = 720;
float resScale = .5f;
float demoTime = 0.0f;
const float demoLength = 60.0f;
bool isPlaying = true;
bool fullscreen = false;
float lastConsoleOut = -0.1f;
bool showStats = false;
char stats[512];
char demoName[32] = "P1X DEMO TOOL V2";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        std::cout << "Bye!\n\n(c)2023.04 w84death / P1X" << std::endl;
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        isPlaying = !isPlaying;
    }

    if (key == GLFW_KEY_LEFT) {
        if (mods & GLFW_MOD_SHIFT) {
            if(demoTime>0.1) demoTime -= 0.1;
            else demoTime=0.0;
        } else {
            if(demoTime>1.0) demoTime -= 1.0;
            else demoTime=0.0;
        }
    }

    if (key == GLFW_KEY_RIGHT) {
        if (mods & GLFW_MOD_SHIFT) {
            if(demoTime<demoLength-0.1) demoTime += 0.1;
            else demoTime=demoLength;
        } else {
            if(demoTime<demoLength-1.0) demoTime += 1.0;
            else demoTime=demoLength;
        }
    }

    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        if (mods & GLFW_MOD_SHIFT) {
            // Handle up arrow press with Shift
        } else {
            // Handle up arrow press without Shift
        }
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        if (mods & GLFW_MOD_SHIFT) {
            // Handle down arrow press with Shift
        } else {
            // Handle down arrow press without Shift
        }
    }
}
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}
void _init(void){};
int main(int argc, char* argv[]) {
    std::cout << "Welcome to the -=[" << demoName << "]=- demo expeience.\n"<< std::endl;

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
            std::cout << "Wrong pargument: " << arg << "\n\nUsage:\n$ ./demo --width 640 --height 360 --resolution-scale 0.25\n\nFor statistics use --stats, for fullscreen use --fullscreen." << std::endl;
            return 0;
       }
    }

    std::cout << "Initializing engine with window resolution " << WIDTH << "x" << HEIGHT << ", internal rendering resolution " << WIDTH*resScale << "x" << HEIGHT*resScale << " (scale " << resScale << ")."<< std::endl;

    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;
    result = ma_decoder_init_file("music.mp3", NULL, &decoder);
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;
    ma_device_init(NULL, &deviceConfig, &device);
    ma_device_start(&device);


    glfwInit();

    GLFWmonitor* monitor = nullptr;
    const GLFWvidmode* videoMode = nullptr;

    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        videoMode = glfwGetVideoMode(monitor);
        WIDTH = videoMode->width;
        HEIGHT = videoMode->height;
    }

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, demoName, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

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
    Shader shader("vertex_shader.glsl","fragment_shader.glsl");
    Shader passthroughShader("pass_vertex_shader.glsl", "pass_fragment_shader.glsl");


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
    GLint timeLocation = glGetUniformLocation(shader.Program, "time");
    GLint widthLocation = glGetUniformLocation(shader.Program, "width");
    GLint heightLocation = glGetUniformLocation(shader.Program, "height");
    GLint passthroughTextureLocation = glGetUniformLocation(passthroughShader.Program, "u_texture");

    float lastTime = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        // Render to half-resolution framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, WIDTH*resScale, HEIGHT*resScale);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();
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

        passthroughShader.Use();
        glUniform1i(passthroughTextureLocation, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, halfResTexture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        float nowTime = static_cast<float>(glfwGetTime());
        float deltaTime = nowTime - lastTime;
        lastTime = nowTime;
        int fps = static_cast<int>(1.0f / deltaTime);
        int frameMs = static_cast<int>(deltaTime * 1000);

        if(isPlaying) demoTime += deltaTime;

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
            char* paused="PLAYING...";
            if(!isPlaying) paused="-- PAUSED --";
            sprintf(stats, "%.0fx%.0f - %d fps, %d ms\nDemo Time %000.0fs %s", WIDTH*resScale, HEIGHT*resScale, fps, frameMs, demoTime, paused);
            gltSetText(textStats, stats);
            gltDrawText2DAligned(textStats,
                (GLfloat)(WIDTH*.5f),
                32.0f,
                1.0f,
                GLT_CENTER, GLT_CENTER);
        }


        gltEndDraw();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    gltDeleteText(textDemoName);
    gltDeleteText(textStats);
    gltTerminate();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();

    return 0;
}
