#include "DXFrame.h"
#include "WinMain.h"
#include "IntegrateModule.h"
#include "Global.h"

CDXFrame* CDXFrame::mSingleton = NULL;

//--------------------------------------------------------------------------------------
CDXFrame::CDXFrame(void)
	:mSampleModule(NULL)
	, mD3D(NULL)
	, mD3DDevice(NULL)
	, mMaxFPS(30)
	, mRenderDelta(0)
	, mLog("Own.log")
{
	mTimer.timeStart();
}
//--------------------------------------------------------------------------------------
CDXFrame::~CDXFrame(void)
{
	SAFE_RELEASE(mD3D);
	SAFE_RELEASE(mD3DDevice);

	SAFE_DELETE(mSampleModule);
}
//--------------------------------------------------------------------------------------
HRESULT CDXFrame::init()
{
	if( FAILED( initDevice() ) )
		return E_FAIL;

	mSampleModule = new CIntegrateModule(mD3DDevice);
	
	if( FAILED( mSampleModule->initScene() ) )
		return E_FAIL;
}
//--------------------------------------------------------------------------------------
HRESULT CDXFrame::initDevice()
{
	if( NULL == ( mD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	ZeroMemory( &mD3Dpp, sizeof(mD3Dpp) );
	mD3Dpp.Windowed = TRUE;
	mD3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	mD3Dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	mD3Dpp.EnableAutoDepthStencil = TRUE;
	mD3Dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	if( FAILED( mD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, CWinMain::getSingletonPtr()->getHWnd(),
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&mD3Dpp, &mD3DDevice ) ) )
	{
		return E_FAIL;
	}

	D3DVIEWPORT9 view_port;
	view_port.X = view_port.Y = static_cast<DWORD>(0);
	view_port.Width = SCREEN_WIDTH;
	view_port.Height = SCREEN_HEIGHT;
	view_port.MinZ = 0;
	view_port.MaxZ = 1;
	mD3DDevice->SetViewport(&view_port);


	return S_OK;
}
//--------------------------------------------------------------------------------------
void CDXFrame::render()
{
	mD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR( 0.0f, 0.25f, 0.25f, 0.55f ), 1.0f, 0 );

	// Begin the scene
	mD3DDevice->BeginScene();

	mSampleModule->draw();

	// End the scene
	mD3DDevice->EndScene();

	mD3DDevice->Present( NULL, NULL, NULL, NULL );
}
//-----------------------------------------------------------------------
void CDXFrame::onInput( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(WM_KEYDOWN)
	{
		switch(wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
		}
	}

	if(NULL != mSampleModule)
		mSampleModule->onInput(uMsg, wParam, lParam);
}
//-----------------------------------------------------------------------
void CDXFrame::deviceRestore()
{
	mSampleModule->deviceRestore();
}
//-----------------------------------------------------------------------
void CDXFrame::deviceReset(LPARAM lParam)
{
	if(!mD3DDevice || !mSampleModule)
		return;

	mSampleModule->releaseForDeviceReset();

	mD3Dpp.BackBufferWidth = LOWORD(lParam);
	mD3Dpp.BackBufferHeight = HIWORD(lParam);
	HRESULT hr = mD3DDevice->Reset(&mD3Dpp);

	if( hr == D3DERR_INVALIDCALL )
	{
		MessageBox( NULL, L"Call to Reset() failed with D3DERR_INVALIDCALL! ",
			L"ERROR", MB_OK | MB_ICONEXCLAMATION );
	}

	mSampleModule->deviceRestore();
}
//-----------------------------------------------------------------------
void CDXFrame::renderLoop()
{
	f32 lastUsec = mTimer.mUsec;

	mTimer.timeEnd();

// 	char buf[80];
// 	sprintf_s(buf, "mRenderDelta = %f, now = %f, last = %f", mRenderDelta, mTimer.mUsec, lastUsec);
// 	mLog.logMessage(buf);

	mRenderDelta += mTimer.mUsec - lastUsec;

	f32 targetDelta;
	if(mMaxFPS > 0)
		targetDelta = 1000000 / mMaxFPS;
	else
		targetDelta = 1000000;

	if(mRenderDelta >= targetDelta)
	{
		mRenderDelta = (int)mRenderDelta % (int)targetDelta;

		//mLog.logMessage("render");
		update();
		render();
	}
}
//-----------------------------------------------------------------------
void CDXFrame::update()
{

}
//--------------------------------------------------------------------------------------
