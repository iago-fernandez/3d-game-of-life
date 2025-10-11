#pragma once

#include <glad/glad.h>

struct TorusMesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;
};

TorusMesh makeTorusGrid(int columns, int rows, float outerRadius, float innerRadius);

void destroyTorus(TorusMesh& t);
