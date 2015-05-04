#ifndef RUBIK_RUBIK_H
#define RUBIK_RUBIK_H

#include <queue>
#include "bigint.h"

#define CUBE_SIZE 18.0f
#define ATOM_SIZE CUBE_SIZE/3.0f
#define ATOM_SPACING 0.2f
#define ANIM_TIMER_DEFAULT 200

static IMeshManipulator* meshManipulator = nullptr;

#define STATE_STRING_LENGTH 20

enum RubikColor
{
    CL_RED = 0,
    CL_GREEN = 1,
    CL_BLUE = 2,
    CL_YELLOW = 3,
    CL_WHITE = 4,
    CL_ORANGE = 5,

    CL_NONE = 6,
    CL_COUNT = 6
};

static SColor rubikColorMap[] = {
    { 255, 200,   0,   0 },
    { 255,   0, 200,   0 },
    { 255,   0,   0, 200 },
    { 255, 200, 200,   0 },
    { 255, 255, 255, 255 },
    { 255, 207,  64,   0 },
    { 255,   0,   0,   0 }
};

static char rubikColorCode[] = { 'R', 'G', 'B', 'Y', 'W', 'O', '-' };

static RubikColor getColorForCode(char code)
{
    for (int i = 0; i < CL_COUNT; i++)
        if (code == rubikColorCode[i])
            return (RubikColor)i;
    return CL_NONE;
}

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

static char rubikFaceCode[] = { 'U', 'D', 'B', 'F', 'R', 'L' };

// this will then depend on user input (these are default values)
static CubeFace colorFaceMap[] = {
    /* CL_RED */    CF_UP,
    /* CL_GREEN */  CF_FRONT,
    /* CL_BLUE */   CF_LEFT,
    /* CL_YELLOW */ CF_RIGHT,
    /* CL_WHITE */  CF_DOWN,
    /* CL_ORANGE */ CF_BACK
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
    { 0.0f, 0.0f, -90.0f },
    { 0.0f, 0.0f, 90.0f }
};

class RubikCube;

struct CubeAtomFace
{
    IMeshSceneNode* meshNode;
    vector3df basePosition;
    vector3df baseRotation;
    RubikColor color;
    RubikCube* parent;

    void setColor(RubikColor cl);
    RubikColor getColor() { return color; }
};

struct CubeAtom
{
    CubeAtomFace* faces[CF_COUNT];
};

// quarter-turn metric flips
enum CubeFlip
{
    FLIP_R_P = 0,
    FLIP_R_2 = 1,
    FLIP_R_N = 2,

    FLIP_L_P = 3,
    FLIP_L_2 = 4,
    FLIP_L_N = 5,

    FLIP_B_P = 6,
    FLIP_B_2 = 7,
    FLIP_B_N = 8,

    FLIP_F_P = 9,
    FLIP_F_2 = 10,
    FLIP_F_N = 11,

    FLIP_D_P = 12,
    FLIP_D_2 = 13,
    FLIP_D_N = 14,

    FLIP_U_P = 15,
    FLIP_U_2 = 16,
    FLIP_U_N = 17,

    FLIP_MAX = FLIP_U_N + 1,
    FLIP_BEGIN = FLIP_R_P,

    FLIP_NONE       // used just as "flag", not real turn
};

static char* cubeFlipStr[] = { "R+", "R2", "R-", "L+", "L2", "L-", "B+", "B2", "B-", "F+", "F2", "F-", "D+", "D2", "D-", "U+", "U2", "U-" };

static int stageAllowedFlips[] = {
    1 << FLIP_U_P | 1 << FLIP_D_P | 1 << FLIP_F_P | 1 << FLIP_B_P | 1 << FLIP_L_P | 1 << FLIP_R_P | 1 << FLIP_U_N | 1 << FLIP_D_N | 1 << FLIP_F_N | 1 << FLIP_B_N | 1 << FLIP_L_N | 1 << FLIP_R_N,
    1 << FLIP_U_P | 1 << FLIP_D_P | 1 << FLIP_F_2 | 1 << FLIP_B_2 | 1 << FLIP_L_P | 1 << FLIP_R_P | 1 << FLIP_U_N | 1 << FLIP_D_N | 1 << FLIP_L_N | 1 << FLIP_R_N,
    1 << FLIP_U_2 | 1 << FLIP_D_2 | 1 << FLIP_F_2 | 1 << FLIP_B_2 | 1 << FLIP_L_P | 1 << FLIP_R_P | 1 << FLIP_L_N | 1 << FLIP_R_N,
    1 << FLIP_U_2 | 1 << FLIP_D_2 | 1 << FLIP_F_2 | 1 << FLIP_B_2 | 1 << FLIP_L_2 | 1 << FLIP_R_2,
};

static int flipCubeEffect[][8] = {
    { 0, 1, 2, 3, 0, 1, 2, 3 },    // U
    { 4, 7, 6, 5, 4, 5, 6, 7 },    // D
    { 0, 9, 4, 8, 0, 3, 5, 4 },    // F
    { 2, 10, 6, 11, 2, 1, 7, 6 },  // B
    { 3, 11, 7, 9, 3, 2, 6, 5 },   // L
    { 1, 8, 5, 10, 1, 0, 4, 7 },   // R
};

// retrieves flip for supplied string identifier
static CubeFlip getFlipForStr(char* str)
{
    for (int i = 0; i < FLIP_MAX; i++)
    {
        if (strcmp(str, cubeFlipStr[i]) == 0)
            return (CubeFlip)i;
    }

    return FLIP_NONE;
}

static char* getStrForFlip(CubeFlip fl)
{
    if (fl < FLIP_MAX)
        return cubeFlipStr[fl];
    return nullptr;
}

#include "Singleton.h"

#define sCube Singleton<RubikCube>::instance()

class RubikCube
{
    friend class Singleton<RubikCube>;
    public:
        ~RubikCube();

        bool LoadFromFile(char* filename);

        void Render();

        void BuildCube(ISceneManager* scene, IVideoDriver* videoDriver);
        CubeAtom* GetAtom(int x, int y, int z);

        void DoFlip(CubeFlip flip, bool draw);

        void Scramble(std::list<CubeFlip> *target);
        void Solve(std::list<CubeFlip> *target);

        void ProceedFlipSequence(std::list<CubeFlip> *source, bool animate);

        void PrintOut();

        void UpdateFlipTiming(int howmuch) { if (m_flipTiming + howmuch >= 100 && m_flipTiming + howmuch <= 5000) m_flipTiming += howmuch; };
        int GetFlipTiming() { return m_flipTiming; };

    private:
        RubikCube();

        CubeAtom* m_cubeAtoms[3][3][3];
        RubikColor m_cubeCache[CF_COUNT][3][3];
        ITexture* m_faceTexture, *m_faceMiniTexture;
        unsigned char m_solveStage;
        int m_flipTiming;

        CubeFlip m_toProgress;
        unsigned int m_progressStart;
        std::queue<CubeFlip> m_flipQueue;

        void SetCubeAtom(int x, int y, int z, CubeAtom* atom);
        CubeAtom* BuildCubeAtom(ISceneManager* scene, IVideoDriver* videoDriver, vector3df basePosition, vector3di cubeOffset);
        CubeAtomFace* BuildFace(ISceneManager* scene, IVideoDriver* videoDriver, CubeFace side, vector3df basePosition);
        void CacheCube();
        void RestoreCacheCube();

        bigint GetStateHash(bigint &state);
        void ConvertToPermutationTable(std::vector<std::string> &dstList);
        char* GetEdgeCode(int x, int y, int z, CubeFace frst, CubeFace scnd);
        char* GetCornerCode(int x, int y, int z, CubeFace frst, CubeFace scnd, CubeFace thrd);
        bigint DoLinearFlip(int move, bigint state);

        void AtomCircularSwap(int ax, int ay, int az, CubeFace a, int bx, int by, int bz, CubeFace b, int cx, int cy, int cz, CubeFace c, int dx, int dy, int dz, CubeFace d, bool reverse = false);
};

#endif
