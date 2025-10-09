#version 330 core

in vec2 vUV;
out vec4 FragColor;

// Number of cells horizontally/vertically (e.g., 10,10)
uniform ivec2 gridSize;

// Colors for the two states (e.g., light/dark)
uniform vec3 deadColor;
uniform vec3 aliveColor;

void main() {
    // Current cell
    vec2 gridUV = vUV * vec2(gridSize);
    ivec2 cell = ivec2(floor(gridUV));

    // Checker pattern by parity (x+y)
    bool isAlive = ((cell.x + cell.y) & 1) == 1;
    vec3 col = isAlive ? aliveColor : deadColor;

    FragColor = vec4(col, 1.0);
}
