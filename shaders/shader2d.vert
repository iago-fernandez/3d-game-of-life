/**
 * @file shader2d.vert
 * @brief Vertex shader for the 2D grid view. Procedurally generates a fullscreen quad.
 */
#version 330 core

out vec2 vUV;

void main() {
    // Generate a full-screen quad using a hardcoded array of vertices
    const vec2 kQuad[6] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 1.0, -1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0, -1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0,  1.0)
    );

    vec2 ndcPos = kQuad[gl_VertexID];
    gl_Position = vec4(ndcPos, 0.0, 1.0);

    // Map NDC [-1, 1] to UV [0, 1]
    vUV = ndcPos * 0.5 + 0.5;
}