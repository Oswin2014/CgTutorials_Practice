
#ifndef _Vertex_Transform_H
#define _Vertex_Transform_H

#include <d3dx9.h>
#include "Global.h"
#include "ChapterBase.h"

class CVertexTransform : public CChapterBase
{
public:
	CVertexTransform(void);
	~CVertexTransform(void);

	CVertexTransform(LPDIRECT3DDEVICE9 pdevice);

	void draw();

	void deviceRestore();

	std::string getVSFunctionName();

	std::string getPSFunctionName();

	HRESULT initScene();

	void releaseForDeviceReset();

	void shaderConfig(LPD3DXCONSTANTTABLE pVtxConstTable = NULL, LPD3DXCONSTANTTABLE pPxlConstTable = NULL);

private:

	VERTEX_TYPE mArrow1[8];
	VERTEX_TYPE mArrow2[8];
	VERTEX_TYPE mArrow3[6];
	VERTEX_TYPE mTriangleX[3];
	VERTEX_TYPE mTriangleZ[3];
	VERTEX_TYPE mAxis[6];
};

#endif