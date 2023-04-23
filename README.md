# P1X Demo Tool V2

*WORK IN PROGRESS*

Tool written in C++ and OpenGL 4.6 designed to render a fullscreen, single GLSL demo. Targetting 64K.

![Demo Screenshot](media/screen2.jpg)

Technical Details:

### The C++ application
* The application uses **X11** for window management and user input handling.
* **GLEW** is utilized for accessing OpenGL functions.
* **OpenGL 4.6** Core Profile is used for rendering.
* **ALSA** for audio generation.
* Vertex and fragment shaders are loaded from external files.
* The application renders the scene to a lower resolution texture using a framebuffer.
* The lower resolution texture is then rendered to the screen (bigger) framebuffer using a passthrough shader.
* **Keyboard input** is handled to control demo playback (pause/play, time scrubbing) and window closing (ESC key).
* The application supports command-line arguments for setting the initial window dimensions and resolution scale.

### The GLSL Shader
* [Ray marching](https://en.wikipedia.org/wiki/Ray_marching) to render the 3D scene
* [Signed distance fuctions](https://en.wikipedia.org/wiki/Signed_distance_function) to generate all the 3D models
* Shoft shadows
* Vignette and lens distortions
* Ambient Occlusion
* Color/exposure correction


## Synthesizer

Simple synthesizer written in C++ using ALSA for audio output. It supports basic synthesis techniques for creating a few different instruments, including a sine wave-based synthesizer, a kick drum, a hi-hat, an arpeggiator synth, and an electric piano. The synthesizer is designed to play multiple tracks of MIDI-like note data simultaneously, allowing for the creation of simple multi-instrument compositions.
Features

* Sine wave-based synthesis for creating simple melodic instruments
* Kick drum synthesis using a combination of sine waves and an exponential decay envelope
* Hi-hat synthesis using white noise and an exponential decay envelope
* Arpeggiator synth with configurable arpeggio patterns and sine wave synthesis
* Electric piano synthesis using a combination of sine waves, varying amplitudes, and a basic ADSR envelope
* Multi-track playback, allowing for simultaneous playback of different instruments
* Simple note data format for composing tracks
* Real-time playback control, including pause and resume functionality
* Utilizes the ALSA library for audio output on Linux systems

## Run the demo

## Usage
```./demo```

For changing parameters use --help:
```./demo --help```

For slower computers try:
```./demo --width 640 --height 360 --resolution-scale 0.25 --stats```

## Compiling and development

![Development Screenshot](media/screen1.jpg)

Ubuntu
```apt install libglew-dev x11-dev libasound2-dev upx```

Arch Linux (Steam Deck)
```pacman -S glew libx11 xorgproto upx```

### Compile and compress binary
```./compile.sh```
