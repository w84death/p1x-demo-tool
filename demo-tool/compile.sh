#!/bin/bash
echo "Compiling for smallest size..."
echo "Linked libraries: X11 GL asound dl m."
g++ main3.cpp -o demo -Os -lX11 -lGL -lasound -ldl -lm && upx --best demo && echo "Done; Run ./demo --help"
