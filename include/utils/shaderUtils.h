#pragma once
#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

	GLuint makeProgramFromFiles(const char* vsPath, const char* fsPath);

	GLuint makeProgramFromSource(const char* vsSource, const char* fsSource, const char* debugName);

#ifdef __cplusplus
}
#endif
