#ifndef RUBIK_DRAWING_H
#define RUBIK_DRAWING_H

#include "Singleton.h"

class Drawing
{
    friend class Singleton<Drawing>;
    public:
        ~Drawing();

        bool Init();
        bool Render();

    private:
        Drawing();

        IrrlichtDevice* m_irrDevice;
        IVideoDriver* m_irrDriver;
        ISceneManager* m_irrScene;
        IGUIEnvironment* m_irrGui;
};

#define sDrawing Singleton<Drawing>::instance()

#endif
