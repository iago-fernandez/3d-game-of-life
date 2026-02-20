/**
 * @file shader2d.frag
 * @brief Fragment shader for the 2D view. Handles cell state fetching, hover effects, and pixel-perfect grid lines.
 */
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

uniform ivec2 uHoverCell;
uniform float uHoverBoost;

uniform vec3 uEdgeUColor;
uniform vec3 uEdgeVColor;
uniform float uEdgeThicknessPx;

void main() {
    // Calculate cell coordinates based on fragment position
    vec2 fragPx = gl_FragCoord.xy;
    vec2 cellPx = uViewportPx / vec2(uGridSize);
    ivec2 cell = ivec2(floor(fragPx / cellPx));

    // Discard fragments outside the logical grid (safety check)
    if (any(lessThan(cell, ivec2(0))) || any(greaterThanEqual(cell, uGridSize))) {
        discard;
    }

    // Retrieve cell state (0 = dead, 1 = alive)
    float stateVal = texelFetch(uState, cell, 0).r;
    vec3 baseColor = (stateVal > 0.0) ? uAliveColor : uDeadColor;

    // Apply hover effect
    if (all(equal(cell, uHoverCell))) {
        baseColor = clamp(baseColor + vec3(uHoverBoost), 0.0, 1.0);
    }

    // Grid lines calculation (Axis-aligned anti-aliasing)
    // Calculate distance to the nearest grid line in pixels
    vec2 posInCell = fract(fragPx / cellPx);
    vec2 distToLine = min(posInCell, 1.0 - posInCell) * cellPx;
    
    // Calculate AA width using derivatives
    vec2 aaWidth = max(fwidth(fragPx), vec2(0.5));
    vec2 lineMaskVec = 1.0 - smoothstep(uLineThicknessPx - aaWidth, uLineThicknessPx + aaWidth, distToLine);
    float lineMask = max(lineMaskVec.x, lineMaskVec.y);

    vec3 color = mix(baseColor, uLineColor, lineMask);

    // Viewport border calculation
    vec2 distToEdge = min(fragPx, uViewportPx - fragPx);
    vec2 aaEdge = fwidth(distToEdge);
    
    float axisUMask = 1.0 - smoothstep(uEdgeThicknessPx - aaEdge.x, uEdgeThicknessPx + aaEdge.x, distToEdge.x);
    float axisVMask = 1.0 - smoothstep(uEdgeThicknessPx - aaEdge.y, uEdgeThicknessPx + aaEdge.y, distToEdge.y);

    color = mix(color, uEdgeUColor, axisUMask);
    color = mix(color, uEdgeVColor, axisVMask);

    FragColor = vec4(color, 1.0);
}