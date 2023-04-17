#!/bin/bash
echo "Compiling for smallest size..."
g++ main.cpp -o demo -std=c++11 -lglfw -lGLEW -lGL && upx --best demo && cat demo pass_fragment_shader.glsl pass_vertex_shader.glsl fragment_shader.glsl vertex_shader.glsl > demoLinux
ls -l demoLinux
echo "Done; Run ./demoLinux"
