#include "shoot_lib.h"
#include <GLFW/glfw3.h>

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    " gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragment1ShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

const char *fragment2ShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = vec4(0.961f, 0.698f, 0.961f, 1.0f);\n"
    "}\0";

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

  unsigned int shaderProgramOrange;
  makeShaderProgram(&shaderProgramOrange, vertexShaderSource,
                    fragment1ShaderSource);
  unsigned int shaderProgramPink;
  makeShaderProgram(&shaderProgramPink, vertexShaderSource,
                    fragment2ShaderSource);

  // f forces a float, floats have less precision, and 4 vs 8 bytes size
  float tri1Vertices[] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, // top left
  };
  unsigned int tri1Indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3, // second triangle
  };
  float tri2Verticies[] = {
      1.0f, 0.0f,  0.0f, // left
      0.5f, 1.0f,  0.0f, // top
      0.5f, -1.0f, 0.0f  // bot
  };

  // vertex buffer to send all points to gpu at once and keep them here
  unsigned int VBOs[2], VAOs[2], EBO;
  // assign a buffer ID to gl object
  glGenVertexArrays(2, VAOs);
  glGenBuffers(2, VBOs);
  glGenBuffers(1, &EBO);
  makeVAO(&(VAOs[0]), &(VBOs[0]), &EBO, &shaderProgramOrange, tri1Vertices,
          sizeof(tri1Vertices), tri1Indices, sizeof(tri1Indices), true);

  makeVAO(&(VAOs[1]), &(VBOs[1]), NULL, &shaderProgramPink, tri2Verticies,
          sizeof(tri2Verticies), NULL, 0, false);

  /*
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // VBO configuration
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  */

  // wireframe
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // to go back
  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderSteps(&shaderProgramOrange, &(VAOs[0]), 6, true);
    renderSteps(&shaderProgramPink, &(VAOs[1]), 3, false);

    // glUseProgram(altShaderProgram);
    // glBindVertexArray(VAOs[1]);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(2, VAOs);
  glDeleteBuffers(2, VBOs);
  glDeleteProgram(shaderProgramOrange);
  glDeleteProgram(shaderProgramPink);
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void makeShaderProgram(unsigned int *shaderProgram, const char *vertexSource,
                       const char *fragmentSource) {
  unsigned int vertShader;
  vertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertShader, 1, &vertexSource, NULL);
  glCompileShader(vertShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  *shaderProgram = glCreateProgram();
  glAttachShader(*shaderProgram, vertShader);
  glAttachShader(*shaderProgram, fragmentShader);
  glLinkProgram(*shaderProgram);
  glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  glDeleteShader(vertShader);
  glDeleteShader(fragmentShader);
}

void makeVAO(unsigned int *VAO, unsigned int *VBO, unsigned int *EBO,
             unsigned int *shaderProgram, float *points, int pointsSize,
             unsigned int *indices, int indicesSize, bool useEBO) {

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
  glEnableVertexAttribArray(0);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void renderSteps(unsigned int *shaderProgram, unsigned int *VAO, int count,
                 bool useEBO) {
  glUseProgram(*shaderProgram);
  glBindVertexArray(*VAO);
  if (useEBO) {
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, count);
  }
}
