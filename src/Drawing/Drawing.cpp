#include "Global.h"
#include "Rubik.h"
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
    m_irrDevice = createDevice(video::EDT_SOFTWARE, dimension2d<u32>(800, 600), 32, false, true, true, 0);

    if (!m_irrDevice)
        return false;

    m_irrDriver = m_irrDevice->getVideoDriver();
    m_irrScene = m_irrDevice->getSceneManager();
    m_irrGui = m_irrDevice->getGUIEnvironment();

    m_mainCamera = m_irrScene->addCameraSceneNode(0, vector3df(15, 25, -30), vector3df(0, 0, 0));

    m_cube = new RubikCube();
    m_cube->BuildCube(m_irrScene, m_irrDriver);

    return true;
}

bool Drawing::Render()
{
    if (!m_irrDevice)
        return false;

    if (!m_irrDevice->run())
        return false;

    m_irrDriver->beginScene(true, true, SColor(255, 100, 140, 101));

    m_irrScene->drawAll();
    m_irrGui->drawAll();

    m_irrDriver->endScene();

    return true;
}
