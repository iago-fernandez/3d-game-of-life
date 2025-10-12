#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform ivec2 uGridSize;
uniform vec2 uViewportPx;
uniform sampler2D uState;

uniform vec3 uDeadColor;
uniform vec3 uAliveColor;

uniform float uLineThicknessPx;
uniform vec3 uLineColor;

uniform ivec2 uHoverCell;     // (-1,-1) if none
uniform float uHoverBoost;

uniform vec3 uEdgeUColor;
uniform vec3 uEdgeVColor;
uniform float uEdgeThicknessPx;

void main() {

    vec2 fragPx = gl_FragCoord.xy;
    vec2 cellPx = uViewportPx / vec2(uGridSize);
    ivec2 cell = ivec2(floor(fragPx / cellPx));
    if (any(lessThan(cell, ivec2(0))) || any(greaterThanEqual(cell, uGridSize))) {
        discard;
    }

    float stateVal = texelFetch(uState, cell, 0).r;
    vec3 baseColor = (stateVal > 0.0) ? uAliveColor : uDeadColor;

    bool isHover = all(equal(cell, uHoverCell));
    if (isHover) {
        baseColor = clamp(baseColor + vec3(uHoverBoost), 0.0, 1.0);
    }

    vec2 cellUV = fract(fragPx / cellPx);
    float distToEdgePx = min(min(cellUV.x, 1.0 - cellUV.x), min(cellUV.y, 1.0 - cellUV.y)) * min(cellPx.x, cellPx.y);
    float aa = 0.75;
    float cellEdgeMask = 1.0 - smoothstep(uLineThicknessPx - aa, uLineThicknessPx + aa, distToEdgePx);

    vec3 color = mix(baseColor, uLineColor, cellEdgeMask);

    float distEdgeX = min(fragPx.x, uViewportPx.x - fragPx.x);
    float distEdgeY = min(fragPx.y, uViewportPx.y - fragPx.y);
    float axisUMask = 1.0 - smoothstep(uEdgeThicknessPx - aa, uEdgeThicknessPx + aa, distEdgeX);
    float axisVMask = 1.0 - smoothstep(uEdgeThicknessPx - aa, uEdgeThicknessPx + aa, distEdgeY);

    color = mix(color, uEdgeUColor, axisUMask);
    color = mix(color, uEdgeVColor, axisVMask);

    FragColor = vec4(color, 1.0);
}
