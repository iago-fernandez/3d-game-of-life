/**
 * @file shader3d.frag
 * @brief Fragment shader for the 3D surface. Computes UV-derivative-based anti-aliased grid lines and topology seams.
 */
#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uState;
uniform ivec2 uGridSize;

uniform vec3 uDeadColor;
uniform vec3 uAliveColor;

uniform float uLinePx;
uniform vec3 uLineColor;

uniform vec3 uEdgeUColor;
uniform vec3 uEdgeVColor;
uniform float uEdgePxUV;

// Helper: Procedural grid lines with constant pixel width based on UV derivatives
float gridLineUV_px(vec2 uv, ivec2 grid, float px) {
    vec2 st = uv * vec2(grid);
    vec2 frac = fract(st);
    vec2 dist = min(frac, 1.0 - frac);
    vec2 fw = fwidth(st);
    vec2 halfW = vec2(0.5 * px) * fw;

    vec2 a = smoothstep(halfW, halfW + fw, dist);
    return 1.0 - min(a.x, a.y);
}

// Helper: Axis highlight logic
float axis_px(float u, float px) {
    float dist = min(u, 1.0 - u);
    float fw = fwidth(u);
    float halfW = 0.5 * px * fw;
    return 1.0 - smoothstep(halfW, halfW + fw, dist);
}

void main() {
    // Sample Simulation State
    // We sample the center of the cell to avoid texture interpolation artifacts
    vec2 grid = vec2(uGridSize);
    vec2 uv01 = fract(vUV); // Wrap UVs for safety
    vec2 cell = floor(uv01 * grid);
    vec2 center = (cell + vec2(0.5)) / grid;

    float s = texture(uState, center).r;
    // Check threshold (GL_R8 is normalized [0,1])
    float alive = (s > (0.5 / 255.0)) ? 1.0 : 0.0;
    
    vec3 baseCol = mix(uDeadColor, uAliveColor, alive);

    // Draw Grid Lines
    float lineMask = 0.0;
    if (uLinePx > 0.0) {
        lineMask = gridLineUV_px(uv01, uGridSize, uLinePx);
    }
    vec3 color = mix(baseCol, uLineColor, lineMask);

    // 3. Draw Topology Edges (Seams)
    float uAxisMask = axis_px(uv01.x, uEdgePxUV);
    float vAxisMask = axis_px(uv01.y, uEdgePxUV);

    color = mix(color, uEdgeUColor, uAxisMask);
    color = mix(color, uEdgeVColor, vAxisMask);

    FragColor = vec4(color, 1.0);
}