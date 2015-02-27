#ifndef RUBIK_APPLICATION_H
#define RUBIK_APPLICATION_H

#include "Singleton.h"

class Application
{
    friend class Singleton<Application>;

    public:
        ~Application();

        bool Init(int argc, char** argv);
        int Run();

    private:
        Application();
};

#define sApplication Singleton<Application>::instance()

#endif
