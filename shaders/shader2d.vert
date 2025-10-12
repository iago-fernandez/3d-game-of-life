#version 330 core

out vec2 vUV;

void main() {
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

    // Map NDC [-1,1] -> UV [0,1]
    vUV = ndcPos * 0.5 + 0.5;
}
