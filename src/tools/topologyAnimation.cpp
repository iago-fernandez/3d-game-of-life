/**
 * Auxiliary tool to generate the topological transformation animation.
 * This file is intended to be used as a standalone executable to render
 * the asset "topology.gif" used in the project documentation/assets.
 * It does not share state with the main application but replicates its
 * visual style for consistency.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cmath>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 960;
const int GRID_COLS = 50;
const int GRID_ROWS = 50;

// Vertex Shader
// Handles the continuous transformation from a 2D plane to a 3D torus.
// Phase 1: Rolls the plane into a cylinder.
// Phase 2: Bends the cylinder into a torus.
const char* vertexShaderSource = R"(
    #version 330 core
    layout(location=0) in vec3 aPos; 
    layout(location=1) in vec2 aUV;  

    uniform mat4 uMVP;
    uniform float uTime; 

    out vec2 vUV; 

    const float INITIAL_SIZE = 4.0;
    const float R_MAJOR = 2.0; 
    const float R_MINOR = 0.7; 
    const float PI = 3.14159265359;

    void main() {
        vUV = aUV; 
        
        float u = aUV.x - 0.5;
        float v = aUV.y - 0.5;

        vec3 pos;
        
        float targetLenTube = 2.0 * PI * R_MINOR;
        float targetLenRing = 2.0 * PI * R_MAJOR;

        if (uTime <= 1.0) {
            float t = uTime; 
            
            float h = mix(INITIAL_SIZE, targetLenTube, t);
            float w = INITIAL_SIZE; 
            
            float x_flat = u * w;
            
            if (t < 0.001) {
                pos = vec3(x_flat, v * h, 0.0);
            } else {
                float radius = h / (2.0 * PI * t);
                float angle = v * 2.0 * PI * t;
                
                pos.x = x_flat;
                pos.y = radius * sin(angle);
                pos.z = radius * (cos(angle) - 1.0); 
            }
        } 
        else {
            float t = uTime - 1.0; 
            
            float angleV = v * 2.0 * PI;
            float rTube = R_MINOR; 
            
            float tubeY = rTube * sin(angleV);
            float tubeZ = rTube * (cos(angleV) - 1.0);
            
            float w = mix(INITIAL_SIZE, targetLenRing, t);
            
            if (t < 0.001) {
                pos = vec3(u * w, tubeY, tubeZ);
            } else {
                float R_bend = w / (2.0 * PI * t);
                float angleU = u * 2.0 * PI * t;
                
                float rho = R_bend + tubeZ;
                
                pos.x = rho * sin(angleU);
                pos.z = rho * cos(angleU) - R_bend; 
                pos.y = tubeY; 
            }
        }

        gl_Position = uMVP * vec4(pos, 1.0);
    }
)";

// Fragment Shader
// Procedurally generates the grid lines and borders to match the main app style.
const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 vUV;
    out vec4 FragColor;

    uniform ivec2 uGridSize;
    uniform vec3 uDeadColor;
    uniform vec3 uAliveColor;
    uniform float uLinePx;
    uniform vec3 uLineColor;
    uniform vec3 uEdgeUColor;
    uniform vec3 uEdgeVColor;
    uniform float uEdgePxUV;

    float gridLineUV_px(vec2 uv, ivec2 grid, float px){
        vec2 st = uv * vec2(grid);
        vec2 frac = fract(st);
        vec2 dist = min(frac, 1.0 - frac);
        vec2 fw = fwidth(st);
        vec2 halfW = 0.5 * px * fw;
        vec2 a = smoothstep(halfW, halfW + fw, dist);
        return 1.0 - min(a.x, a.y);
    }

    float axisU_px(vec2 uv, float px){
        float d = min(uv.x, 1.0 - uv.x);
        float fw = fwidth(uv.x);
        float hw = 0.5 * px * fw;
        return 1.0 - smoothstep(hw, hw + fw, d);
    }

    float axisV_px(vec2 uv, float px){
        float d = min(uv.y, 1.0 - uv.y);
        float fw = fwidth(uv.y);
        float hw = 0.5 * px * fw;
        return 1.0 - smoothstep(hw, hw + fw, d);
    }

    void main() {
        vec2 uv01 = fract(vUV);
        vec3 baseCol = uDeadColor; 

        float lineMask = (uLinePx > 0.0) ? gridLineUV_px(uv01, uGridSize, uLinePx) : 0.0;
        vec3 color = mix(baseCol, uLineColor, lineMask);

        float uAxis = axisU_px(uv01, uEdgePxUV);
        float vAxis = axisV_px(uv01, uEdgePxUV);
        
        color = mix(color, uEdgeUColor, uAxis);
        color = mix(color, uEdgeVColor, vAxis);

        FragColor = vec4(color, 1.0);
    }
)";

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Topology Animation Generator", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, nullptr);
    glCompileShader(vs);

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fs);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    const int MESH_RES = 100;

    for (int j = 0; j <= MESH_RES; ++j) {
        for (int i = 0; i <= MESH_RES; ++i) {
            float u = (float)i / (float)MESH_RES;
            float v = (float)j / (float)MESH_RES;

            // Dummy position, geometry is calculated in VS
            vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
            vertices.push_back(u); vertices.push_back(v);
        }
    }

    for (int j = 0; j < MESH_RES; ++j) {
        for (int i = 0; i < MESH_RES; ++i) {
            int row1 = j * (MESH_RES + 1);
            int row2 = (j + 1) * (MESH_RES + 1);

            indices.push_back(row1 + i);
            indices.push_back(row1 + i + 1);
            indices.push_back(row2 + i + 1);

            indices.push_back(row1 + i);
            indices.push_back(row2 + i + 1);
            indices.push_back(row2 + i);
        }
    }

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(program);
    glUniform2i(glGetUniformLocation(program, "uGridSize"), GRID_COLS, GRID_ROWS);
    glUniform3f(glGetUniformLocation(program, "uDeadColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(program, "uAliveColor"), 0.0f, 0.0f, 0.0f);
    glUniform1f(glGetUniformLocation(program, "uLinePx"), 0.7f);
    glUniform3f(glGetUniformLocation(program, "uLineColor"), 0.75f, 0.75f, 0.75f);
    glUniform3f(glGetUniformLocation(program, "uEdgeUColor"), 0.30f, 0.50f, 1.00f);
    glUniform3f(glGetUniformLocation(program, "uEdgeVColor"), 1.00f, 0.35f, 0.35f);
    glUniform1f(glGetUniformLocation(program, "uEdgePxUV"), 2.0f);

    // Camera setup: Closer and slightly tilted/elevated for better perspective
    glm::mat4 proj = glm::perspective(glm::radians(28.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(3.0f, 5.5f, 8.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    while (!glfwWindowShouldClose(window)) {
        double time = glfwGetTime();

        // 6 second loop: 4s animation + 2s pause
        float t = (float)fmod(time, 6.0);
        float animVal = (t > 4.0f) ? 2.0f : (t / 4.0f) * 2.0f;

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 mvp = proj * view * model;

        glUniformMatrix4fv(glGetUniformLocation(program, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
        glUniform1f(glGetUniformLocation(program, "uTime"), animVal);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}