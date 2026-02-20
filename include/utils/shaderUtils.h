/**
 * @file shaderUtils.h
 * @brief Utility functions for reading, compiling, and linking GLSL shader programs with error reporting.
 */
#pragma once

#include <glad/glad.h>

namespace utils {

    /**
     * @brief Compile and link a shader program from file paths.
     * @param vsPath Path to Vertex Shader file.
     * @param fsPath Path to Fragment Shader file.
     * @return GL program handle. Throws std::runtime_error on failure.
     */
    GLuint makeProgramFromFiles(const char* vsPath, const char* fsPath);

    /**
     * @brief Compile and link a shader program from source strings.
     * @param vsSource Vertex Shader source code.
     * @param fsSource Fragment Shader source code.
     * @param debugName Name used for error logging.
     * @return GL program handle. Throws std::runtime_error on failure.
     */
    GLuint makeProgramFromSource(const char* vsSource, const char* fsSource, const char* debugName);

}