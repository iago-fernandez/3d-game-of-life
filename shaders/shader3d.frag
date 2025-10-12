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

// Grid lines
float gridLineUV_px(vec2 uv, ivec2 grid, float px){
    vec2 st = uv * vec2(grid);
    vec2 frac = fract(st);
    vec2 dist = min(frac, 1.0 - frac);
    vec2 fw = fwidth(st);
    vec2 halfW = 0.5 * px * fw;

    vec2 a = smoothstep(halfW, halfW + fw, dist);
    float line = 1.0 - min(a.x, a.y);
    return line;
}

// Axis
float axisU_px(vec2 uv, float px){
    float d = min(uv.x, 1.0 - uv.x);
    float fw = fwidth(uv.x);
    float hw = 0.5 * px * fw;
    return 1.0 - smoothstep(hw, hw + fw, d);
}

float axisV_px(vec2 uv, float px){
    float d = min(uv.y, 1.0 - uv.y);
    float fw = fwidth(uv.y);
    float hw = 0.5 * px * fw;
    return 1.0 - smoothstep(hw, hw + fw, d);
}

void main() {

    vec2 grid = vec2(uGridSize);
    vec2 uv01 = fract(vUV);
    vec2 cell = floor(uv01 * grid);
    vec2 center = (cell + vec2(0.5)) / grid;

    float s = texture(uState, center).r;
    float alive = (s > (0.5/255.0)) ? 1.0 : 0.0;
    vec3 baseCol = mix(uDeadColor, uAliveColor, alive);

    float lineMask = (uLinePx > 0.0) ? gridLineUV_px(uv01, uGridSize, uLinePx) : 0.0;
    vec3 color = mix(baseCol, uLineColor, lineMask);

    float uAxis = axisU_px(uv01, uEdgePxUV);
    float vAxis = axisV_px(uv01, uEdgePxUV);
    color = mix(color, uEdgeUColor, uAxis);
    color = mix(color, uEdgeVColor, vAxis);

    FragColor = vec4(color, 1.0);
}
