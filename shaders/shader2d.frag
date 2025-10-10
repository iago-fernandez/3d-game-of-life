#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform ivec2 gridSize; 
uniform vec2  viewportPx;

// Per-cell state texture: GL_R8, one texel per cell (0 or 1)
uniform sampler2D state;

uniform vec3 deadColor;
uniform vec3 aliveColor;

uniform float lineThicknessPx;
uniform vec3  lineColor;

// Hover highlight ((-1,-1) if none)
uniform ivec2 hoverCell;
uniform float hoverBoost;

void main() {

    // Current cell from screen-space position
    vec2 p = gl_FragCoord.xy;
    vec2 cellPx = viewportPx / vec2(gridSize);
    ivec2 cell = ivec2(floor(p / cellPx));
    if (any(lessThan(cell, ivec2(0))) || any(greaterThanEqual(cell, gridSize))) {
        discard;
    }

    // Cell state
    float s = texelFetch(state, cell, 0).r;
    vec3 base = (s > 0.0) ? aliveColor : deadColor; // any non-zero as alive

    // Hover brighten
    bool isHover = all(equal(cell, hoverCell));
    if (isHover) {
        base = clamp(base + vec3(hoverBoost), 0.0, 1.0);
    }

    // Screen-space border with constant pixel thickness
    vec2 uv = fract(p / cellPx);
    float edgePx = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y)) * min(cellPx.x, cellPx.y);
    float aa = 0.75;
    float edgeMask = 1.0 - smoothstep(lineThicknessPx - aa, lineThicknessPx + aa, edgePx);

    vec3 col = mix(base, lineColor, edgeMask);
    FragColor = vec4(col, 1.0);
}
