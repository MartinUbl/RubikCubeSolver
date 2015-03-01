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
    SIrrlichtCreationParameters params = SIrrlichtCreationParameters();
    params.AntiAlias = 8;
    params.Bits = 32;
    params.DriverType = video::EDT_OPENGL;
    params.WindowSize = core::dimension2d<u32>(800, 600);
    m_irrDevice = createDeviceEx(params);

    if (!m_irrDevice)
        return false;

    m_irrDriver = m_irrDevice->getVideoDriver();
    m_irrScene = m_irrDevice->getSceneManager();
    m_irrGui = m_irrDevice->getGUIEnvironment();

    m_mainCamera = m_irrScene->addCameraSceneNode(0, vector3df(15, 25, -30), vector3df(0, 0, 0));
    m_irrScene->addLightSceneNode(m_mainCamera);

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
