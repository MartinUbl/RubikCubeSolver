#include "Global.h"
#include "Application.h"
#include "Drawing.h"

Application::Application()
{
    //
}

Application::~Application()
{
    //
}

bool Application::Init(int argc, char** argv)
{
    if (!sDrawing->Init())
        return false;

    return true;
}

int Application::Run()
{
    while (true)
    {
        if (!sDrawing->Render())
            break;
    }

    return 0;
}
