
#ifndef _D3D9_Tutorials_H
#define _D3D9_Tutorials_H

#include <d3dx9.h>
#include "ChapterBase.h"

class CD3D9Tutorials : public CChapterBase
{
public:
	CD3D9Tutorials(void);
	~CD3D9Tutorials(void);

	CD3D9Tutorials(LPDIRECT3DDEVICE9 pdevice);

	void createLight();

	void createMatricesTransform();

	HRESULT createVertexBuff();

	void draw();

	void deviceRestore();

	HRESULT initScene();

	void releaseForDeviceReset();

private:
	LPDIRECT3DVERTEXBUFFER9 mVBTestPointShader, mVBXYZAxis, mVBTexture, mVBLight;

	LPDIRECT3DTEXTURE9 mTextureShesays;
};

#endif