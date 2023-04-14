#!/bin/bash
echo "Compiling for smallest size..."
g++ main.cpp -o demo -lSDL2 -lGLEW -lGL -lSDL2_mixer && upx --best demo
echo "Done; Run ./demo"
