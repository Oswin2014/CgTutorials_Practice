
#ifndef _Integrate_Module_H
#define _Integrate_Module_H

#include <d3dx9.h>
#include <string>

class CChapterBase;
class OwnCamera;
class OwnVector3;

class CIntegrateModule
{
public:
	CIntegrateModule(LPDIRECT3DDEVICE9 pdevice);
	~CIntegrateModule(void);

	void cameraCtrl(OwnVector3 *pDeltaPos = NULL, OwnVector3 *pDeltaRot = NULL, OwnVector3 *pDeltaOrbitRot = NULL);

	void draw();

	void deviceRestore();

	std::string getVSFunctionName();

	std::string getPSFunctionName();

	HRESULT initScene();

	HRESULT loadShader();

	void onInput(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void releaseForDeviceReset();

	void renderPrevious();

private:
	LPDIRECT3DDEVICE9 mD3DDevice;

	IDirect3DVertexShader9 *mVertexShader;
	IDirect3DPixelShader9  *mPixelShader;

	LPD3DXCONSTANTTABLE mVtxConstTable;
	LPD3DXCONSTANTTABLE mPxlConstTable;

	CChapterBase *mChapterBase;

	OwnCamera *mCamera;
};

#endif