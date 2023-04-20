#!/bin/bash
echo "Compiling for smallest size..."
g++ main.cpp -o demo -lX11 -lGLEW -lGL -lmikmod && upx --best demo && echo "Done; Run ./demo --stats"