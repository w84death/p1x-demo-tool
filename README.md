# P1T - P1X 1ntro Tool

C++ tools for creating GLSL intros (demos up to 64KB).

## Features

The engine itself.

### Done
- initialize SDL window with one, fullscreen shader
- sends resolution/percentage to the shader
- calculates deltaTime (see console output)
- compiling script
- running script
- quits in [ESC]

### Planned
- audio/midi support
- mouse position support
- custom uniforms to tweak on runtime

Shader intro.

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
