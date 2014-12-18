
#ifndef _Shader_Base_H
#define _Shader_Base_H

#include <d3dx9.h>
#include <string>
#include "Global.h"
#include "ChapterBase.h"

class CShaderBase : public CChapterBase
{
public:
	CShaderBase(void);
	~CShaderBase(void);

	CShaderBase(LPDIRECT3DDEVICE9 pdevice);

	void draw();

	void drawT2();

	void drawT3();

	void deviceRestore();

	std::string getVSFunctionName();
									 
	std::string getPSFunctionName();

	HRESULT initScene();

	HRESULT initSceneT2();

	HRESULT initSceneT3();

	void shaderConfig(LPD3DXCONSTANTTABLE pVtxConstTable = NULL, LPD3DXCONSTANTTABLE pPxlConstTable = NULL);

	void shaderConfigT2(LPD3DXCONSTANTTABLE pVtxConstTable = NULL, LPD3DXCONSTANTTABLE pPxlConstTable = NULL);

	void shaderConfigT3(LPD3DXCONSTANTTABLE pVtxConstTable = NULL, LPD3DXCONSTANTTABLE pPxlConstTable = NULL);

	void releaseForDeviceReset();

private:
	LPDIRECT3DTEXTURE9 mTextureShesays;

	D3DXVECTOR4 mVertexT2[3];

	VERTEX_XYZW_DIFFUSE_TEX1 mVertexT3[4];
};

#endif