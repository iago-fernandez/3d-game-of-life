#version 330 core

in vec2 vUV;

uniform sampler2D uState;
uniform vec3 uDeadColor;
uniform vec3 uAliveColor;
uniform ivec2 uGridSize;
uniform float uLinePx;
uniform vec3 uLineColor;
uniform vec3 uEdgeUColor;
uniform vec3 uEdgeVColor;
uniform float uEdgePxUV;

out vec4 FragColor;

float gridLineUV(vec2 uv, ivec2 grid, float px){
    vec2 g = fract(uv * vec2(grid));
    vec2 edge = min(g, 1.0 - g);
    float lx = step(edge.x, px);
    float ly = step(edge.y, px);
    return clamp(max(lx, ly), 0.0, 1.0);
}

void main() {

    vec2 grid = vec2(uGridSize);
    vec2 uv01 = fract(vUV);
    vec2 cell = floor(uv01 * grid);
    vec2 centerUV = (cell + vec2(0.5)) / grid;  // one texel per face

    float s = texture(uState, centerUV).r;
    const float thresh = 0.5 / 255.0;
    float alive = s > thresh ? 1.0 : 0.0;

    vec3 base = mix(uDeadColor, uAliveColor, alive);

    float lineMask = (uLinePx > 0.0) ? gridLineUV(uv01, uGridSize, uLinePx) : 0.0;
    vec3 color = mix(base, uLineColor, lineMask);

    float axisU = max(step(uv01.x, uEdgePxUV), step(1.0 - uv01.x, uEdgePxUV));
    float axisV = max(step(uv01.y, uEdgePxUV), step(1.0 - uv01.y, uEdgePxUV));
    color = mix(color, uEdgeUColor, axisU);
    color = mix(color, uEdgeVColor, axisV);

    FragColor = vec4(color, 1.0);
}
