
#ifndef _Animation_H
#define _Animation_H

#include "TessellateSample.h"
#include "OwnMath.h"
#include "OwnTimer.h"

class CAnimation : public CTessellateSample
{
public:
	CAnimation(void);
	~CAnimation(void);

	CAnimation(LPDIRECT3DDEVICE9 pdevice);

	void draw();

	void deviceRestore();

	std::string getVSFunctionName();

	std::string getPSFunctionName();

	LPCWSTR getVSFileName() { return L"VertexShader_Animation.fx"; }

	LPCWSTR getPSFileName() { return L"PixelShader_Animation.fx"; }

	HRESULT initScene();

	void releaseForDeviceReset();

	void shaderConfig(LPD3DXCONSTANTTABLE pVtxConstTable = NULL, LPD3DXCONSTANTTABLE pPxlConstTable = NULL);

protected:
	void copyVertices(LPD3DXPATCHMESH pMesh, DWORD numVertex);

	DWORD getFVF();

	void loadResource(char *filename, int *patches, int *vertices);

	void loadVertex( int ii, float x, float y, float z, const OwnVector3& normal, const OwnVector2& texCoord);

	void shaderApply(OwnMatrix4x4 *pModelMtx = NULL);

	void twoLightShaderConfig();

protected:

	LPD3DXCONSTANTTABLE mVtxConstTable;

	LPD3DXCONSTANTTABLE mPxlConstTable;

	D3DXVECTOR3 mNormal[MAX_VERTEX];

	D3DXVECTOR2 mTexCoords[MAX_VERTEX];

	LPDIRECT3DTEXTURE9 mTextureShesays;

	OwnTimer mTimer;
};

#endif