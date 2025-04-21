#include <cstdlib>
#include <glad/glad.h>
#include <iostream>

#include <GLFW/glfw3.h>
#include <stdio.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void makeShaderProgram(unsigned int *shaderProgram,
                       const char *vertexShaderSource,
                       const char *fragmentShaderSource);

void makeVAO(unsigned int *VAO, unsigned int *VBO, unsigned int *EBO,
             unsigned int *shaderProgram, float *points, int pointsSize,
             unsigned int *indices, int indicesSize, bool useEBO);

void renderSteps(unsigned int *shaderProgram, unsigned int *VAO, int count,
                 bool useEBO);

void vertexShader(float *points);

void shapeAssembly();

void geometryShader();

void rasterizer();

void fragShader();

void aTestBlending();
