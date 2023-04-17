# P1X Demo Tool V2

OpenGL 4 demo tool written in C++ designed to render a fullscreen GLSL demo. Targetting 64K.

![Development Screenshot](screen1.jpg)

![Demo Screenshot](screen2.jpg)

Technical Details:

    The application uses GLFW for window management and user input handling.
    GLEW is utilized for accessing OpenGL functions.
    OpenGL 4.0 Core Profile is used for rendering.
    GLM library is included for mathematical operations (although not used in the current code).
    Vertex and fragment shaders are loaded from external files using a custom Shader class.
    The application renders the scene to a half-resolution texture using a framebuffer.
    The half-resolution texture is then rendered to the screen framebuffer using a passthrough shader.
    Keyboard input is handled to control demo playback (pause/play, time scrubbing) and window closing (ESC key).
    The application supports command-line arguments for setting the initial window dimensions and enabling fullscreen mode.

## Requiments

### Ubuntu
```apt install libglfw3```

### Arch Linux (Steam Deck)
```pacman -S glfw```

## Usage

```
./demo
./demo --width 640 --height 360 --resolution-scale 0.25
```

For statistics use ```--stats```, for fullscreen use ```--fullscreen```.

## Compiling

### Ubuntu
```
apt install libglew-dev libglfw3-dev upx```
```

### Arch Linux (Steam Deck)
```pacman -S glfw glm glew upx```

```
./compile.sh
```
