#include "SVEApp.h"

void SVEApp::Run() 
{
    while (!sveWindow.ShouldClose()) 
    {
        glfwPollEvents();
    }
}