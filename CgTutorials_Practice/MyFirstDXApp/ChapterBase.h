
#ifndef _Chapter_Base_H
#define _Chapter_Base_H

#include <d3dx9.h>
#include <string>

class OwnCamera;

class CChapterBase
{
public:
	CChapterBase(void);
	virtual ~CChapterBase(void);

	CChapterBase(LPDIRECT3DDEVICE9 pdevice);

	virtual void draw() = 0;

	virtual void deviceRestore() = 0;

	virtual std::string getVSFunctionName() { return ""; }

	virtual std::string getPSFunctionName() { return ""; }

	virtual LPCWSTR getVSFileName() { return L"VertexShader.fx"; }

	virtual LPCWSTR getPSFileName() { return L"PixelShader.fx"; }

	OwnCamera* getCamera() { return mCamera; };

	virtual HRESULT initScene() = 0;

	virtual void releaseForDeviceReset() = 0;

	virtual void renderPrevious() {};

	virtual void shaderConfig(LPD3DXCONSTANTTABLE pVtxConstTable = NULL, LPD3DXCONSTANTTABLE pPxlConstTable = NULL) {}

	virtual bool shaderUse(){ return mShaderUse; }

protected:
	LPDIRECT3DDEVICE9 mD3DDevice;

	bool mShaderUse;

	OwnCamera *mCamera;
};

#endif