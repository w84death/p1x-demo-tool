#!/bin/bash
echo "Compiling for smallest size..."
g++ main.cpp -o intro -lSDL2 -lGLEW -lGL -lSDL2_mixer && upx --best intro
echo "Done; Run ./intro"
