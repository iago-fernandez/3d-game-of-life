/**
 * @file shader3d.vert
 * @brief Vertex shader for the 3D toroidal mesh, applying the Model-View-Projection matrix.
 */
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aUV;

uniform mat4 uMVP;

out vec2 vUV;

void main() {
    vUV = aUV;
    gl_Position = uMVP * vec4(aPos, 1.0);
}