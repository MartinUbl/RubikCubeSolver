#ifndef RUBIK_DRAWING_H
#define RUBIK_DRAWING_H

#include "Singleton.h"

class RubikCube;

class Drawing
{
    friend class Singleton<Drawing>;
    public:
        ~Drawing();

        bool Init();
        bool Render();

        IrrlichtDevice* getDevice() { return m_irrDevice; };
        IVideoDriver* getDriver() { return m_irrDriver; };

    private:
        Drawing();

        IrrlichtDevice* m_irrDevice;
        IVideoDriver* m_irrDriver;
        ISceneManager* m_irrScene;
        IGUIEnvironment* m_irrGui;

        ICameraSceneNode* m_mainCamera;

        RubikCube* m_cube;
};

#define sDrawing Singleton<Drawing>::instance()

#endif
