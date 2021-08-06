#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>

#include "AccelerationStructures/grid.h"
#include "DataStructures/hybrid_vector.h"
#include "Simulation/simulation.h"
#include "Renderer/renderer.h"

uint32_t w = 2048;
uint32_t h = 1024;
uint32_t n = 64000;
AccelerationStructureType type = AccelerationStructureType::Grid;

Simulation simulation;
Renderer* p_renderer;

bool key_pressed_up    = false;
bool key_pressed_down  = false;
bool key_pressed_left  = false;
bool key_pressed_right = false;

double prev_mouse_x = 0.0f;
double prev_mouse_y = 0.0f;

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (simulation.paused()) simulation.unpause();
        else simulation.pause();
    }
    if (key == GLFW_KEY_UP) {
        key_pressed_up = action;
    }
    if (key == GLFW_KEY_DOWN) {
        key_pressed_down = action;
    }
    if (key == GLFW_KEY_LEFT) {
        key_pressed_left = action;
    }
    if (key == GLFW_KEY_RIGHT) {
        key_pressed_right = action;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &prev_mouse_x, &prev_mouse_y);
    }
}


void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS) {
        double delta_x = xpos - prev_mouse_x;
        double delta_y = ypos - prev_mouse_y;

        p_renderer->translate(-delta_x, delta_y);

        prev_mouse_x = xpos;
        prev_mouse_y = ypos;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    p_renderer->zoom(yoffset);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    p_renderer->resize(width, height);
}


void handle_input(Renderer &renderer)
{
    if (key_pressed_up)
        renderer.translate(0, 1);
    if (key_pressed_down)
        renderer.translate(0, -1);
    if (key_pressed_right)
        renderer.translate(1, 0);
    if (key_pressed_left)
        renderer.translate(-1, 0);
}

static uint32_t num_fps_frames = 0;
void showFPS(GLFWwindow *p_window, double &last_time)
{
    double currentTime = glfwGetTime();
    double delta = currentTime - last_time;
    num_fps_frames++;

    if ( delta >= 1.0 ) {
        double fps = double(num_fps_frames) / delta;

        std::stringstream ss;
        ss << "Collision demo" << " [" << fps << " FPS]";

        glfwSetWindowTitle(p_window, ss.str().c_str());

        num_fps_frames = 0;
        last_time = currentTime;
    }
}

void usage()
{
    printf("Usage:\n");
    printf("    collision [<arguments>]\n");
    printf("        arguments:\n");
    printf("            -h         : display usage information\n");
    printf("            -grid      : run using a grid acceleration structure (default)\n");
    printf("            -quad      : run with a quadtree acceleration structure\n");
    printf("            -n <N>     : set the number of particles (default 64000)\n");
}

void parseArguments(int argc, char** argv)
{
    int k = 1;
    while (k < argc) {
        if (std::string("-h").compare(argv[k]) == 0) {
            usage();
            exit(0);
        } else if (std::string("-grid").compare(argv[k]) == 0) {
            type = AccelerationStructureType::Grid;
        } else if (std::string("-quad").compare(argv[k]) == 0) {
            type = AccelerationStructureType::Quadtree;
        } else if (std::string("-n").compare(argv[k]) == 0) {
            n = atoi(argv[++k]);
        }
        k++;
    }
}

int main(int argc, char** argv)
{
    parseArguments(argc, argv);

    uint8_t frame = 0;
    if (!glfwInit()) { exit(EXIT_FAILURE); }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Collision demo", NULL, NULL);
    if (!window) { exit(EXIT_FAILURE); }

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetErrorCallback(error_callback);
    glfwMakeContextCurrent(window);

    simulation.setup(w, h, n, type);
    Renderer renderer(simulation.entities(), 640, 480);
    p_renderer = &renderer;

    double previous_time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        handle_input(renderer);
        simulation.step();
        renderer.draw();
        showFPS(window, previous_time);

        glfwSwapBuffers(window);
        glfwPollEvents();
        frame++;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}