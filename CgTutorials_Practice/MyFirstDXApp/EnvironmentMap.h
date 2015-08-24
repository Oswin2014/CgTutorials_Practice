
#ifndef _Environment_Map_H
#define _Environment_Map_H

#include "Animation.h"
#include "OwnMath.h"
#include "OwnTimer.h"
#include "ModelObject.h"
#include "OwnCamera.h"



class CEnvironmentMap : public CAnimation
{
public:
	CEnvironmentMap();
	~CEnvironmentMap();

	CEnvironmentMap(LPDIRECT3DDEVICE9 pdevice);

	void draw();

	void draw(bool bRenderEnvMappedMesh);

	void deviceRestore();

	std::string getVSFunctionName();

	std::string getPSFunctionName();

	LPCWSTR getVSFileName() { return L"VertexShader_EnvironmentMap.fx"; }

	LPCWSTR getPSFileName() { return L"PixelShader_EnvironmentMap.fx"; }

	HRESULT initScene();

	void releaseForDeviceReset();

	void shaderConfig(LPD3DXCONSTANTTABLE pVtxConstTable = NULL, LPD3DXCONSTANTTABLE pPxlConstTable = NULL);

protected:

	HRESULT createVertexBuff();

	DWORD getFVF();

	void initMesh(LPD3DXPATCHMESH pMesh, FLOAT fTessLevel);

	void lightShaderConfig();

	void modelShaderApply(OwnMatrix4x4 *pModelMtx = NULL, bool bRenderEnvMappedMesh = false);

	void renderSceneIntoCubeMap(IDirect3DDevice9* pdevice);

	void renderPrevious();

	void shaderApply();

	void setCubeMapCamera(OwnCameraQuaternion* camera, int face);

private:

	static const int s_ObjCount = 9;

	MeshObject mEnvironmentObj[s_ObjCount];

	LPDIRECT3DVERTEXBUFFER9 mVBQuad;

	IDirect3DCubeTexture9* mCubeMap;

	IDirect3DSurface9* mDepthCube;

};


#endif