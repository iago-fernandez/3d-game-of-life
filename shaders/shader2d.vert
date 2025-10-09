#version 330 core

// Fullscreen quad generated from gl_VertexID (no VBO needed).
// 6 vertices: two triangles covering NDC [-1,1]^2.

out vec2 vUV; // UV in [0,1] across the screen

void main() {
    // Two-triangle quad (CCW)
    const vec2 positions[6] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 1.0, -1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0, -1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0,  1.0)
    );

    vec2 pos = positions[gl_VertexID];
    gl_Position = vec4(pos, 0.0, 1.0);

    // Map NDC [-1,1] -> UV [0,1]
    vUV = pos * 0.5 + 0.5;
}
