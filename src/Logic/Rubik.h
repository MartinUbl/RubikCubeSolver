#ifndef RUBIK_RUBIK_H
#define RUBIK_RUBIK_H

#define CUBE_SIZE 18.0f
#define ATOM_SIZE CUBE_SIZE/3.0f
#define ATOM_SPACING 0.2f

static IMeshManipulator* meshManipulator = nullptr;

enum RubikColor
{
    CL_RED = 0,
    CL_GREEN = 1,
    CL_BLUE = 2,
    CL_YELLOW = 3,
    CL_WHITE = 4,
    CL_ORANGE = 5,

    CL_NONE = 6
};

static SColor rubikColorMap[] = {
    { 255, 200,   0,   0 },
    { 255,   0, 200,   0 },
    { 255,   0,   0, 200 },
    { 255, 200, 200,   0 },
    { 255, 255, 255, 255 },
    { 255, 255, 127,   0 },
    { 255,   0,   0,   0 }
};

static char rubikColorCode[] = { 'R', 'G', 'B', 'Y', 'W', 'O', '-' };

enum CubeFace
{
    CF_UP = 0,
    CF_DOWN = 1,
    CF_BACK = 2,
    CF_FRONT = 3,
    CF_RIGHT = 4,
    CF_LEFT = 5,

    CF_BEGIN = CF_UP,
    CF_END = CF_LEFT + 1,
    CF_COUNT = CF_END
};

static vector3df cubeFaceOffset[] = {
    { 0,                    ATOM_SIZE,           0 },
    { 0,                    0,                   0 },
    { 0,                    ATOM_SIZE / 2.0f,   ATOM_SIZE / 2.0f },
    { 0,                    ATOM_SIZE / 2.0f,  -ATOM_SIZE / 2.0f },
    { ATOM_SIZE / 2.0f,     ATOM_SIZE / 2.0f, 0 },
    { -ATOM_SIZE / 2.0f,    ATOM_SIZE / 2.0f, 0 }
};

static vector3df cubeFaceRotation[] = {
    { 0, 0, 0 },
    { -180.0f, 0, 0 },
    { 90.0f, 0.0f, 0.0f },
    { -90.0f, 0.0f, 0.0f },
    { -90.0f, -90.0f, 0 },
    { -90.0f, 90.0f, 0 }
};

class RubikCube;

struct CubeAtomFace
{
    IMeshSceneNode* meshNode;
    RubikColor color;
    RubikCube* parent;

    void setColor(RubikColor cl)
    {
        meshManipulator->setVertexColors(meshNode->getMesh(), rubikColorMap[cl]);
        color = cl;
    }
    RubikColor getColor()
    {
        return color;
    }
};

struct CubeAtom
{
    CubeAtomFace* faces[CF_COUNT];
};

class RubikCube
{
    public:
        RubikCube();
        ~RubikCube();

        void Render();

        void BuildCube(ISceneManager* scene, IVideoDriver* videoDriver);
        CubeAtom* GetAtom(int x, int y, int z);

        void PrintOut();

    private:
        CubeAtom* m_cubeAtoms[3][3][3];
        RubikColor m_faceArray[CF_COUNT][3][3];
        ITexture* m_faceTexture, *m_faceMiniTexture;

        void SetCubeAtom(int x, int y, int z, CubeAtom* atom);
        void CacheCube();
        CubeAtom* BuildCubeAtom(ISceneManager* scene, IVideoDriver* videoDriver, vector3df basePosition, vector3di cubeOffset);
        CubeAtomFace* BuildFace(ISceneManager* scene, IVideoDriver* videoDriver, CubeFace side, vector3df basePosition);
};

#endif
