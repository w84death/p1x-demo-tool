#!/bin/bash
echo "Compiling for smallest size..."
g++ main.cpp -o intro -lSDL2 -lGLEW -lGL -Os -s -flto
echo "Done; Run ./intro"
