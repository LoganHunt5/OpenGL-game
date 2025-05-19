#include "glm/ext/matrix_transform.hpp"
#include "shader_lib.hpp"
#include "shoot_lib.hpp"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cstddef>
#include <iostream>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "camera.h"
#include "stb_image.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

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

  Shader Orange("./vertexShaderSource.vs", "./lightingShader.fs");
  Shader LightSourceShader("./lightsourcevertShader.vs",
                           "./lightsourcefragShader.fs");
  // Shader Pink("./vertexShaderSource.txt", "./fragmentShaderSource.txt");
  // 0-2 pos, 3-4 texture coords
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};
  // vertex buffer to send all points to gpu at once and keep them here
  unsigned int VBOs[2], VAOs[2], EBO, lightVAOs[1];
  // assign a buffer ID to gl object
  glGenVertexArrays(2, VAOs);
  glGenBuffers(2, VBOs);
  glGenBuffers(1, &EBO);
  makeVAO(&(VAOs[0]), &(VBOs[0]), NULL, vertices, sizeof(vertices), NULL, 0,
          false);

  glGenVertexArrays(1, lightVAOs);
  glBindVertexArray(lightVAOs[0]);
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // wireframe
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // to go back
  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //

  // texture picture
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(
      true); // tell stb_image.h to flip loaded texture's on the y-axis.
  unsigned char *data = stbi_load("pig.png", &width, &height, &nrChannels, 0);
  unsigned int textures[2];
  glGenTextures(2, textures);
  glBindTexture(GL_TEXTURE_2D, textures[0]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  data = stbi_load("plaid.jpg", &width, &height, &nrChannels, 0);
  glBindTexture(GL_TEXTURE_2D, textures[1]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  // Orange.use();
  // Orange.setInt("texture1", 0);
  // Orange.setInt("texture2", 1);

  // rotate so that the plane is laying on the floor
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::rotate(model, glm::radians((float)glfwGetTime()),
                      glm::vec3(1.0f, 0.0f, 0.0f));

  // move cam back a little(move world forward)
  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

  glm::mat4 projection;
  projection =
      glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  glEnable(GL_DEPTH_TEST);

  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f) //
  };

  view = camera.GetViewMatrix();
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  const float radius = 10.0f;

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // rendering
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    Orange.use();

    Orange.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    Orange.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    Orange.setVec3("lightPos", lightPos);
    Orange.setVec3("viewPos", camera.Position);

    view = camera.GetViewMatrix();
    Orange.setMat4("view", view);

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    Orange.setMat4("projection", projection);

    glBindVertexArray(VAOs[0]);
    for (unsigned int i = 0; i < 1; i++) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i + 10.0f;
      model = glm::rotate(model, glm::radians(angle * (float)glfwGetTime()),
                          glm::vec3(1.0f, 0.3f, 0.5f));
      Orange.setMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    LightSourceShader.use();
    view = camera.GetViewMatrix();
    LightSourceShader.setMat4("view", view);
    LightSourceShader.setMat4("projection", projection);

    // draw light cube
    model = glm::mat4(1.0f);
    lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    lightPos *= glm::vec3(
        sin((float)glfwGetTime()) * 2.0f, cos((float)glfwGetTime()) * 2.0f,
        cos((float)glfwGetTime()) * 3.0f * sin((float)glfwGetTime()));
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    LightSourceShader.setMat4("model", model);

    glBindVertexArray(lightVAOs[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(2, VAOs);
  glDeleteBuffers(2, VBOs);
  glDeleteProgram(Orange.ID);
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
