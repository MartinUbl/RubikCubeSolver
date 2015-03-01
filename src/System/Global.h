#ifndef RUBIK_GLOBAL_H
#define RUBIK_GLOBAL_H

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <irrlicht.h>

using namespace std;

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#endif
