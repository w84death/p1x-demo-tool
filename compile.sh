#!/bin/bash
echo "Compiling for smallest size..."
g++ main.cpp -o demo -std=c++11 -lglfw -lGLEW -lGL && upx --best demo && echo "Done; Run ./demo --stats"
