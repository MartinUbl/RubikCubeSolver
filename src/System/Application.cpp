#include "Global.h"
#include "Application.h"
#include "Drawing.h"

#include <ctime>

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

    srand((unsigned int)time(NULL));

    return true;
}

int Application::Run()
{
    int frames = 99;

    while (true)
    {
        if (!sDrawing->Render())
            break;

        if (++frames == 100)
        {
            core::stringw str = L"Rubik's Cube - KIV/UIR [ ";
            str += (s32)sDrawing->getDriver()->getFPS();
            str += L" FPS ]";

            sDrawing->getDevice()->setWindowCaption(str.c_str());
            frames = 0;
        }
    }

    return 0;
}
