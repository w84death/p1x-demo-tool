#!/bin/bash
echo "Compiling for smallest size..."
echo "Linked libraries: X11 GLEW GL asound dl m."
g++ main.cpp -o demo -O2 -lX11 -lGLEW -lGL -lasound -ldl -lm && upx --best demo && echo "Done; Run ./demo --help"
