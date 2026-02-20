/**
 * @file shaderUtils.h
 * @brief Utility functions for reading, compiling, and linking GLSL shader programs with error reporting.
 */
#pragma once

#include <glad/glad.h>

GLuint makeProgramFromFiles(const char* vsPath, const char* fsPath);

GLuint makeProgramFromSource(const char* vsSource, const char* fsSource, const char* debugName);