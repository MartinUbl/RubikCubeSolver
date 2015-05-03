#include "Global.h"
#include "bigint.h"
#include <queue>
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
    params.EventReceiver = &m_eventReceiver;
    m_irrDevice = createDeviceEx(params);

    if (!m_irrDevice)
        return false;

    m_irrDriver = m_irrDevice->getVideoDriver();
    m_irrScene = m_irrDevice->getSceneManager();
    m_irrGui = m_irrDevice->getGUIEnvironment();

    m_cameraAngleX = - PI / 4.0f;
    m_cameraAngleY = PI / 4.0f;

    m_mainCamera = m_irrScene->addCameraSceneNode(0, vector3df(25, 25, -25), vector3df(0, 0, 0));
    updateCameraPosition();

    m_irrScene->addLightSceneNode(m_mainCamera);

    m_cube = new RubikCube();
    m_cube->BuildCube(m_irrScene, m_irrDriver);

    m_appFont = m_irrGui->getFont("../data/appfont.png");
    m_appFont->setKerningWidth(-3);

    return true;
}

bool Drawing::Render()
{
    if (!m_irrDevice)
        return false;

    if (!m_irrDevice->run())
        return false;

    m_irrDriver->beginScene(true, true, SColor(255, 120, 190, 130));

    m_irrScene->drawAll();
    m_cube->Render();

    m_appFont->draw(L"OVLADANI\nR\t\t\t\t\trozmichat\nS\t\t\t\t\tslozit\n+ -\t\t\tzrychlit/zpomalit otaceni", rect<s32>(5, 600 - 24*5, 100, 100), SColor(255, 0, 0, 127));

    stringw repstr = "Rychlost otaceni: ";
    std::string str = std::to_string(m_cube->GetFlipTiming() / 1000.0f);
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    repstr += str.c_str();
    repstr += " s";

    m_appFont->draw(repstr, rect<s32>(5, 600 - 24, 100, 100), SColor(255, 0, 50, 240));

    m_irrGui->drawAll();

    m_irrDriver->endScene();

    return true;
}

void Drawing::updateCameraPosition()
{
    m_mainCamera->setPosition(vector3df(35 * cos(m_cameraAngleX) * cos(m_cameraAngleY), 35 * sin(m_cameraAngleY), 35 * sin(m_cameraAngleX) * cos(m_cameraAngleY)));
}

void Drawing::mouseMoveCallback(int deltaX, int deltaY)
{
    if (m_eventReceiver.GetMouseState().LeftButtonDown)
    {
        m_cameraAngleX -= (float)deltaX * PI / 280.0f;
        m_cameraAngleY += (float)deltaY * PI / 280.0f;

        updateCameraPosition();
    }
}

bool EventReceiver::OnEvent(const SEvent& event)
{
    // Remember the mouse state
    if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
    {
        switch (event.MouseInput.Event)
        {
            case EMIE_LMOUSE_PRESSED_DOWN:
                MouseState.LeftButtonDown = true;
                break;
            case EMIE_LMOUSE_LEFT_UP:
                MouseState.LeftButtonDown = false;
                break;
            case EMIE_MOUSE_MOVED:
                sDrawing->mouseMoveCallback(event.MouseInput.X - MouseState.Position.X, event.MouseInput.Y - MouseState.Position.Y);
                MouseState.Position.X = event.MouseInput.X;
                MouseState.Position.Y = event.MouseInput.Y;
                break;
            default:
                break;
        }
    }
    else if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown)
    {
        switch (event.KeyInput.Key)
        {
            case KEY_KEY_R:
            {
                cout << "Nahodne rozmichavam kostku:" << endl;
                std::list<CubeFlip> fliplist;
                sDrawing->getCube()->Scramble(&fliplist);
                sDrawing->getCube()->ProceedFlipSequence(&fliplist, true);
                break;
            }
            case KEY_KEY_S:
            {
                cout << "Hledam reseni..." << endl;
                std::list<CubeFlip> fliplist;
                sDrawing->getCube()->Solve(&fliplist);
                cout << "Resim kostku:" << endl;
                sDrawing->getCube()->ProceedFlipSequence(&fliplist, true);
                break;
            }
            case KEY_MINUS:
            case KEY_SUBTRACT:
            {
                sDrawing->getCube()->UpdateFlipTiming(+100);
                break;
            }
            case KEY_PLUS:
            case KEY_ADD:
            {
                sDrawing->getCube()->UpdateFlipTiming(-100);
                break;
            }
        }
    }

    return false;
}
