#!/bin/bash

demo_executable="./demo"
shader_file="shader.glsl"

kill_demo() {
  pkill -f "$demo_executable"
}

watch_shader() {
  inotifywait -m -e close_write,moved_to,create "$shader_file" |
  while read -r directory event filename; do
    echo "Shader file changed, restarting demo..."
    kill_demo
    $demo_executable & disown
  done
}

# Ensure inotify-tools is installed
if ! command -v inotifywait >/dev/null 2>&1; then
  echo "Error: inotifywait command not found. Please install inotify-tools."
  exit 1
fi

# Start the initial instance of demo
$demo_executable & disown

# Start watching the shader file
watch_shader
