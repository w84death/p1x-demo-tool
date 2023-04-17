#!/bin/bash
echo "Compiling for smallest size..."
g++ main.cpp -o demo -lglfw -lGLEW -lGL && upx --best demo
echo "Done; Run ./demo"
