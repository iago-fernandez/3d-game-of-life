#include "../../include/utils/shaderUtils.h"

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

static std::string readTextFile(const char* path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error(std::string("Cannot open: ") + path);
    std::ostringstream ss; ss << f.rdbuf();
    return ss.str();
}

static void checkShader(GLuint s, const char* dbg) {
    GLint ok = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        std::string log(len > 0 ? (size_t)len : 1, '\0');
        glGetShaderInfoLog(s, len, nullptr, log.data());
        throw std::runtime_error(std::string("Shader compile error [") + dbg + "]:\n" + log);
    }
}
static void checkProgram(GLuint p, const char* dbg) {
    GLint ok = 0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        std::string log(len > 0 ? (size_t)len : 1, '\0');
        glGetProgramInfoLog(p, len, nullptr, log.data());
        throw std::runtime_error(std::string("Program link error [") + dbg + "]:\n" + log);
    }
}

static GLuint compileShaderFromSource(GLenum type, const char* src, const char* dbg) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    checkShader(s, dbg);
    return s;
}
static GLuint compileShaderFromFile(GLenum type, const char* path) {
    std::string src = readTextFile(path);
    return compileShaderFromSource(type, src.c_str(), path);
}

static GLuint linkProgram(GLuint vs, GLuint fs, const char* dbg) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    checkProgram(prog, dbg);
    glDetachShader(prog, vs);
    glDetachShader(prog, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

GLuint makeProgramFromFiles(const char* vsPath, const char* fsPath) {
    GLuint vs = compileShaderFromFile(GL_VERTEX_SHADER, vsPath);
    GLuint fs = compileShaderFromFile(GL_FRAGMENT_SHADER, fsPath);
    return linkProgram(vs, fs, "vs+fs");
}

GLuint makeProgramFromSource(const char* vsSource, const char* fsSource, const char* debugName) {
    GLuint vs = compileShaderFromSource(GL_VERTEX_SHADER, vsSource, "vs-src");
    GLuint fs = compileShaderFromSource(GL_FRAGMENT_SHADER, fsSource, "fs-src");
    return linkProgram(vs, fs, debugName ? debugName : "src+src");
}
