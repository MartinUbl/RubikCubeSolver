#include "Global.h"
#include "bigint.h"
#include <queue>
#include "Rubik.h"
#include "Drawing.h"

#include <time.h>
#include <string>
#include <utility>
using namespace std;

RubikCube::RubikCube()
{
    m_progressStart = 0;
    m_toProgress = FLIP_NONE;
    m_flipTiming = ANIM_TIMER_DEFAULT;
}

RubikCube::~RubikCube()
{
    //
}

CubeAtomFace* RubikCube::BuildFace(ISceneManager* scene, IVideoDriver* videoDriver, CubeFace side, vector3df basePosition)
{
    IMeshSceneNode* node;
    IMesh* mesh = scene->getGeometryCreator()->createPlaneMesh(dimension2d<f32>(ATOM_SIZE, ATOM_SIZE), dimension2d<u32>(1, 1), nullptr);
    node = scene->addMeshSceneNode(mesh, nullptr, -1, basePosition + cubeFaceOffset[side], cubeFaceRotation[side]);
    node->setMaterialTexture(0, m_faceTexture);

    CubeAtomFace* face = new CubeAtomFace();
    face->meshNode = node;
    face->basePosition = basePosition + cubeFaceOffset[side];
    face->baseRotation = cubeFaceRotation[side];
    face->parent = this;

    return face;
}

CubeAtom* RubikCube::BuildCubeAtom(ISceneManager* scene, IVideoDriver* videoDriver, vector3df basePosition, vector3di cubeOffset)
{
    CubeAtom* atom = new CubeAtom();

    for (int i = CF_BEGIN; i < CF_END; i++)
    {
        atom->faces[i] = BuildFace(scene, videoDriver, (CubeFace)i, basePosition);
        atom->faces[i]->setColor(CL_NONE);
    }

    if (cubeOffset.X == 1)
        atom->faces[CF_RIGHT]->setColor(CL_YELLOW);
    else if (cubeOffset.X == -1)
        atom->faces[CF_LEFT]->setColor(CL_BLUE);

    if (cubeOffset.Z == -1)
        atom->faces[CF_FRONT]->setColor(CL_GREEN);
    else if (cubeOffset.Z == 1)
        atom->faces[CF_BACK]->setColor(CL_ORANGE);

    if (cubeOffset.Y == 1)
        atom->faces[CF_UP]->setColor(CL_RED);
    else if (cubeOffset.Y == -1)
        atom->faces[CF_DOWN]->setColor(CL_WHITE);

    /*for (int i = CF_BEGIN; i < CF_END; i++)
    {
        if (atom->faces[i]->color == CL_NONE)
        {
            atom->faces[i]->meshNode->remove();
            atom->faces[i]->meshNode = nullptr;
        }
    }*/

    return atom;
}

void RubikCube::SetCubeAtom(int x, int y, int z, CubeAtom* atom)
{
    m_cubeAtoms[(x + 1)][(y + 1)][(z + 1)] = atom;
}

CubeAtom* RubikCube::GetAtom(int x, int y, int z)
{
    return m_cubeAtoms[(x + 1)][(y + 1)][(z + 1)];
}

void RubikCube::CacheCube()
{
    CubeAtom* ca;

    for (int x = 0; x <= 2; x++)
    {
        for (int y = 0; y <= 2; y++)
        {
            for (int z = 0; z <= 2; z++)
            {
                if (x == 1 && y == 1 && z == 1)
                    continue;
                ca = GetAtom(x-1, y-1, z-1);
                if (ca == nullptr)
                    continue;

                if (z == 0)
                    m_cubeCache[CF_FRONT][x][y] = ca->faces[CF_FRONT]->getColor();
                else if (z == 2)
                    m_cubeCache[CF_BACK][x][y] = ca->faces[CF_BACK]->getColor();

                if (x == 2)
                    m_cubeCache[CF_RIGHT][y][z] = ca->faces[CF_RIGHT]->getColor();
                else if (x == 0)
                    m_cubeCache[CF_LEFT][2-y][z] = ca->faces[CF_LEFT]->getColor();

                if (y == 2)
                    m_cubeCache[CF_UP][x][z] = ca->faces[CF_UP]->getColor();
                else if (y == 0)
                    m_cubeCache[CF_DOWN][x][z] = ca->faces[CF_DOWN]->getColor();
            }
        }
    }
}

void RubikCube::Render()
{
    if (m_toProgress != FLIP_NONE && m_progressStart > 0)
    {
        unsigned int diff = getMSTimeDiff(m_progressStart, getMSTime());
        float progress = ((float)diff) / (float)m_flipTiming;

        bool endflag = false;
        if (progress >= 1.0f)
        {
            endflag = true;
            progress = 0;
        }

        int x, y, z, dirX, dirY, dirZ, rotX, rotY, rotZ;
        int group;

        CubeAtom* at;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                rotX = (m_toProgress % 3) == 2 ? -1 : 1;
                rotY = rotX;
                rotZ = rotX;

                switch (m_toProgress)
                {
                    // GROUP 0
                    case FLIP_F_P:
                    case FLIP_F_N:
                        x = i;
                        y = j;
                        z = -1;
                        rotX = 0;
                        rotY = 0;
                        rotZ *= 1;
                        dirX = rotZ;
                        dirY = 1;

                        group = 0;
                        break;
                    case FLIP_B_P:
                    case FLIP_B_N:
                        x = - i;
                        y = j;
                        z = 1;
                        rotX = 0;
                        rotY = 0;
                        rotZ *= -1;
                        dirX = rotZ;
                        dirY = 1;

                        group = 0;
                        break;
                    // GROUP 1
                    case FLIP_L_P:
                    case FLIP_L_N:
                        x = -1;
                        y = j;
                        z = -i;
                        rotX *= 1;
                        rotY = 0;
                        rotZ = 0;
                        dirX = 1;
                        dirY = 1;
                        dirZ = -rotX;

                        group = 1;
                        break;
                    case FLIP_R_P:
                    case FLIP_R_N:
                        x = 1;
                        y = j;
                        z = i;
                        rotX *= -1;
                        rotY = 0;
                        rotZ = 0;
                        dirX = 1;
                        dirY = 1;
                        dirZ = -rotX;

                        group = 1;
                        break;
                    // GROUP 2
                    case FLIP_U_P:
                    case FLIP_U_N:
                        x = i;
                        y = 1;
                        z = j;
                        rotX = 0;
                        rotY *= -1;
                        rotZ = 0;
                        dirX = 0;
                        dirY = rotY;
                        dirZ = 0;

                        group = 2;
                        break;
                    case FLIP_D_P:
                    case FLIP_D_N:
                        x = -i;
                        y = -1;
                        z = j;
                        rotX = 0;
                        rotY *= 1;
                        rotZ = 0;
                        dirX = 0;
                        dirY = -rotY;
                        dirZ = 0;

                        group = 2;
                        break;
                }

                at = GetAtom(x, y, z);
                if (!at)
                    continue;

                for (int f = CF_BEGIN; f < CF_END; f++)
                {
                    CubeAtomFace* caf = at->faces[f];
                    if (!caf->meshNode)
                        continue;
                    bool black = caf->color == CL_NONE;

                    // group 0 = front and back flip
                    if (group == 0)
                    {
                        float baseAngle = atan(caf->basePosition.Y / caf->basePosition.X);
                        if (x == -1)
                            baseAngle -= PI;

                        // otherwise glitches would appear
                        if (x == 0 && black && f == CF_LEFT)
                            baseAngle += PI;

                        float baseDist = sqrt(caf->basePosition.X*caf->basePosition.X + caf->basePosition.Y*caf->basePosition.Y);

                        // do not move center
                        if (x != 0 || y != 0)
                        {
                            caf->meshNode->setPosition(
                                vector3df
                                (
                                    cos(baseAngle - rotZ * progress*PI / 2.0f)*baseDist - dirX * progress * ATOM_SIZE / 2.0f,
                                    sin(baseAngle - rotZ * progress*PI / 2.0f)*baseDist + dirY * progress * ATOM_SIZE / 2.0f,
                                    caf->basePosition.Z
                                )
                            );
                        }

                        caf->meshNode->setRotation(
                            vector3df
                            (
                                caf->baseRotation.X - rotX * progress * 90.0f,
                                caf->baseRotation.Y - rotY * progress * 90.0f,
                                caf->baseRotation.Z - rotZ * progress * 90.0f
                            )
                        );
                    }
                    // group 1 = left and right flip
                    else if (group == 1)
                    {
                        float baseAngle = atan(caf->basePosition.Y / caf->basePosition.Z);
                        if (z == -1)
                            baseAngle -= PI;

                        // otherwise glitches would appear
                        if (z == 0 && black && f == CF_FRONT)
                            baseAngle += PI;

                        float baseDist = sqrt(caf->basePosition.Z*caf->basePosition.Z + caf->basePosition.Y*caf->basePosition.Y);
                        // do not move center
                        if (z != 0 || y != 0)
                        {
                            caf->meshNode->setPosition(
                                vector3df
                                (
                                    caf->basePosition.X,
                                    sin(baseAngle + rotX * progress*PI / 2.0f)*baseDist + dirY * progress * ATOM_SIZE / 2.0f,
                                    cos(baseAngle + rotX * progress*PI / 2.0f)*baseDist - dirZ * progress * ATOM_SIZE / 2.0f
                                )
                            );
                        }

                        if (f == CF_RIGHT || f == CF_LEFT)
                        {
                            caf->meshNode->setRotation(
                                vector3df
                                (
                                    caf->baseRotation.X,
                                    caf->baseRotation.Y + (((f == CF_LEFT) ? (rotX) : (-rotX)) * progress * 90.0f),
                                    caf->baseRotation.Z
                                )
                            );
                        }
                        else
                        {
                            caf->meshNode->setRotation(
                                vector3df
                                (
                                    caf->baseRotation.X - rotX * progress * 90.0f,
                                    caf->baseRotation.Y - rotY * progress * 90.0f,
                                    caf->baseRotation.Z - rotZ * progress * 90.0f
                                )
                            );
                        }
                    }
                    // group 2 = up and down flip
                    else if (group == 2)
                    {
                        float baseAngle = atan(caf->basePosition.Z / caf->basePosition.X);
                        if (x == -1)
                            baseAngle -= PI;

                        float baseDist = sqrt(caf->basePosition.Z*caf->basePosition.Z + caf->basePosition.X*caf->basePosition.X);

                        // do not move center
                        if (x != 0 || z != 0)
                        {
                            caf->meshNode->setPosition(
                                vector3df
                                    (
                                        cos(baseAngle + rotY * progress*PI / 2.0f)*baseDist + dirX * progress * ATOM_SIZE / 4.0f,
                                        caf->basePosition.Y,
                                        sin(baseAngle + rotY * progress*PI / 2.0f)*baseDist + dirZ * progress * ATOM_SIZE / 4.0f
                                    )
                                );
                        }

                        if (f == CF_UP || f == CF_DOWN)
                        {
                            caf->meshNode->setRotation(
                                vector3df
                                (
                                    caf->baseRotation.X,
                                    caf->baseRotation.Y - rotY * progress * 90.0f,
                                    caf->baseRotation.Z
                                )
                            );
                        }
                        else if (f == CF_RIGHT || f == CF_LEFT)
                        {
                            caf->meshNode->setRotation(
                                vector3df
                                (
                                    caf->baseRotation.X + ((f == CF_LEFT) ? (-rotY) : (rotY)) * progress * 90.0f,
                                    caf->baseRotation.Y,
                                    caf->baseRotation.Z
                                )
                            );
                        }
                        else // front / back
                        {
                            caf->meshNode->setRotation(
                                vector3df
                                (
                                    caf->baseRotation.X,
                                    caf->baseRotation.Y - rotY * progress * 90.0f,
                                    caf->baseRotation.Z
                                )
                            );
                        }
                    }
                }
            }
        }

        if (endflag)
        {
            DoFlip(m_toProgress, true);

            // set next move from queue if any
            if (m_flipQueue.size() > 0)
            {
                cout << getStrForFlip(m_toProgress) << ", ";
                m_toProgress = m_flipQueue.front();
                m_flipQueue.pop();

                m_progressStart = getMSTime();
            }
            else
            {
                m_toProgress = FLIP_NONE;
                cout << "\b\b  " << endl;
            }
        }
    }

    int i, j;

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 60 + i*20, 2 + 40 - j*20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_cubeCache[CF_UP][i][j]], false);
        }
    }

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 40 - j * 20, 2 + 60 + i * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_cubeCache[CF_LEFT][i][j]], false);
        }
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 60 + i * 20, 2 + 60 + 40 - j * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_cubeCache[CF_FRONT][i][j]], false);
        }
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 2*60 + j * 20, 2 + 60 + 40 - i * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_cubeCache[CF_RIGHT][i][j]], false);
        }
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 3*60 + 40 - i * 20, 2 + 60 + 40 - j * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_cubeCache[CF_BACK][i][j]], false);
        }
    }

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 60 + i * 20, 2 + 2*60 + j * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_cubeCache[CF_DOWN][i][j]], false);
        }
    }
}

void RubikCube::Scramble(std::list<CubeFlip> *target)
{
    CubeFlip a;
    int count = 20 + rand() % 10;
    for (int i = 0; i < count; i++)
    {
        a = (CubeFlip)(rand() % FLIP_MAX);
        target->push_back(a);
    }
}

void RubikCube::ProceedFlipSequence(std::list<CubeFlip> *source, bool animate)
{
    if (!animate)
    {
        for (std::list<CubeFlip>::iterator itr = source->begin(); itr != source->end(); ++itr)
            DoFlip(*itr, true);
    }
    else
    {
        if (!source || source->empty())
            return;

        CubeFlip fl;
        for (std::list<CubeFlip>::iterator itr = source->begin(); itr != source->end(); ++itr)
        {
            fl = *itr;

            if (fl % 3 == 1)
            {
                fl = (CubeFlip)(fl - 1);
                m_flipQueue.push(fl);
            }
            m_flipQueue.push(fl);
        }

        m_toProgress = m_flipQueue.front();
        m_flipQueue.pop();

        cout << getStrForFlip(m_toProgress) << ", ";

        m_progressStart = getMSTime();
    }
}

void RubikCube::BuildCube(ISceneManager* scene, IVideoDriver* videoDriver)
{
    meshManipulator = scene->getMeshManipulator();

    m_faceTexture = videoDriver->getTexture("../data/face.bmp");
    m_faceMiniTexture = videoDriver->getTexture("../data/mini_face.bmp");

    CubeAtom* tmp;

    for (int ix = -1; ix <= 1; ix++)
    {
        for (int iy = -1; iy <= 1; iy++)
        {
            for (int iz = -1; iz <= 1; iz++)
            {
                if (ix == 0 && iy == 0 && iz == 0)
                    continue;

                tmp = BuildCubeAtom(scene, videoDriver, vector3df(ix*(ATOM_SIZE + ATOM_SPACING), iy*(ATOM_SIZE + ATOM_SPACING), iz*(ATOM_SIZE + ATOM_SPACING)),
                    vector3di(ix, iy, iz));

                SetCubeAtom(ix, iy, iz, tmp);
            }
        }
    }

    CacheCube();
}

// circular swap of cube atom faces
void RubikCube::AtomCircularSwap(int ax, int ay, int az, CubeFace a, int bx, int by, int bz, CubeFace b, int cx, int cy, int cz, CubeFace c, int dx, int dy, int dz, CubeFace d, bool reverse /* = false */)
{
    // get atoms
    CubeAtom* _a = GetAtom(ax, ay, az);
    CubeAtom* _b = GetAtom(bx, by, bz);
    CubeAtom* _c = GetAtom(cx, cy, cz);
    CubeAtom* _d = GetAtom(dx, dy, dz);

    // get their original values
    RubikColor c_a = _a->faces[a]->getColor();
    RubikColor c_b = _b->faces[b]->getColor();
    RubikColor c_c = _c->faces[c]->getColor();
    RubikColor c_d = _d->faces[d]->getColor();

    // yes, this may be a bit more optimalized, but this step is done only when we know what we are doing
    // - means we use different flipping (swapping, ..) method for critical applications like cube solving

    // and flip them, in requested direction
    if (!reverse)
    {
        _a->faces[a]->setColor(c_d);
        _b->faces[b]->setColor(c_a);
        _c->faces[c]->setColor(c_b);
        _d->faces[d]->setColor(c_c);
    }
    else
    {
        _a->faces[a]->setColor(c_b);
        _b->faces[b]->setColor(c_c);
        _c->faces[c]->setColor(c_d);
        _d->faces[d]->setColor(c_a);
    }
}

// perform swap on visual cube, and prepare inputs for solving mechanism as well (same source)
void RubikCube::DoFlip(CubeFlip flip, bool draw)
{
    switch (flip)
    {
        case FLIP_F_P:
        {
            AtomCircularSwap(-1, 1, -1, CF_UP,   1, 1, -1, CF_RIGHT, 1, -1, -1, CF_DOWN,  -1, -1, -1, CF_LEFT);
            AtomCircularSwap( 0, 1, -1, CF_UP,   1, 0, -1, CF_RIGHT, 0, -1, -1, CF_DOWN,  -1,  0, -1, CF_LEFT);
            AtomCircularSwap(-1, 1, -1, CF_LEFT, 1, 1, -1, CF_UP,    1, -1, -1, CF_RIGHT, -1, -1, -1, CF_DOWN);

            AtomCircularSwap(-1, 1, -1, CF_FRONT, 1, 1, -1, CF_FRONT, 1, -1, -1, CF_FRONT, -1, -1, -1, CF_FRONT);
            AtomCircularSwap(0, 1, -1,  CF_FRONT, 1, 0, -1, CF_FRONT, 0, -1, -1, CF_FRONT, -1, 0, -1,  CF_FRONT);
            break;
        }
        case FLIP_F_2:
        {
            DoFlip(FLIP_F_P, false);
            DoFlip(FLIP_F_P, draw);
            break;
        }
        case FLIP_F_N:
        {
            AtomCircularSwap(-1, 1, -1, CF_UP, 1, 1, -1, CF_RIGHT, 1, -1, -1, CF_DOWN, -1, -1, -1, CF_LEFT, true);
            AtomCircularSwap(0, 1, -1, CF_UP, 1, 0, -1, CF_RIGHT, 0, -1, -1, CF_DOWN, -1, 0, -1, CF_LEFT, true);
            AtomCircularSwap(-1, 1, -1, CF_LEFT, 1, 1, -1, CF_UP, 1, -1, -1, CF_RIGHT, -1, -1, -1, CF_DOWN, true);

            AtomCircularSwap(-1, 1, -1, CF_FRONT, 1, 1, -1, CF_FRONT, 1, -1, -1, CF_FRONT, -1, -1, -1, CF_FRONT, true);
            AtomCircularSwap(0, 1, -1, CF_FRONT, 1, 0, -1, CF_FRONT, 0, -1, -1, CF_FRONT, -1, 0, -1, CF_FRONT, true);
            break;
        }
        case FLIP_U_P:
        {
            AtomCircularSwap(-1, 1, -1, CF_FRONT, 1, 1, -1, CF_RIGHT, 1, 1, 1, CF_BACK, -1, 1, 1, CF_LEFT, true);
            AtomCircularSwap(0, 1, -1, CF_FRONT, 1, 1, 0, CF_RIGHT, 0, 1, 1, CF_BACK, -1, 1, 0, CF_LEFT, true);
            AtomCircularSwap(-1, 1, -1, CF_LEFT, 1, 1, -1, CF_FRONT, 1, 1, 1, CF_RIGHT, -1, 1, 1, CF_BACK, true);

            AtomCircularSwap(-1, 1, -1, CF_UP, 1, 1, -1, CF_UP, 1, 1, 1, CF_UP, -1, 1, 1, CF_UP, true);
            AtomCircularSwap(0, 1, -1, CF_UP, 1, 1, 0, CF_UP, 0, 1, 1, CF_UP, -1, 1, 0, CF_UP, true);
            break;
        }
        case FLIP_U_2:
        {
            DoFlip(FLIP_U_P, draw);
            DoFlip(FLIP_U_P, draw);
            break;
        }
        case FLIP_U_N:
        {
            AtomCircularSwap(-1, 1, -1, CF_FRONT, 1, 1, -1, CF_RIGHT, 1, 1, 1, CF_BACK, -1, 1, 1, CF_LEFT);
            AtomCircularSwap(0, 1, -1, CF_FRONT, 1, 1, 0, CF_RIGHT, 0, 1, 1, CF_BACK, -1, 1, 0, CF_LEFT);
            AtomCircularSwap(-1, 1, -1, CF_LEFT, 1, 1, -1, CF_FRONT, 1, 1, 1, CF_RIGHT, -1, 1, 1, CF_BACK);

            AtomCircularSwap(-1, 1, -1, CF_UP, 1, 1, -1, CF_UP, 1, 1, 1, CF_UP, -1, 1, 1, CF_UP);
            AtomCircularSwap(0, 1, -1, CF_UP, 1, 1, 0, CF_UP, 0, 1, 1, CF_UP, -1, 1, 0, CF_UP);
            break;
        }
        case FLIP_R_P:
        {
            AtomCircularSwap(1, 1, -1, CF_FRONT, 1, 1, 1, CF_UP, 1, -1, 1, CF_BACK, 1, -1, -1, CF_DOWN);
            AtomCircularSwap(1, 1, 0, CF_UP, 1, 0, 1, CF_BACK, 1, -1, 0, CF_DOWN, 1, 0, -1, CF_FRONT);
            AtomCircularSwap(1, 1, -1, CF_UP, 1, 1, 1, CF_BACK, 1, -1, 1, CF_DOWN, 1, -1, -1, CF_FRONT);

            AtomCircularSwap(1, 1, -1, CF_RIGHT, 1, 1, 1, CF_RIGHT, 1, -1, 1, CF_RIGHT, 1, -1, -1, CF_RIGHT);
            AtomCircularSwap(1, 1, 0, CF_RIGHT, 1, 0, 1, CF_RIGHT, 1, -1, 0, CF_RIGHT, 1, 0, -1, CF_RIGHT);
            break;
        }
        case FLIP_R_2:
        {
            DoFlip(FLIP_R_P, draw);
            DoFlip(FLIP_R_P, draw);
            break;
        }
        case FLIP_R_N:
        {
            AtomCircularSwap(1, 1, -1, CF_FRONT, 1, 1, 1, CF_UP, 1, -1, 1, CF_BACK, 1, -1, -1, CF_DOWN, true);
            AtomCircularSwap(1, 1, 0, CF_UP, 1, 0, 1, CF_BACK, 1, -1, 0, CF_DOWN, 1, 0, -1, CF_FRONT, true);
            AtomCircularSwap(1, 1, -1, CF_UP, 1, 1, 1, CF_BACK, 1, -1, 1, CF_DOWN, 1, -1, -1, CF_FRONT, true);

            AtomCircularSwap(1, 1, -1, CF_RIGHT, 1, 1, 1, CF_RIGHT, 1, -1, 1, CF_RIGHT, 1, -1, -1, CF_RIGHT, true);
            AtomCircularSwap(1, 1, 0, CF_RIGHT, 1, 0, 1, CF_RIGHT, 1, -1, 0, CF_RIGHT, 1, 0, -1, CF_RIGHT, true);
            break;
        }
        case FLIP_L_P:
        {
            AtomCircularSwap(-1, 1, -1, CF_FRONT, -1, 1, 1, CF_UP, -1, -1, 1, CF_BACK, -1, -1, -1, CF_DOWN, true);
            AtomCircularSwap(-1, 1, 0, CF_UP, -1, 0, 1, CF_BACK, -1, -1, 0, CF_DOWN, -1, 0, -1, CF_FRONT, true);
            AtomCircularSwap(-1, 1, -1, CF_UP, -1, 1, 1, CF_BACK, -1, -1, 1, CF_DOWN, -1, -1, -1, CF_FRONT, true);

            AtomCircularSwap(-1, 1, -1, CF_LEFT, -1, 1, 1, CF_LEFT, -1, -1, 1, CF_LEFT, -1, -1, -1, CF_LEFT, true);
            AtomCircularSwap(-1, 1, 0, CF_LEFT, -1, 0, 1, CF_LEFT, -1, -1, 0, CF_LEFT, -1, 0, -1, CF_LEFT, true);
            break;
        }
        case FLIP_L_2:
        {
            DoFlip(FLIP_L_P, draw);
            DoFlip(FLIP_L_P, draw);
            break;
        }
        case FLIP_L_N:
        {
            AtomCircularSwap(-1, 1, -1, CF_FRONT, -1, 1, 1, CF_UP, -1, -1, 1, CF_BACK, -1, -1, -1, CF_DOWN);
            AtomCircularSwap(-1, 1, 0, CF_UP, -1, 0, 1, CF_BACK, -1, -1, 0, CF_DOWN, -1, 0, -1, CF_FRONT);
            AtomCircularSwap(-1, 1, -1, CF_UP, -1, 1, 1, CF_BACK, -1, -1, 1, CF_DOWN, -1, -1, -1, CF_FRONT);

            AtomCircularSwap(-1, 1, -1, CF_LEFT, -1, 1, 1, CF_LEFT, -1, -1, 1, CF_LEFT, -1, -1, -1, CF_LEFT);
            AtomCircularSwap(-1, 1, 0, CF_LEFT, -1, 0, 1, CF_LEFT, -1, -1, 0, CF_LEFT, -1, 0, -1, CF_LEFT);
            break;
        }
        case FLIP_B_P:
        {
            AtomCircularSwap(-1, 1, 1, CF_UP, 1, 1, 1, CF_RIGHT, 1, -1, 1, CF_DOWN, -1, -1, 1, CF_LEFT, true);
            AtomCircularSwap(0, 1, 1, CF_UP, 1, 0, 1, CF_RIGHT, 0, -1, 1, CF_DOWN, -1, 0, 1, CF_LEFT, true);
            AtomCircularSwap(-1, 1, 1, CF_LEFT, 1, 1, 1, CF_UP, 1, -1, 1, CF_RIGHT, -1, -1, 1, CF_DOWN, true);

            AtomCircularSwap(-1, 1, 1, CF_BACK, 1, 1, 1, CF_BACK, 1, -1, 1, CF_BACK, -1, -1, 1, CF_BACK, true);
            AtomCircularSwap(0, 1, 1, CF_BACK, 1, 0, 1, CF_BACK, 0, -1, 1, CF_BACK, -1, 0, 1, CF_BACK, true);
            break;
        }
        case FLIP_B_2:
        {
            DoFlip(FLIP_B_P, draw);
            DoFlip(FLIP_B_P, draw);
            break;
        }
        case FLIP_B_N:
        {
            AtomCircularSwap(-1, 1, 1, CF_UP, 1, 1, 1, CF_RIGHT, 1, -1, 1, CF_DOWN, -1, -1, 1, CF_LEFT);
            AtomCircularSwap(0, 1, 1, CF_UP, 1, 0, 1, CF_RIGHT, 0, -1, 1, CF_DOWN, -1, 0, 1, CF_LEFT);
            AtomCircularSwap(-1, 1, 1, CF_LEFT, 1, 1, 1, CF_UP, 1, -1, 1, CF_RIGHT, -1, -1, 1, CF_DOWN);

            AtomCircularSwap(-1, 1, 1, CF_BACK, 1, 1, 1, CF_BACK, 1, -1, 1, CF_BACK, -1, -1, 1, CF_BACK);
            AtomCircularSwap(0, 1, 1, CF_BACK, 1, 0, 1, CF_BACK, 0, -1, 1, CF_BACK, -1, 0, 1, CF_BACK);
            break;
        }
        case FLIP_D_P:
        {
            AtomCircularSwap(-1, -1, -1, CF_FRONT, 1, -1, -1, CF_RIGHT, 1, -1, 1, CF_BACK, -1, -1, 1, CF_LEFT);
            AtomCircularSwap(0, -1, -1, CF_FRONT, 1, -1, 0, CF_RIGHT, 0, -1, 1, CF_BACK, -1, -1, 0, CF_LEFT);
            AtomCircularSwap(-1, -1, -1, CF_LEFT, 1, -1, -1, CF_FRONT, 1, -1, 1, CF_RIGHT, -1, -1, 1, CF_BACK);

            AtomCircularSwap(-1, -1, -1, CF_DOWN, 1, -1, -1, CF_DOWN, 1, -1, 1, CF_DOWN, -1, -1, 1, CF_DOWN);
            AtomCircularSwap(0, -1, -1, CF_DOWN, 1, -1, 0, CF_DOWN, 0, -1, 1, CF_DOWN, -1, -1, 0, CF_DOWN);
            break;
        }
        case FLIP_D_2:
        {
            DoFlip(FLIP_D_P, draw);
            DoFlip(FLIP_D_P, draw);
            break;
        }
        case FLIP_D_N:
        {
            AtomCircularSwap(-1, -1, -1, CF_FRONT, 1, -1, -1, CF_RIGHT, 1, -1, 1, CF_BACK, -1, -1, 1, CF_LEFT, true);
            AtomCircularSwap(0, -1, -1, CF_FRONT, 1, -1, 0, CF_RIGHT, 0, -1, 1, CF_BACK, -1, -1, 0, CF_LEFT, true);
            AtomCircularSwap(-1, -1, -1, CF_LEFT, 1, -1, -1, CF_FRONT, 1, -1, 1, CF_RIGHT, -1, -1, 1, CF_BACK, true);

            AtomCircularSwap(-1, -1, -1, CF_DOWN, 1, -1, -1, CF_DOWN, 1, -1, 1, CF_DOWN, -1, -1, 1, CF_DOWN, true);
            AtomCircularSwap(0, -1, -1, CF_DOWN, 1, -1, 0, CF_DOWN, 0, -1, 1, CF_DOWN, -1, -1, 0, CF_DOWN, true);
            break;
        }
    }

    if (draw)
        CacheCube();
}

void RubikCube::PrintOut()
{
    /*int i, j;

    cout << endl;

    for (i = 0; i < 3; i++)
    {
        cout << "   ";
        for (j = 0; j < 3; j++)
            cout << rubikColorCode[m_faceArray[CF_BACK][i][j]];
        cout << endl;
    }

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
            cout << rubikColorCode[m_faceArray[CF_LEFT][i][j]];
        for (j = 0; j < 3; j++)
            cout << rubikColorCode[m_faceArray[CF_DOWN][i][j]];
        for (j = 0; j < 3; j++)
            cout << rubikColorCode[m_faceArray[CF_RIGHT][i][j]];
        for (j = 0; j < 3; j++)
            cout << rubikColorCode[m_faceArray[CF_UP][i][j]];
        cout << endl;
    }

    for (i = 0; i < 3; i++)
    {
        cout << "   ";
        for (j = 0; j < 3; j++)
            cout << rubikColorCode[m_faceArray[CF_FRONT][i][j]];
        cout << endl;
    }*/
}

// performs flip in linearized state and returns new state
bigint RubikCube::DoLinearFlip(int move, bigint state)
{
    // how many turns do we have to do (i.e. for F2 we do 2 flips, for F3 we do 3, although its the same
    // as F-, which is 1 flip in negative direction, but this is more generic way to do it)
    int turns = move % 3 + 1;
    // which face are we about to turn
    int face = move / 3;
    bigint oldState;

    // as far as we have turns left
    while (turns)
    {
        turns--;
        // copy old state
        oldState.copy(state);
        for (int i = 0; i < 8; i++)
        {
            // if it's corner, the behaviour is different
            int isCorner = i > 3;

            // what index do we modify
            int target = flipCubeEffect[face][i] + isCorner * 12;

            // with what value (at which index) we will substitute
            int killer = flipCubeEffect[face][(i & 3) == 3 ? i - 3 : i + 1] + isCorner * 12;

            // just determines which face we will flip
            int orientationDelta = (i < 4) ? (face > 1 && face < 4) : ((face < 2) ? 0 : (2 - (i & 1)));
            state.d[target] = oldState.d[killer];
            state.d[target + 20] = oldState.d[killer + 20] + orientationDelta;

            // at last turn, strip the hash portion to 1-2 bits
            if (!turns)
                state.d[target + 20] %= 2 + isCorner;
        }
    }
    return state;
}

// retrieve inverse move to current (just inverses the offset within move group)
// (see CubeFlip enumerator)
int inverse(int move)
{
    return move + 2 - 2 * (move % 3);
}

// hashes supplied state depending on solution stage we are in
// - it will hash only those things, we are about to solve this stage, so the rest
// could be almost anything (we don't care about the rest in our stage, maybe later)
bigint RubikCube::GetStateHash(bigint &state)
{
    // in stage 1 - we hash only edges (their orientation)
    if (m_solveStage == 1)
        return bigint(state, 20, 32);

    // in stage 2 - we hash corner orientations, and middle slice edges
    if (m_solveStage == 2){
        bigint result(state, 31, 40);
        // hash middle edges including corners
        for (int e = 0; e < 12; e++)
            result.d[0] |= (state.d[e] / 8) << e;
        return result;
    }

    // in stage 3 - we hash middle, center edges, and L+R edges+corners to fix parity
    if (m_solveStage == 3)
    {
        bigint result(3);

        int base = 0;

        // middle strip (12 sides)
        for (int e = 0; e < 12; e++)
            result.d[0] |= ((state.d[e] > 7) ? 2 : (state.d[e] & 1)) << (2 * e);

        base = 12;
        // center edges
        for (int c = 0; c < 8; c++)
            result.d[1] |= ((state.d[c + base] - base) & 5) << (3 * c);

        // edge+corners in L+R sides
        for (int i = 12; i < 20; i++)
            for (int j = i + 1; j < 20; j++)
                result.d[2] ^= state.d[i] > state.d[j];

        return result;
    }

    // in stage 4 we do not hash at all - we are now heading to complete solution,
    // so we have to "hash everything" (and that does not make much sense, so return whole state)
    return bigint(state);
}

// this will return permutation for current edge (means - we have to have red on upper side
// and yellow on right side, so we append U and R together and return)
char* RubikCube::GetEdgeCode(int x, int y, int z, CubeFace frst, CubeFace scnd)
{
    CubeAtom* ca;
    char* ret = new char[3];
    ret[2] = '\0';

    // retrieve desired atom
    ca = GetAtom(x, y, z);

    // and encode inputs
    ret[0] = rubikFaceCode[colorFaceMap[ca->faces[frst]->getColor()]];
    ret[1] = rubikFaceCode[colorFaceMap[ca->faces[scnd]->getColor()]];

    return ret;
}

// this will return permutation for current corner (means - we have to have red on upper side,
// yellow on right side and green on front side, so we append U, R and F together and return)
char* RubikCube::GetCornerCode(int x, int y, int z, CubeFace frst, CubeFace scnd, CubeFace thrd)
{
    CubeAtom* ca;
    char* ret = new char[4];
    ret[3] = '\0';

    ca = GetAtom(x, y, z);

    ret[0] = rubikFaceCode[colorFaceMap[ca->faces[frst]->getColor()]];
    ret[1] = rubikFaceCode[colorFaceMap[ca->faces[scnd]->getColor()]];
    ret[2] = rubikFaceCode[colorFaceMap[ca->faces[thrd]->getColor()]];

    return ret;
}

// this will convert current state of Rubik's cube to permutation of edges
// - this is very important step, because as permutation table, the state is
// linearized, and we are able to work with it much faster
void RubikCube::ConvertToPermutationTable(std::vector<std::string> &dst)
{
    // upper edges
    dst.push_back(std::string(GetEdgeCode(0, 1, -1, CF_UP, CF_FRONT)));
    dst.push_back(std::string(GetEdgeCode(1, 1,  0, CF_UP, CF_RIGHT)));
    dst.push_back(std::string(GetEdgeCode(0, 1,  1, CF_UP, CF_BACK)));
    dst.push_back(std::string(GetEdgeCode(-1, 1, 0, CF_UP, CF_LEFT)));

    // down edges
    dst.push_back(std::string(GetEdgeCode(0, -1, -1, CF_DOWN, CF_FRONT)));
    dst.push_back(std::string(GetEdgeCode(1, -1, 0, CF_DOWN, CF_RIGHT)));
    dst.push_back(std::string(GetEdgeCode(0, -1, 1, CF_DOWN, CF_BACK)));
    dst.push_back(std::string(GetEdgeCode(-1, -1, 0, CF_DOWN, CF_LEFT)));

    // frontal side edges
    dst.push_back(std::string(GetEdgeCode(1, 0, -1, CF_FRONT, CF_RIGHT)));
    dst.push_back(std::string(GetEdgeCode(-1, 0, -1, CF_FRONT, CF_LEFT)));

    // back side edges
    dst.push_back(std::string(GetEdgeCode(1, 0, 1, CF_BACK, CF_RIGHT)));
    dst.push_back(std::string(GetEdgeCode(-1, 0, 1, CF_BACK, CF_LEFT)));


    // top corners
    dst.push_back(std::string(GetCornerCode(1,  1, -1, CF_UP, CF_FRONT, CF_RIGHT)));
    dst.push_back(std::string(GetCornerCode(1, 1, 1, CF_UP, CF_RIGHT, CF_BACK)));
    dst.push_back(std::string(GetCornerCode(-1, 1, 1, CF_UP, CF_BACK, CF_LEFT)));
    dst.push_back(std::string(GetCornerCode(-1, 1, -1, CF_UP, CF_LEFT, CF_FRONT)));

    // bottom corners
    dst.push_back(std::string(GetCornerCode(1, -1, -1, CF_DOWN, CF_RIGHT, CF_FRONT)));
    dst.push_back(std::string(GetCornerCode(-1, -1, -1, CF_DOWN, CF_FRONT, CF_LEFT)));
    dst.push_back(std::string(GetCornerCode(-1, -1, 1, CF_DOWN, CF_LEFT, CF_BACK)));
    dst.push_back(std::string(GetCornerCode(1, -1, 1, CF_DOWN, CF_BACK, CF_RIGHT)));
}

void RubikCube::Solve(std::list<CubeFlip> *target)
{
    if (!target)
        return;

    // reset solve stage
    m_solveStage = 0;

    // permutation format inspired by: http://codegolf.stackexchange.com/questions/10768/solve-rubiks-cube

    // this is the goal we would like to reach - zero transposition permutation
    std::string goal[] = { "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL", "UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR" };

    // convert current cube to permutation table
    std::vector<std::string> permTable;
    ConvertToPermutationTable(permTable);

    // just convert current state, and destination state to hashed structures
    bigint currentState(40);
    bigint goalState(40);
    std::string atom;

    for (int i = 0; i < STATE_STRING_LENGTH; i++)
    {
        goalState.d[i] = i;

        atom = permTable[i];
        // this little funky part of code will lookup the atom (means UF, UL, .. formatted permutation) in
        // goal state. If it's not found, permutate it (in case of edge atom, just switch UF to FU, etc.),
        // and verify that again - it has to be there now, otherwise it's error in preformatter, and we got
        // another equivalence group of states, thus no solution
        while ((currentState.d[i] = find(goal, goal + 20, atom) - goal) == STATE_STRING_LENGTH)
        {
            atom = atom.substr(1) + atom[0];
            currentState.d[i + STATE_STRING_LENGTH]++;
        }
    }

    queue<bigint> q;
    std::map<bigint, bigint> predecessor;
    std::map<bigint, int> direction;
    std::map<bigint, int> lastMove;

    // start five stage Thistlethwaite algorithm
    while (++m_solveStage < 5)
    {
        // hash current state and goal state
        // the hash is different in every state! It depends on what are we about to solve this stage
        // i.e. in stage 1 we hash only edges due to their orientation, etc.
        bigint currentId = GetStateHash(currentState);
        bigint goalId = GetStateHash(goalState);

        // if we are there, skip and end
        if (currentId == goalId)
            continue;

        // clear BFS queue (pop what left, if neccessarry)
        while (!q.empty())
            q.pop();

        // at the bottom, push goal state, and over it, push current state
        q.push(currentState);
        q.push(goalState);

        // init helper maps to be able to return / go forward when finding solution ("path" in state graph)
        predecessor.clear(); // map of predecessors, to determine return path
        direction.clear();   // here we will store directions of bidirectional BFS
        lastMove.clear();    // here we will store last move made on specific state - this will help us determine HOW
                             // we got from state A to state B

        // direction map (matches contents of bBFS queue)
        // 1 = go forward (towards solution)
        // 2 = go backwards (away from solution, towards current state)
        direction[currentId] = 1;
        direction[goalId] = 2;

        // run bidirectional BFS
        while (true)
        {
            // get state from queue
            bigint currState = q.front();
            q.pop();

            // compute its ID and get direction
            bigint currId = GetStateHash(currState);
            int &currDir = direction[currId];

            // try all allowed moves in specified stage
            // move types in stages are restricted using Thistletwaite's algorithm
            for (int move = FLIP_BEGIN; move < FLIP_MAX; move++)
            {
                if ((stageAllowedFlips[m_solveStage - 1] & (1 << move)) != 0)
                {
                    // flips the cube (linearized state)
                    bigint newState = DoLinearFlip(move, currState);
                    // computes new state id
                    bigint newId = GetStateHash(newState);
                    // and retrieves reference to direction record in dir map
                    int &newDir = direction[newId];

                    // if we already have been in new state, and its direction is different from current ("previous") state
                    // direction, we found a connection between forward and backward search of bidirectional BFS
                    if (newDir && newDir != currDir)
                    {
                        // swap those states to allow unified path search
                        if (currDir > 1)
                        {
                            swap(newId, currId);
                            move = inverse(move);
                        }

                        // reconstruct path using stored predecessor map and lastMove map
                        vector<int> path(1, move);
                        // forwards path
                        while (currId != currentId)
                        {
                            // insert to beginning, to have first move on head
                            path.insert(path.begin(), lastMove[currId]);
                            currId = predecessor[currId];
                        }
                        // backwards path
                        while (newId != goalId)
                        {
                            // we need to inverse the last move to get the opposite direction
                            // push to the back, so we have last move 
                            path.push_back(inverse(lastMove[newId]));
                            newId = predecessor[newId];
                        }

                        // when we have our path complete, convert it to flips and push it to solution list
                        for (int i = 0; i < (int)path.size(); i++)
                        {
                            CubeFlip cfl = (CubeFlip)((5 - path[i] / 3) * 3 + path[i] % 3);
                            target->push_back(cfl);
                            currentState = DoLinearFlip(path[i], currentState);
                        }

                        // and then jump to next stage
                        // here's the case, when goto comes in handy - instead of setting two state booleans to
                        // be able to "continue" on outer loop, we use goto to bypass these needs
                        goto nextStage;
                    }

                    // we haven't been in this state yet, so save it
                    if (!newDir)
                    {
                        // to queue
                        q.push(newState);
                        // we came from "currdir" direction (newDir is reference, so it will pass to the map also)
                        newDir = currDir;
                        // save the move we used to get there
                        lastMove[newId] = move;
                        // and save its predecessor
                        predecessor[newId] = currId;
                    }
                }
            }
        }

        // label to jump to, when we are ready for next stage of Thistletwaite algorithm
    nextStage:
        ;
    }

    //
}
