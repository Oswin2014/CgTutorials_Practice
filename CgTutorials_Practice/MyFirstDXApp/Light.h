
#ifndef _Light_H
#define _Light_H

#include <d3dx9.h>
#include "TessellateSample.h"
#include "OwnMath.h"
#include "ModelObject.h"

//class OwnCameraNormal;
class OwnVector3;
//class OwnMatrix4x4;

class CLight : public CTessellateSample
{
public:

	CLight(void);
	~CLight(void);

	CLight(LPDIRECT3DDEVICE9 pdevice);

	void draw();

	void deviceRestore();

	std::string getVSFunctionName();

	std::string getPSFunctionName();

	HRESULT initScene();

	void releaseForDeviceReset();

	void shaderConfig(LPD3DXCONSTANTTABLE pVtxConstTable = NULL, LPD3DXCONSTANTTABLE pPxlConstTable = NULL);

protected:
	void copyVertices(LPD3DXPATCHMESH pMesh, DWORD numVertex);

	DWORD getFVF();

	void initMesh(LPD3DXPATCHMESH pMesh, FLOAT fTessLevel);

	void loadResource(char *filename, int *patches, int *vertices);

	void loadVertex( int ii, float x, float y, float z, const OwnVector3& normal );

	void singleLightShaderConfig();

	void twoLightShaderConfig();

	void attenuationLightShaderConfig();

private:
	void shaderApply(OwnMatrix4x4 *pModelMtx = NULL);

	LPD3DXCONSTANTTABLE mVtxConstTable;

	LPD3DXCONSTANTTABLE mPxlConstTable;

	D3DXVECTOR3 mNormal[MAX_VERTEX];

	LightType mLightType;

	MeshObject mLightObject[16];
};

#endif