/*
* Alexandra Ispas
* 336CA
*/

#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Lab1::Lab1()
{
    // TODO(student): Never forget to initialize class variables!
    red = 0;
    green = 0;
    blue = 0;
    z_coord = 3;
    y_coord = 1;
    x_coord = 1.5;
    object = "";
}


Lab1::~Lab1(){}


void Lab1::Init()
{
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // TODO(student): Load some more meshes. The value of RESOURCE_PATH::MODELS
    // is actually a path on disk, go there and you will find more meshes.
    {
        Mesh* mesh1 = new Mesh("sphere");
        mesh1->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh1->GetMeshID()] = mesh1;
    }

    {
        Mesh* mesh2 = new Mesh("quad");
        mesh2->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        meshes[mesh2->GetMeshID()] = mesh2;
    }

    {
        Mesh* mesh3 = new Mesh("teapot");
        mesh3->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
        meshes[mesh3->GetMeshID()] = mesh3;
    }
}


void Lab1::FrameStart(){}


void Lab1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffer
    glClearColor(red, green, blue, 1);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    // Render the object
    RenderMesh(meshes["box"], glm::vec3(0, 0, 0), glm::vec3(0.5f));

    // Render the object again but with different properties
    RenderMesh(meshes["box"], glm::vec3(-1, 0.5f, 0));

    // TODO(student): We need to render (a.k.a. draw) the mesh that
    // was previously loaded. We do this using `RenderMesh`. Check the
    // signature of this function to see the meaning of its parameters.
    // You can draw the same mesh any number of times.

    RenderMesh(meshes["sphere"], glm::vec3(1, 0.5f, 2), glm::vec3(0.5f));
    RenderMesh(meshes["sphere"], glm::vec3(-1, 0.5f, 2));

    RenderMesh(meshes[object], glm::vec3(x_coord, y_coord, z_coord), glm::vec3(0.3f));
}


void Lab1::FrameEnd() {
    DrawCoordinateSystem();
}

void Lab1::OnInputUpdate(float deltaTime, int mods) {
    // Treat continuous update based on input

    // TODO(student): Add some key hold events that will let you move
    // a mesh instance on all three axes. You will also need to
    // generalize the position used by `RenderMesh`.
    if (window->KeyHold(GLFW_KEY_W)) {
        x_coord -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        x_coord += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        z_coord -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        z_coord += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        y_coord -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        y_coord += deltaTime;
    }
}


void Lab1::OnKeyPress(int key, int mods) {
    if (key == GLFW_KEY_F) {
        red = (rand() % 255) / 255.f;
        green = (rand() % 255) / 255.f;
        blue = (rand() % 255) / 255.f;
    }

    // TODO(student): Add a key press event that will let you cycle
    // through at least two meshes, rendered at the same position.
    // You will also need to generalize the mesh name used by `RenderMesh`.
    if (key == GLFW_KEY_H) {
        object = "sphere";
    }
    if (key == GLFW_KEY_B) {
        object = "box";
    }
    if (key == GLFW_KEY_T) {
        object = "teapot";
    }
}


void Lab1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
