#include "Global.h"
#include "Drawing.h"

Drawing::Drawing()
{
    //
}

Drawing::~Drawing()
{
    //
}

bool Drawing::Init()
{
    m_irrDevice = createDevice(video::EDT_SOFTWARE, dimension2d<u32>(640, 480), 16, false, false, false, 0);

    if (!m_irrDevice)
        return false;

    m_irrDevice->setWindowCaption(L"Rubik's Cube - KIV/UIR");

    m_irrDriver = m_irrDevice->getVideoDriver();
    m_irrScene = m_irrDevice->getSceneManager();
    m_irrGui = m_irrDevice->getGUIEnvironment();

    return true;
}

bool Drawing::Render()
{
    if (!m_irrDevice)
        return false;

    if (!m_irrDevice->run())
        return false;

    m_irrDriver->beginScene(true, true, SColor(255, 100, 101, 140));

    m_irrScene->drawAll();
    m_irrGui->drawAll();

    m_irrDriver->endScene();

    return true;
}
