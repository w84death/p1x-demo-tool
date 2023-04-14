# P1T - P1X 1ntro Tool

C++ tools for creating GLSL intros (demos up to 64KB).

## How to use

Compile and run:
```
./compile
./intro
```

Additional parameters:
```
./intro \
  --width 640 \
  --height 360 \
  --percentage 0.5 \
  --fullscreen
```

Those are default.

## Shader bootstraping app features

### Done
- initialize SDL window with one, fullscreen shader
- sends resolution/percentage to the shader
- calculates deltaTime (see console output)
- compiling script
- running script
- quits in [ESC]
- application arguments for changing resolution/screen percentage/fullscreen

### Planned
- audio/midi support
- mouse position support
- custom uniforms to tweak on runtime

## Shader (engine) features

### Done
- raymarching
- primitives SDFs
- SDFs operators
- materials
- camera control
- time synchronization (via map function)
- color correction
- AO
- soft shadows

### Planned
- fix repetition
- more usefule SDF ops
- reflective materials
- reflections
- mouse support for camera control
- support for custom uniforms
