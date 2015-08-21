
#ifndef _Model_Object_H
#define _Model_Object_H


#include <d3dx9.h>
#include "OwnMath.h"

struct MeshObject
{
	LPD3DXMESH mesh;
	OwnMatrix4x4 modelMtx;
};


#endif