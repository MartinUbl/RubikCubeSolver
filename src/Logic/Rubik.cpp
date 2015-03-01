#include "Global.h"
#include "Rubik.h"

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
    node->setMaterialTexture(0, videoDriver->getTexture("../data/white_face.bmp"));

    CubeAtomFace* face = new CubeAtomFace();
    face->meshNode = node;

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
        atom->faces[CF_TOP]->setColor(CL_RED);
    else if (cubeOffset.Y == -1)
        atom->faces[CF_BOTTOM]->setColor(CL_WHITE);

    return atom;
}

void RubikCube::SetCubeAtom(int x, int y, int z, CubeAtom* atom)
{
    m_cubeAtoms[(x+1) + 3*(y+1) + 3*3*(z+1)] = atom;
}

CubeAtom* RubikCube::GetAtom(int x, int y, int z)
{
    return m_cubeAtoms[(x + 1) + 3 * (y + 1) + 3 * 3 * (z + 1)];
}

void RubikCube::BuildCube(ISceneManager* scene, IVideoDriver* videoDriver)
{
    meshManipulator = scene->getMeshManipulator();

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
}
