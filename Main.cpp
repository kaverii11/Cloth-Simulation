#include "Cloth.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

const char *vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 MVP;
    void main() {
        gl_Position = MVP * vec4(aPos, 1.0);
    }
)glsl";

const char *fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 color;
    void main() {
        FragColor = vec4(color, 1.0);
    }
)glsl";

int main() {
  if (!glfwInit())
    return -1;

  // --- MACOS SPECIFIC CORE PROFILE CONFIG ---
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS

  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                        "Cloth Sim - Core Profile", NULL, NULL);
  if (!window) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE; // Needed for Core Profile
  if (glewInit() != GLEW_OK)
    return -1;

  // Shaders setup
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // Get Uniform Locations
  int mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
  int colorLoc = glGetUniformLocation(shaderProgram, "color");

  // ImGui Setup
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");

  Cloth cloth;
  cloth.InitMesh();

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Physics
    cloth.Simulate(0.01f);

    // UI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Settings");
    ImGui::SliderFloat("Wind Strength", (float *)&cloth.wind_force.x(), -5.0f,
                       5.0f);
    ImGui::End();

    // Render
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrices
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 10, 50), glm::vec3(0, 0, 0),
                                 glm::vec3(0, 1, 0));
    glm::mat4 mvp = projection * view;

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    // Draw Cloth
    glUniform3f(colorLoc, 0.8f, 0.8f, 0.8f);
    // Draw Cloth
    glUniform3f(colorLoc, 0.8f, 0.8f, 0.8f);
    cloth.Draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}