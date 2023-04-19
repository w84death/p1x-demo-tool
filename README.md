# P1X Demo Tool V2

OpenGL 4 tool written in C++ designed to render a fullscreen single GLSL demo. Targetting 64K.

![Demo Screenshot](media/screen2.jpg)

Technical Details:

* The application uses **GLFW** for window management and user input handling.
* **GLEW** is utilized for accessing OpenGL functions.
* **OpenGL 4.6** Core Profile is used for rendering.
* Vertex and fragment shaders are loaded from external files using a custom Shader class.
* The application renders the scene to a lower resolution texture using a framebuffer.
* The lower resolution texture is then rendered to the screen framebuffer using a passthrough shader.
* **Keyboard input** is handled to control demo playback (pause/play, time scrubbing) and window closing (ESC key).
* The application supports command-line arguments for setting the initial window dimensions and resolution scale.

## Run the demo


## Usage
```./demo```

For slower computers try:
```./demoLinux --width 640 --height 360 --resolution-scale 0.25 --stats```

For statistics use ```--stats```, for fullscreen use ```--fullscreen```.

## Compiling and development

![Development Screenshot](media/screen1.jpg)

Ubuntu
```apt install libglew-dev libglfw3-dev upx```

Arch Linux (Steam Deck)
```pacman -S glfw glew upx```

### Compile and compress binary
```./compile.sh```
