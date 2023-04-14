# P1T - P1X 1ntro Tool

C++ tools for creating GLSL intros (demos up to 64KB).

!(screen-dev.png)[Development Screenshot]

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
  --fullscreen \
  --shader shader.glsl
```

Those are defaults.

## Requiments

On Steam Deck (Arch Linux) I needed to intall:
```
sudo pacman -S sdl2 glew g++ inotify-tools base-devel glibc linux-api-headers sdl2-dev glu libgl
```

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
