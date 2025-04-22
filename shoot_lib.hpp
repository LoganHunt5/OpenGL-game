#ifndef SHOOT_LIB
#define SHOOT_LIB

#include "shader_lib.hpp"
#include <cmath>
#include <glad/glad.h>

#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void makeShaderProgram(unsigned int *shaderProgram,
                       const char *vertexShaderSource,
                       const char *fragmentShaderSource);

void makeVAO(unsigned int *VAO, unsigned int *VBO, unsigned int *EBO,
             float *points, int pointsSize, unsigned int *indices,
             int indicesSize, bool useEBO);

void renderSteps(Shader *shaderProgram, unsigned int *VAO, int count,
                 bool useEBO);

void vertexShader(float *points);

#endif
