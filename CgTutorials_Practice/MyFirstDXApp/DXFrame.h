
#ifndef _DX_Frame_H
#define _DX_Frame_H

#include <d3dx9.h>

#include "OwnTimer.h"
#include "OwnLog.h"

class CIntegrateModule;

class CDXFrame
{
public:
	~CDXFrame(void);

	static CDXFrame* getSingletonPtr()
	{
		if (NULL == mSingleton)
		{
			mSingleton = new CDXFrame;
		}
		return mSingleton;
	}

	void deviceRestore();

	void deviceReset(LPARAM lParam);

	HRESULT init();

	HRESULT initDevice();

	void onInput(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void render();

	void renderLoop();

	void update();

private:
	CDXFrame(void);

	static CDXFrame* mSingleton;

	LPDIRECT3D9 mD3D;

	LPDIRECT3DDEVICE9 mD3DDevice;

	D3DPRESENT_PARAMETERS mD3Dpp; 

	CIntegrateModule* mSampleModule;

	OwnTimer mTimer;

	OwnLog mLog;

	int mMaxFPS;

	f32 mRenderDelta;
};

#endif