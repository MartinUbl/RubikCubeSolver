#include "Global.h"
#include "Rubik.h"
#include "Drawing.h"

RubikCube::RubikCube()
{
    //
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

void RubikCube::Render()
{
    int i, j;

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 60 + i*20, 2 + 40 - j*20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_faceArray[CF_BACK][i][j]], false);
        }
    }

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 40 - j * 20, 2 + 60 + i * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_faceArray[CF_LEFT][i][j]], false);
        }
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 60 + i * 20, 2 + 60 + 40 - j * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_faceArray[CF_DOWN][i][j]], false);
        }
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 2*60 + j * 20, 2 + 60 + 40 - i * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_faceArray[CF_RIGHT][i][j]], false);
        }
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 3*60 + 40 - i * 20, 2 + 60 + 40 - j * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_faceArray[CF_UP][i][j]], false);
        }
    }

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 60 + i * 20, 2 + 2*60 + j * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_faceArray[CF_FRONT][i][j]], false);
        }
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

    // randomly mess the cube
    /*CubeFlip a;
    for (int i = 0; i < 50; i++)
    {
        a = (CubeFlip) (rand() % FLIP_MAX);
        DoFlip((CubeFlip)a, true);
    }*/

    DoFlip(FLIP_U_P, true);
}

void RubikCube::DoFlip(CubeFlip flip, bool draw)
{
    cout << "Flipping: " << getStrForFlip(flip) << endl;

    switch (flip)
    {
        case FLIP_F_P:
        {
            // swap frontal corners
            CircularSwap(&m_faceArray[CF_FRONT][0][2], &m_faceArray[CF_FRONT][2][2], &m_faceArray[CF_FRONT][2][0], &m_faceArray[CF_FRONT][0][0]);
            // swap frontal two-edges
            CircularSwap(&m_faceArray[CF_FRONT][1][2], &m_faceArray[CF_FRONT][2][1], &m_faceArray[CF_FRONT][1][0], &m_faceArray[CF_FRONT][0][1]);

            // swap the sides
            CircularSwap(&m_faceArray[CF_UP][0][0], &m_faceArray[CF_RIGHT][0][2], &m_faceArray[CF_DOWN][0][0], &m_faceArray[CF_LEFT][2][0]);
            CircularSwap(&m_faceArray[CF_UP][1][0], &m_faceArray[CF_RIGHT][0][1], &m_faceArray[CF_DOWN][1][0], &m_faceArray[CF_LEFT][2][1]);
            CircularSwap(&m_faceArray[CF_UP][2][0], &m_faceArray[CF_RIGHT][0][0], &m_faceArray[CF_DOWN][2][0], &m_faceArray[CF_LEFT][2][2]);
            break;
        }
        case FLIP_F_N:
        {
            ReverseCircularSwap(&m_faceArray[CF_FRONT][0][2], &m_faceArray[CF_FRONT][2][2], &m_faceArray[CF_FRONT][2][0], &m_faceArray[CF_FRONT][0][0]);
            ReverseCircularSwap(&m_faceArray[CF_FRONT][1][2], &m_faceArray[CF_FRONT][2][1], &m_faceArray[CF_FRONT][1][0], &m_faceArray[CF_FRONT][0][1]);

            ReverseCircularSwap(&m_faceArray[CF_UP][0][0], &m_faceArray[CF_RIGHT][0][2], &m_faceArray[CF_DOWN][0][0], &m_faceArray[CF_LEFT][2][0]);
            ReverseCircularSwap(&m_faceArray[CF_UP][1][0], &m_faceArray[CF_RIGHT][0][1], &m_faceArray[CF_DOWN][1][0], &m_faceArray[CF_LEFT][2][1]);
            ReverseCircularSwap(&m_faceArray[CF_UP][2][0], &m_faceArray[CF_RIGHT][0][0], &m_faceArray[CF_DOWN][2][0], &m_faceArray[CF_LEFT][2][2]);
            break;
        }
        case FLIP_U_P:
        {
            CircularSwap(&m_faceArray[CF_UP][0][2], &m_faceArray[CF_UP][2][2], &m_faceArray[CF_UP][2][0], &m_faceArray[CF_UP][0][0]);
            CircularSwap(&m_faceArray[CF_UP][1][2], &m_faceArray[CF_UP][2][1], &m_faceArray[CF_UP][1][0], &m_faceArray[CF_UP][0][1]);

            CircularSwap(&m_faceArray[CF_FRONT][0][2], &m_faceArray[CF_LEFT][0][2], &m_faceArray[CF_BACK][2][2], &m_faceArray[CF_RIGHT][0][2]);
            CircularSwap(&m_faceArray[CF_FRONT][1][2], &m_faceArray[CF_LEFT][1][2], &m_faceArray[CF_BACK][1][2], &m_faceArray[CF_RIGHT][1][2]);
            CircularSwap(&m_faceArray[CF_FRONT][2][2], &m_faceArray[CF_LEFT][2][2], &m_faceArray[CF_BACK][0][2], &m_faceArray[CF_RIGHT][2][2]);
            break;
        }
        case FLIP_U_N:
        {
            ReverseCircularSwap(&m_faceArray[CF_UP][0][2], &m_faceArray[CF_UP][2][2], &m_faceArray[CF_UP][2][0], &m_faceArray[CF_UP][0][0]);
            ReverseCircularSwap(&m_faceArray[CF_UP][1][2], &m_faceArray[CF_UP][2][1], &m_faceArray[CF_UP][1][0], &m_faceArray[CF_UP][0][1]);

            ReverseCircularSwap(&m_faceArray[CF_FRONT][0][2], &m_faceArray[CF_LEFT][0][2], &m_faceArray[CF_BACK][2][2], &m_faceArray[CF_RIGHT][0][2]);
            ReverseCircularSwap(&m_faceArray[CF_FRONT][1][2], &m_faceArray[CF_LEFT][1][2], &m_faceArray[CF_BACK][1][2], &m_faceArray[CF_RIGHT][1][2]);
            ReverseCircularSwap(&m_faceArray[CF_FRONT][2][2], &m_faceArray[CF_LEFT][2][2], &m_faceArray[CF_BACK][0][2], &m_faceArray[CF_RIGHT][2][2]);
            break;
        }
        case FLIP_R_P:
        {
            CircularSwap(&m_faceArray[CF_RIGHT][0][2], &m_faceArray[CF_RIGHT][2][2], &m_faceArray[CF_RIGHT][2][0], &m_faceArray[CF_RIGHT][0][0]);
            CircularSwap(&m_faceArray[CF_RIGHT][1][2], &m_faceArray[CF_RIGHT][2][1], &m_faceArray[CF_RIGHT][1][0], &m_faceArray[CF_RIGHT][0][1]);

            CircularSwap(&m_faceArray[CF_FRONT][2][0], &m_faceArray[CF_UP][2][0], &m_faceArray[CF_BACK][2][2], &m_faceArray[CF_DOWN][2][2]);
            CircularSwap(&m_faceArray[CF_FRONT][2][1], &m_faceArray[CF_UP][2][1], &m_faceArray[CF_BACK][2][1], &m_faceArray[CF_DOWN][2][1]);
            CircularSwap(&m_faceArray[CF_FRONT][2][2], &m_faceArray[CF_UP][2][2], &m_faceArray[CF_BACK][2][0], &m_faceArray[CF_DOWN][2][0]);
            break;
        }
        case FLIP_R_N:
        {
            ReverseCircularSwap(&m_faceArray[CF_RIGHT][0][2], &m_faceArray[CF_RIGHT][2][2], &m_faceArray[CF_RIGHT][2][0], &m_faceArray[CF_RIGHT][0][0]);
            ReverseCircularSwap(&m_faceArray[CF_RIGHT][1][2], &m_faceArray[CF_RIGHT][2][1], &m_faceArray[CF_RIGHT][1][0], &m_faceArray[CF_RIGHT][0][1]);

            ReverseCircularSwap(&m_faceArray[CF_FRONT][2][0], &m_faceArray[CF_UP][2][0], &m_faceArray[CF_BACK][2][2], &m_faceArray[CF_DOWN][2][2]);
            ReverseCircularSwap(&m_faceArray[CF_FRONT][2][1], &m_faceArray[CF_UP][2][1], &m_faceArray[CF_BACK][2][1], &m_faceArray[CF_DOWN][2][1]);
            ReverseCircularSwap(&m_faceArray[CF_FRONT][2][2], &m_faceArray[CF_UP][2][2], &m_faceArray[CF_BACK][2][0], &m_faceArray[CF_DOWN][2][0]);
            break;
        }
        case FLIP_L_P:
        {
            CircularSwap(&m_faceArray[CF_LEFT][2][2], &m_faceArray[CF_LEFT][0][2], &m_faceArray[CF_LEFT][0][0], &m_faceArray[CF_LEFT][2][0]);
            CircularSwap(&m_faceArray[CF_LEFT][1][2], &m_faceArray[CF_LEFT][0][1], &m_faceArray[CF_LEFT][1][0], &m_faceArray[CF_LEFT][2][1]);

            CircularSwap(&m_faceArray[CF_UP][0][2], &m_faceArray[CF_FRONT][0][2], &m_faceArray[CF_DOWN][0][2], &m_faceArray[CF_BACK][0][2]);
            CircularSwap(&m_faceArray[CF_UP][0][1], &m_faceArray[CF_FRONT][0][1], &m_faceArray[CF_DOWN][0][1], &m_faceArray[CF_BACK][0][1]);
            CircularSwap(&m_faceArray[CF_UP][0][0], &m_faceArray[CF_FRONT][0][0], &m_faceArray[CF_DOWN][0][0], &m_faceArray[CF_BACK][0][0]);
            break;
        }
        case FLIP_L_N:
        {
            ReverseCircularSwap(&m_faceArray[CF_LEFT][2][2], &m_faceArray[CF_LEFT][0][2], &m_faceArray[CF_LEFT][0][0], &m_faceArray[CF_LEFT][2][0]);
            ReverseCircularSwap(&m_faceArray[CF_LEFT][1][2], &m_faceArray[CF_LEFT][0][1], &m_faceArray[CF_LEFT][1][0], &m_faceArray[CF_LEFT][2][1]);

            ReverseCircularSwap(&m_faceArray[CF_UP][0][2], &m_faceArray[CF_FRONT][0][2], &m_faceArray[CF_DOWN][0][2], &m_faceArray[CF_BACK][0][2]);
            ReverseCircularSwap(&m_faceArray[CF_UP][0][1], &m_faceArray[CF_FRONT][0][1], &m_faceArray[CF_DOWN][0][1], &m_faceArray[CF_BACK][0][1]);
            ReverseCircularSwap(&m_faceArray[CF_UP][0][0], &m_faceArray[CF_FRONT][0][0], &m_faceArray[CF_DOWN][0][0], &m_faceArray[CF_BACK][0][0]);
            break;
        }
        case FLIP_B_P:
        {
            CircularSwap(&m_faceArray[CF_BACK][2][2], &m_faceArray[CF_BACK][0][2], &m_faceArray[CF_BACK][0][0], &m_faceArray[CF_BACK][2][0]);
            CircularSwap(&m_faceArray[CF_BACK][1][2], &m_faceArray[CF_BACK][0][1], &m_faceArray[CF_BACK][1][0], &m_faceArray[CF_BACK][2][1]);

            CircularSwap(&m_faceArray[CF_UP][2][2], &m_faceArray[CF_LEFT][0][2], &m_faceArray[CF_DOWN][2][2], &m_faceArray[CF_RIGHT][2][2]);
            CircularSwap(&m_faceArray[CF_UP][1][2], &m_faceArray[CF_LEFT][0][1], &m_faceArray[CF_DOWN][1][2], &m_faceArray[CF_RIGHT][2][1]);
            CircularSwap(&m_faceArray[CF_UP][0][2], &m_faceArray[CF_LEFT][0][0], &m_faceArray[CF_DOWN][0][2], &m_faceArray[CF_RIGHT][2][0]);
            break;
        }
        case FLIP_B_N:
        {
            ReverseCircularSwap(&m_faceArray[CF_BACK][2][2], &m_faceArray[CF_BACK][0][2], &m_faceArray[CF_BACK][0][0], &m_faceArray[CF_BACK][2][0]);
            ReverseCircularSwap(&m_faceArray[CF_BACK][1][2], &m_faceArray[CF_BACK][0][1], &m_faceArray[CF_BACK][1][0], &m_faceArray[CF_BACK][2][1]);

            ReverseCircularSwap(&m_faceArray[CF_UP][2][2], &m_faceArray[CF_LEFT][0][2], &m_faceArray[CF_DOWN][2][2], &m_faceArray[CF_RIGHT][2][2]);
            ReverseCircularSwap(&m_faceArray[CF_UP][1][2], &m_faceArray[CF_LEFT][0][1], &m_faceArray[CF_DOWN][1][2], &m_faceArray[CF_RIGHT][2][1]);
            ReverseCircularSwap(&m_faceArray[CF_UP][0][2], &m_faceArray[CF_LEFT][0][0], &m_faceArray[CF_DOWN][0][2], &m_faceArray[CF_RIGHT][2][0]);
            break;
        }
        case FLIP_D_P:
        {
            CircularSwap(&m_faceArray[CF_DOWN][0][0], &m_faceArray[CF_DOWN][2][0], &m_faceArray[CF_DOWN][2][2], &m_faceArray[CF_DOWN][0][2]);
            CircularSwap(&m_faceArray[CF_DOWN][1][0], &m_faceArray[CF_DOWN][2][1], &m_faceArray[CF_DOWN][1][2], &m_faceArray[CF_DOWN][0][1]);

            CircularSwap(&m_faceArray[CF_FRONT][0][0], &m_faceArray[CF_RIGHT][0][0], &m_faceArray[CF_BACK][2][0], &m_faceArray[CF_LEFT][0][0]);
            CircularSwap(&m_faceArray[CF_FRONT][1][0], &m_faceArray[CF_RIGHT][1][0], &m_faceArray[CF_BACK][1][0], &m_faceArray[CF_LEFT][1][0]);
            CircularSwap(&m_faceArray[CF_FRONT][2][0], &m_faceArray[CF_RIGHT][2][0], &m_faceArray[CF_BACK][0][0], &m_faceArray[CF_LEFT][2][0]);

            break;
        }
        case FLIP_D_N:
        {
            ReverseCircularSwap(&m_faceArray[CF_DOWN][0][0], &m_faceArray[CF_DOWN][2][0], &m_faceArray[CF_DOWN][2][2], &m_faceArray[CF_DOWN][0][2]);
            ReverseCircularSwap(&m_faceArray[CF_DOWN][1][0], &m_faceArray[CF_DOWN][2][1], &m_faceArray[CF_DOWN][1][2], &m_faceArray[CF_DOWN][0][1]);

            ReverseCircularSwap(&m_faceArray[CF_FRONT][0][0], &m_faceArray[CF_RIGHT][0][0], &m_faceArray[CF_BACK][2][0], &m_faceArray[CF_LEFT][0][0]);
            ReverseCircularSwap(&m_faceArray[CF_FRONT][1][0], &m_faceArray[CF_RIGHT][1][0], &m_faceArray[CF_BACK][1][0], &m_faceArray[CF_LEFT][1][0]);
            ReverseCircularSwap(&m_faceArray[CF_FRONT][2][0], &m_faceArray[CF_RIGHT][2][0], &m_faceArray[CF_BACK][0][0], &m_faceArray[CF_LEFT][2][0]);

            break;
        }
    }

    PrintOut();

    if (draw)
        RestoreCubeCache();
}

void RubikCube::CacheCube()
{
    CubeAtom* tmp;

    for (int ix = -1; ix <= 1; ix++)
    {
        for (int iy = -1; iy <= 1; iy++)
        {
            for (int iz = -1; iz <= 1; iz++)
            {
                tmp = GetAtom(ix, iy, iz);

                if (ix == 1)
                    m_faceArray[CF_RIGHT][iz + 1][iy + 1] = tmp->faces[CF_RIGHT]->getColor();
                else if (ix == -1)
                    m_faceArray[CF_LEFT][1 - iz][iy + 1] = tmp->faces[CF_LEFT]->getColor();

                if (iz == -1)
                    m_faceArray[CF_FRONT][ix + 1][iy + 1] = tmp->faces[CF_FRONT]->getColor();
                else if (iz == 1)
                    m_faceArray[CF_BACK][ix + 1][iy + 1] = tmp->faces[CF_BACK]->getColor();

                if (iy == 1)
                    m_faceArray[CF_UP][ix + 1][iz + 1] = tmp->faces[CF_UP]->getColor();
                else if (iy == -1)
                    m_faceArray[CF_DOWN][ix + 1][iz + 1] = tmp->faces[CF_DOWN]->getColor();
            }
        }
    }
}

void RubikCube::RestoreCubeCache()
{
    CubeAtom* tmp;

    for (int ix = -1; ix <= 1; ix++)
    {
        for (int iy = -1; iy <= 1; iy++)
        {
            for (int iz = -1; iz <= 1; iz++)
            {
                tmp = GetAtom(ix, iy, iz);

                if (ix == 1)
                    tmp->faces[CF_RIGHT]->setColor(m_faceArray[CF_RIGHT][iz + 1][iy + 1]);
                else if (ix == -1)
                    tmp->faces[CF_LEFT]->setColor(m_faceArray[CF_LEFT][1 - iz][iy + 1]);

                if (iz == -1)
                    tmp->faces[CF_FRONT]->setColor(m_faceArray[CF_FRONT][ix + 1][iy + 1]);
                else if (iz == 1)
                    tmp->faces[CF_BACK]->setColor(m_faceArray[CF_BACK][ix + 1][iy + 1]);

                if (iy == 1)
                    tmp->faces[CF_UP]->setColor(m_faceArray[CF_UP][ix + 1][iz + 1]);
                else if (iy == -1)
                    tmp->faces[CF_DOWN]->setColor(m_faceArray[CF_DOWN][ix + 1][iz + 1]);
            }
        }
    }
}

void RubikCube::PrintOut()
{
    int i, j;

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
    }
}

bool RubikCube::IsSolved()
{
    RubikColor rc;

    for (int i = CF_BEGIN; i < CF_END; i++)
    {
        // get reference color
        rc = m_faceArray[i][0][0];

        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                if (m_faceArray[i][r][c] != rc)
                    return false;
            }
        }
    }

    return true;
}

void RubikCube::CircularSwap(RubikColor *cl1, RubikColor *cl2, RubikColor *cl3, RubikColor *cl4)
{
    RubikColor tmp, tmp2;

    tmp = *cl2;
    *cl2 = *cl1;
    tmp2 = *cl3;
    *cl3 = tmp;
    tmp = *cl4;
    *cl4 = tmp2;
    *cl1 = tmp;
}

void RubikCube::ReverseCircularSwap(RubikColor *cl1, RubikColor *cl2, RubikColor *cl3, RubikColor *cl4)
{
    RubikColor tmp, tmp2;

    tmp = *cl3;
    *cl3 = *cl4;
    tmp2 = *cl2;
    *cl2 = tmp;
    tmp = *cl1;
    *cl1 = tmp2;
    *cl4 = tmp;
}
