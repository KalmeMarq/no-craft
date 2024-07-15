#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Application.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int, char**)
{
    KM::Application app;
    app.Run();
}