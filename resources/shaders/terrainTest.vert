#version 430 core

layout(location = 0) in vec2 position;
out vec2 positionV;

void main() {
    positionV = position;
}