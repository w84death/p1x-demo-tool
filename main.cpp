/*
 * P1X 1ntro Tool
 * by Krzysztof Krystian Jankowski
 *
 * (c) 2023.04
 * https://github.com/w84death/p1x-intro-tool
 * */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_mixer.h>

// Default values
const char* appName = "P1X INTRO TOOL";
float windowWidth = 640.0f;
float windowHeight = 360.0f;
float resolutionPercentage = 0.5f;
bool fullscreen = false;
std::string shaderFileName = "shader.glsl";

// Application variables
Uint32 frameStart;
int frameTime;
bool isPlaying = true;
int seekAmount = 1000;


// Function prototypes
std::string readFile(const std::string& filePath);
GLuint createShader(GLenum shaderType, const std::string& source);
GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
void render(GLuint shaderProgram, GLuint VAO);

int main(int argc, char* argv[]) {
  // Parse command-line arguments
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "--width" && i + 1 < argc) {
      windowWidth = std::stof(argv[++i]);
    } else if (arg == "--height" && i + 1 < argc) {
      windowHeight = std::stof(argv[++i]);
    } else if (arg == "--percentage" && i + 1 < argc) {
      resolutionPercentage = std::stof(argv[++i]);
    } else if (arg == "--fullscreen") {
      fullscreen = true;
    } else if (arg == "--shader" && i + 1 < argc) {
      shaderFileName = argv[++i];
    }
  }

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Set OpenGL attributes
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // Create the SDL window with the specified width, height, and fullscreen mode
  Uint32 windowFlags = SDL_WINDOW_OPENGL;
  if (fullscreen) {
    windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }
  SDL_Window* window = SDL_CreateWindow(appName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowFlags);

  if (!window) {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  // Create OpenGL context
  SDL_GLContext context = SDL_GL_CreateContext(window);
  if (!context) {
    std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Initialize SDL_mixer
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    return 1;
  }

  // Load music file
  std::string musicFileName = "music.mp3";
  Mix_Music* music = Mix_LoadMUS(musicFileName.c_str());
  if (music == nullptr) {
    std::cerr << "Failed to load music file: " << musicFileName << ", SDL_mixer Error: " << Mix_GetError() << std::endl;
    return 1;
  }

  //  Vertex Shader
  std::stringstream vertexShaderSourceStream;
  vertexShaderSourceStream << R"glsl(
    #version 330 core
    layout (location = 0) in vec2 aPos;

    out vec2 TexCoord;

    void main() {
      gl_Position = vec4(aPos, 0.0, 1.0);
      TexCoord = (2.0 * (aPos) * vec2()glsl" << (windowWidth / resolutionPercentage) << ", " << (windowHeight / resolutionPercentage) << R"glsl())/ )glsl" << (windowHeight / resolutionPercentage) << R"glsl(;
    }
  )glsl";
  std::string vertexShaderSource = vertexShaderSourceStream.str();

  //  Fragment Shader (default or from arguments)
  std::ifstream shaderFile(shaderFileName);
  if (!shaderFile.is_open()) {
    std::cerr << "Failed to open shader file: " << shaderFileName << std::endl;
    return 1;
  }
  std::stringstream shaderStream;
  shaderStream << shaderFile.rdbuf();
  shaderFile.close();
  std::string fragmentShaderSource = shaderStream.str();

  // Compile shaders and create shader program
  GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

  // Create a VAO, VBO, and EBO for rendering a fullscreen quad
  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  GLfloat vertices[] = {
    -1.0f,  1.0f,
    -1.0f, -1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f
  };

  GLuint indices[] = {
    0, 1, 2,
    2, 3, 0
  };

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  float startTime = SDL_GetTicks() / 1000.0f;
  float lastTime = startTime;
  GLint timeUniformLocation = glGetUniformLocation(shaderProgram, "u_time");

  // Render loop
  bool running = true;
  SDL_Event event;

  // Play MIDI file (loop indefinitely)
  Mix_PlayMusic(music, -1);

  while (running) {
    frameStart = SDL_GetTicks();
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            running = false;
            break;
          case SDLK_SPACE:
            if (isPlaying) {
              Mix_PauseMusic();
            } else {
              Mix_ResumeMusic();
            }
            isPlaying = !isPlaying;
            break;
          case SDLK_LEFT:
          if (event.key.keysym.mod & KMOD_SHIFT) {
            if(Mix_GetMusicPosition(music)>0.1){
              Mix_SetMusicPosition(Mix_GetMusicPosition(music) - 0.1);
            }else {
              Mix_SetMusicPosition(0.0);
            }
          } else {
            if(Mix_GetMusicPosition(music)>1.0){
              Mix_SetMusicPosition(Mix_GetMusicPosition(music) - 1.0);
            }else{
              Mix_SetMusicPosition(0.0);
            }
          }
          break;
        case SDLK_RIGHT:
          if (event.key.keysym.mod & KMOD_SHIFT) {
            Mix_SetMusicPosition(Mix_GetMusicPosition(music) + 0.1);
          } else {
            Mix_SetMusicPosition(Mix_GetMusicPosition(music) + seekAmount / 1000.0);
          }
          break;
          default:
            break;
        }
      }
    }

    // Calculate FPS and frame time
    float currentTime = SDL_GetTicks() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    int fps = static_cast<int>(1.0f / deltaTime);
    int frameMs = static_cast<int>(deltaTime * 1000);

    // Send timer to the shader
    float introTime = Mix_GetMusicPosition(music);
    glUniform1f(timeUniformLocation, introTime);

    // Print FPS and frame time to console
    std::cout << "FPS: " << fps << " | " << frameMs << " ms" << " --- INTRO TIME: " << introTime << "s ---" << std::endl;

    render(shaderProgram, VAO);
    SDL_GL_SwapWindow(window);
  }

  // Cleanup
  Mix_FreeMusic(music);
  Mix_CloseAudio();
  glDeleteBuffers(1, &EBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteProgram(shaderProgram);
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

// Compiling Shader
GLuint createShader(GLenum shaderType, const std::string& source) {
  GLuint shader = glCreateShader(shaderType);
  const char* sourceCStr = source.c_str();
  glShaderSource(shader, 1, &sourceCStr, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cerr << "Failed to compile shader: " << infoLog << std::endl;
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

// Linking shader program
GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
  GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
  GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  GLint success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    std::cerr << "Failed to link shader program: " << infoLog << std::endl;
    glDeleteProgram(shaderProgram);
    shaderProgram = 0;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

// Render shader program
void render(GLuint shaderProgram, GLuint VAO) {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shaderProgram);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

