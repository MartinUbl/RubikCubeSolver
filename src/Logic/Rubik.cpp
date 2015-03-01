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
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 60 + i*20, 2 + j*20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_faceArray[CF_BACK][i][j]], false);
        }
    }

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + i * 20, 2 + 60 + j * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_faceArray[CF_LEFT][i][j]], false);
        }
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 60 + i * 20, 2 + 60 + j * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_faceArray[CF_DOWN][i][j]], false);
        }
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 2*60 + i * 20, 2 + 60 + j * 20),
                core::rect<s32>(0, 0, 20, 20), nullptr,
                rubikColorMap[m_faceArray[CF_RIGHT][i][j]], false);
        }
        for (j = 0; j < 3; j++)
        {
            sDrawing->getDriver()->draw2DImage(m_faceMiniTexture, core::position2d<s32>(2 + 3*60 + i * 20, 2 + 60 + j * 20),
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

    PrintOut();
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
