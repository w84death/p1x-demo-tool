R"(
#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
out vec2 TexCoords;
uniform float width;
uniform float height;
void main() {
    gl_Position = vec4(position, 1.0);
    float aspectRatio = width / height;
    vec2 centeredTexCoords = (texCoords - 0.5) * 2.0;
    TexCoords = vec2(centeredTexCoords.x * aspectRatio, centeredTexCoords.y);
})";
