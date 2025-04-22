#include "shader_lib.h"
#include "shoot_lib.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
  if (!glfwInit()) {
    return -1;
  }
  // pick option, then set value
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "Shoot", NULL, NULL);
  if (window == NULL) {
    puts("could not create window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // load all the glad function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    puts("could not init glad");
    return -1;
  }
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  Shader Orange("./vertexShaderSource.txt", "./fragmentShaderSource.txt");
  // Shader Pink("./vertexShaderSource.txt", "./fragmentShaderSource.txt");

  // 0-2 pos, 3-5 color, 6-8 texture coords
  float tri1Vertices[] = {
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f  // top left
  };
  unsigned int tri1Indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3, // second triangle
  };
  /*
  float tri2Verticies[] = {
      1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // left
      0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top
      0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f  // bot
  };
*/
  // vertex buffer to send all points to gpu at once and keep them here
  unsigned int VBOs[2], VAOs[2], EBO;
  // assign a buffer ID to gl object
  glGenVertexArrays(2, VAOs);
  glGenBuffers(2, VBOs);
  glGenBuffers(1, &EBO);
  makeVAO(&(VAOs[0]), &(VBOs[0]), &EBO, tri1Vertices, sizeof(tri1Vertices),
          tri1Indices, sizeof(tri1Indices), true);

  // makeVAO(&(VAOs[1]), &(VBOs[1]), NULL, tri2Verticies, sizeof(tri2Verticies),
  //         NULL, 0, false);

  // wireframe
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // to go back
  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //

  // texture picture
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("smileyjpg.jpg", &width, &height, &nrChannels, 0);
  unsigned int textures;
  glGenTextures(1, &textures);
  glBindTexture(GL_TEXTURE_2D, textures);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  float timeValue;
  float xOffValue, yOffValue;
  int vHOffLocation = glGetUniformLocation(Orange.ID, "xyOffset");
  glUseProgram(Orange.ID);
  glUniform3f(vHOffLocation, 0.0f, 0.0f, 0.0f);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    timeValue = glfwGetTime();
    xOffValue = (sin(timeValue) / 2.0f);
    yOffValue = (cos(timeValue) / 2.0f);
    glUniform3f(vHOffLocation, xOffValue, yOffValue, 0.0f);
    glBindTexture(GL_TEXTURE_2D, textures);
    renderSteps(&Orange, &(VAOs[0]), 6, true);
    // renderSteps(&Orange, &(VAOs[1]), 3, false);

    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(2, VAOs);
  glDeleteBuffers(2, VBOs);
  glDeleteProgram(Orange.ID);
  // glDeleteProgram(Pink.ID);
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void makeVAO(unsigned int *VAO, unsigned int *VBO, unsigned int *EBO,
             float *points, int pointsSize, unsigned int *indices,
             int indicesSize, bool useEBO) {

  glBindVertexArray(*VAO);
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  // VBO configuration
  /*
   * GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few
   * times. GL_STATIC_DRAW: the data is set only once and used many times.
   * GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
   */
  glBufferData(GL_ARRAY_BUFFER, pointsSize, points, GL_STATIC_DRAW);
  if (useEBO) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
  }
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void renderSteps(Shader *shaderProgram, unsigned int *VAO, int count,
                 bool useEBO) {
  shaderProgram->use();
  glBindVertexArray(*VAO);
  if (useEBO) {
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, count);
  }
}
