
#ifndef _Tessellate_Sample_H
#define _Tessellate_Sample_H

#include <d3dx9.h>
#include "ChapterBase.h"

#define MAX_VERTEX 1000
#define MAX_PATCHES 1000
#define VERTEX_PER_PATCH 16

class CTessellateSample : public CChapterBase
{
public:
	CTessellateSample(void);
	virtual ~CTessellateSample(void);

	CTessellateSample(LPDIRECT3DDEVICE9 pdevice);

	void draw();

	void deviceRestore();

	HRESULT initScene();

protected:
	void createMatricesTransform();

	virtual void copyVertices(LPD3DXPATCHMESH pMesh, DWORD numVertex);

	virtual DWORD getFVF();

	void generateMesh(LPD3DXPATCHMESH pMesh, LPD3DXMESH *dstMesh, FLOAT fTessLevel);

	virtual void initPatch(const std::string& fileName, float tessLevel);

	virtual void initMesh(LPD3DXPATCHMESH pMesh, FLOAT fTessLevel);

	virtual void loadResource(char *filename, int *patches, int *vertices);

	void loadPatch(int ii, int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n, int o, int p);

	virtual void loadVertex(int ii, double x, double y, double z);

	void releaseForDeviceReset();

protected:
	D3DXVECTOR3 mVertexes[MAX_VERTEX];

	WORD mPatches[MAX_PATCHES][VERTEX_PER_PATCH];

	LPD3DXMESH mMesh;
};

#endif