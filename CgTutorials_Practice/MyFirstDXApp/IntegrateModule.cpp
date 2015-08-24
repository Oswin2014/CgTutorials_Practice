#include "IntegrateModule.h"

#include <windowsx.h>

#include "OwnMath.h"
#include "OwnCamera.h"
#include "Global.h"
#include "D3D9Tutorials.h"
#include "ShaderBase.h"
#include "VertexTransform.h"
#include "TessellateSample.h"
#include "Light.h"
#include "Animation.h"
#include "EnvironmentMap.h"

//--------------------------------------------------------------------------------------
CIntegrateModule::CIntegrateModule(LPDIRECT3DDEVICE9 pdevice)
	: mVertexShader(NULL)
	, mPixelShader(NULL)
	, mVtxConstTable(NULL)
	, mPxlConstTable(NULL)
	, mChapterBase(NULL)
	, mCamera(NULL)
{
	mD3DDevice = pdevice;

	//mChapterBase = new CD3D9Tutorials(pdevice);

	//mChapterBase = new CShaderBase(pdevice);

	//mChapterBase = new CVertexTransform(pdevice);

	//mChapterBase = new CTessellateSample(pdevice);

	//mChapterBase = new CLight(pdevice);

	//mChapterBase = new CAnimation(pdevice);

	mChapterBase = new CEnvironmentMap(pdevice);

	mCamera = mChapterBase->getCamera();
}
//-----------------------------------------------------------------------
CIntegrateModule::~CIntegrateModule(void)
{
	SAFE_DELETE(mChapterBase);

	SAFE_RELEASE(mVertexShader);
	SAFE_RELEASE(mPixelShader);

	SAFE_RELEASE(mVtxConstTable);
	SAFE_RELEASE(mPxlConstTable);
}
//-----------------------------------------------------------------------
HRESULT CIntegrateModule::initScene()
{
	if(NULL != mChapterBase && mChapterBase->shaderUse())
	{
		loadShader();
		mD3DDevice->SetVertexShader(mVertexShader);
		mD3DDevice->SetPixelShader(mPixelShader);
	}

	if(NULL != mChapterBase)
		mChapterBase->initScene();

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CIntegrateModule::loadShader()
{
#pragma region shader load
	HANDLE hFile = CreateFile( /*L"VertexShader.fx"*/mChapterBase->getVSFileName(), GENERIC_READ, FILE_SHARE_READ, NULL,		//..\\..\\Media\\VertexShader.txt
		OPEN_EXISTING, 0, NULL);
	
	DWORD dVFileSize = 1;
	DWORD dwBytesRead = 0;

	char *pVertexFileData;
	if( INVALID_HANDLE_VALUE != hFile )
	{
		dVFileSize = GetFileSize( hFile, NULL );
		pVertexFileData = (char *)malloc(dVFileSize);
		ReadFile( hFile, pVertexFileData, dVFileSize, &dwBytesRead, NULL);
		CloseHandle( hFile );
	}

	if( dwBytesRead != dVFileSize )
		return E_FAIL;

	hFile = CreateFile( /*L"PixelShader.fx"*/mChapterBase->getPSFileName(), GENERIC_READ, FILE_SHARE_READ, NULL,		//..\\..\\Media\\PixelShader.txt
		OPEN_EXISTING, 0, NULL);

	DWORD dPFileSize = 1;
	dwBytesRead = 0;

	char *pPixelFileData;
	if( INVALID_HANDLE_VALUE != hFile )
	{
		dPFileSize = GetFileSize( hFile, NULL );
		pPixelFileData = (char *)malloc(dPFileSize);
		ReadFile( hFile, pPixelFileData, dPFileSize, &dwBytesRead, NULL);
		CloseHandle( hFile );
	}

	if( dwBytesRead != dPFileSize )
		return E_FAIL;


	LPD3DXBUFFER pErrorMsgs = NULL;
	LPD3DXBUFFER pVShaderBuf;
	LPD3DXBUFFER pPShaderBuf;

	HRESULT hr = D3DXCompileShader( pVertexFileData, dVFileSize, NULL, NULL, getVSFunctionName().c_str(), "vs_3_0", 
		D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION,		//C4E1_Transform C3E5v_twoTextures C2E1v_green
					&pVShaderBuf, &pErrorMsgs, &mVtxConstTable);

	char *pMsg = NULL;
	if(  pErrorMsgs)
	{
		pMsg = (char *)pErrorMsgs->GetBufferPointer();
		return E_FAIL;
	}
	if( FAILED(hr) )
	{
	}

	hr = D3DXCompileShader( pPixelFileData, dPFileSize, NULL, NULL,getPSFunctionName().c_str(), "ps_3_0", 
		D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION,	//C2E2f_passthrough C3E6f_twoTextures 
		&pPShaderBuf, &pErrorMsgs, &mPxlConstTable);

	if(  pErrorMsgs)
	{
		pMsg = (char *)pErrorMsgs->GetBufferPointer();
		return E_FAIL;
	}
	if( FAILED(hr) )
	{
	}

	hr = mD3DDevice->CreateVertexShader( static_cast<DWORD*>(pVShaderBuf->GetBufferPointer()), &mVertexShader );
	if (FAILED(hr))
	{
		std::string errMsg = "Cannot create D3D9 vertex shader from microcode ";
		switch (hr)
		{
		case D3DERR_INVALIDCALL:
			errMsg += "(D3DERR_INVALIDCALL)";
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			errMsg += "(D3DERR_OUTOFVIDEOMEMORY)";
			break;
		case E_OUTOFMEMORY:
			errMsg += "(E_OUTOFMEMORY)";
			break;
		default:
			break;
		}
		return E_FAIL;
	}

	hr = mD3DDevice->CreatePixelShader( static_cast<DWORD*>(pPShaderBuf->GetBufferPointer()), &mPixelShader );
	if (FAILED(hr))
	{
		std::string errMsg = "Cannot create D3D9 vertex shader from microcode ";
		switch (hr)
		{
		case D3DERR_INVALIDCALL:
			errMsg += "(D3DERR_INVALIDCALL)";
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			errMsg += "(D3DERR_OUTOFVIDEOMEMORY)";
			break;
		case E_OUTOFMEMORY:
			errMsg += "(E_OUTOFMEMORY)";
			break;
		default:
			break;
		}
		return E_FAIL;
	}

#pragma endregion shader load

// 	LPD3DXCONSTANTTABLE pVtxConstTable;
// 	hr = D3DXGetShaderConstantTable( (DWORD *)pVShaderBuf->GetBufferPointer(), &pVtxConstTable );
// 
// 	LPD3DXCONSTANTTABLE pPxlConstTable;
// 	hr = D3DXGetShaderConstantTable( (DWORD *)pPShaderBuf->GetBufferPointer(), &pPxlConstTable );

	if(NULL != mChapterBase)
		mChapterBase->shaderConfig(mVtxConstTable, mPxlConstTable);

	SAFE_RELEASE(pErrorMsgs);
	SAFE_RELEASE(pVShaderBuf);
	SAFE_RELEASE(pPShaderBuf);
}
//-----------------------------------------------------------------------
void CIntegrateModule::draw()
{
	//The default value for this render state is D3DZB_TRUE if a depth stencil was created along with 
	//the swap chain by setting the EnableAutoDepthStencil member of the D3DPRESENT_PARAMETERS structure to TRUE, 
	//and D3DZB_FALSE otherwise.
	// 	mD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);		
	// 	mD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	// 	mD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL) ;

	mD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	if(NULL != mChapterBase)
	{
		mChapterBase->draw();
	}

	// 	mD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	// 	mD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}
//-----------------------------------------------------------------------
std::string CIntegrateModule::getVSFunctionName()
{
	if(NULL != mChapterBase && mChapterBase->shaderUse())
		return mChapterBase->getVSFunctionName();
}
//-----------------------------------------------------------------------
std::string CIntegrateModule::getPSFunctionName()
{
	if(NULL != mChapterBase && mChapterBase->shaderUse())
		return mChapterBase->getPSFunctionName();
}
//-----------------------------------------------------------------------
void CIntegrateModule::onInput( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	f32 camRotOffset = 0.05f, camMoveOffset = 0.5f;
	OwnVector3 *pDeltaPos = NULL, *pDeltaRot = NULL, *pDeltaOrbit = NULL;
	OwnVector3 deltaPos, deltaRot, deltaOrbit;

	static struct 
	{
		int btnType;	//0 for null, 1 for move, 2 for rotate dir, 3 for rotate by lookat
		OwnVector2 mousePos;
	} s_MouseState;

	if(WM_LBUTTONDOWN == uMsg ||
		WM_RBUTTONDOWN == uMsg ||
		WM_MBUTTONDOWN == uMsg)
	{
		if( ( GET_KEYSTATE_WPARAM(wParam) & MK_CONTROL ) && WM_LBUTTONDOWN == uMsg ||
			WM_MBUTTONDOWN == uMsg)
			s_MouseState.btnType = 3;
		else if(WM_LBUTTONDOWN == uMsg)
			s_MouseState.btnType = 1;
		else if(WM_RBUTTONDOWN == uMsg)
			s_MouseState.btnType = 2;

		s_MouseState.mousePos.m_x = GET_X_LPARAM(lParam);
		s_MouseState.mousePos.m_y = GET_Y_LPARAM(lParam);
	}
	else if(WM_LBUTTONUP == uMsg ||
		WM_RBUTTONUP == uMsg ||
		WM_MBUTTONUP == uMsg ||
		WM_MOUSELEAVE == uMsg)
	{
		s_MouseState.btnType = 0;
	}
	else if(WM_MOUSEMOVE == uMsg)
	{
		OwnVector3 deltaMove(0,0,0);
		//mouse screen coordinate: leftTop(0,0), rightButtom(width, height).
		deltaMove.m_x = -( GET_X_LPARAM(lParam) - s_MouseState.mousePos.m_x ) * 0.1f;
		deltaMove.m_y = -( GET_Y_LPARAM(lParam) - s_MouseState.mousePos.m_y ) * 0.1f;

		s_MouseState.mousePos.m_x = GET_X_LPARAM(lParam);
		s_MouseState.mousePos.m_y = GET_Y_LPARAM(lParam);

		f32 moveFactor = 0.5f, rotFactor = 0.05f;

		switch(s_MouseState.btnType)
		{
		case 1:
			deltaPos.set(deltaMove.m_x * moveFactor, -deltaMove.m_y * moveFactor, 0);
			pDeltaPos = &deltaPos;
			break;
		case 2:
			deltaRot.set(deltaMove.m_y * rotFactor, deltaMove.m_x * rotFactor, 0);
			pDeltaRot = &deltaRot;
			break;
		case 3:
			deltaOrbit.set(deltaMove.m_y * -rotFactor, deltaMove.m_x * -rotFactor, 0);
			pDeltaOrbit = &deltaOrbit;
			break;
		}
	}
	else if(WM_MOUSEWHEEL == uMsg)
	{
		deltaPos.set(0,0,0.01f * GET_WHEEL_DELTA_WPARAM(wParam));
		pDeltaPos = & deltaPos;
	}
	else if(WM_KEYDOWN)
	{
		switch(wParam)
		{
		case 'W':
		case 'w':
			deltaPos.set(0,0,camMoveOffset);
			pDeltaPos = &deltaPos;
			break;
		case 'S':
		case 's':
			deltaPos.set(0,0,-camMoveOffset);
			pDeltaPos = &deltaPos;
			break;
		case 'A':
		case 'a':
			deltaPos.set(-camMoveOffset, 0, 0);
			pDeltaPos = &deltaPos;
			break;
		case 'D':
		case 'd':
			deltaPos.set(camMoveOffset, 0, 0);
			pDeltaPos = &deltaPos;
			break;
		case 'Q':
		case 'q':
			deltaPos.set(0, camMoveOffset, 0);
			pDeltaPos = &deltaPos;
			break;
		case 'E':
		case 'e':
			deltaPos.set(0, -camMoveOffset, 0);
			pDeltaPos = &deltaPos;
			break;
		case VK_UP:
			deltaRot.set(camRotOffset, 0, 0);
			pDeltaRot = &deltaRot;
			break;
		case VK_DOWN:
			deltaRot.set(-camRotOffset, 0, 0);
			pDeltaRot = &deltaRot;
			break;
		case VK_LEFT:
			deltaRot.set(0, camRotOffset, 0);
			pDeltaRot = &deltaRot;
			break;
		case VK_RIGHT:
			deltaRot.set(0, -camRotOffset, 0);
			pDeltaRot = &deltaRot;
			break;
		}
	}

	cameraCtrl(pDeltaPos, pDeltaRot, pDeltaOrbit);
}
//-----------------------------------------------------------------------
void CIntegrateModule::cameraCtrl( OwnVector3 *pDeltaPos, OwnVector3 *pDeltaRot, OwnVector3 *pDeltaOrbitRot )
{
	if(NULL == mCamera)
		return;

	if(NULL == pDeltaPos && NULL == pDeltaRot && NULL == pDeltaOrbitRot)
		return;

	mCamera->onInput(pDeltaPos, pDeltaRot, pDeltaOrbitRot);
}
//-----------------------------------------------------------------------
void CIntegrateModule::releaseForDeviceReset()
{
	mChapterBase->releaseForDeviceReset();
}

//-----------------------------------------------------------------------
void CIntegrateModule::renderPrevious()
{
	mChapterBase->renderPrevious();
}

//-----------------------------------------------------------------------
void CIntegrateModule::deviceRestore()
{
	if(NULL != mChapterBase)
		mChapterBase->shaderConfig(mVtxConstTable, mPxlConstTable);

	mD3DDevice->SetVertexShader(mVertexShader);
	mD3DDevice->SetPixelShader(mPixelShader);

	mChapterBase->deviceRestore();
}
//--------------------------------------------------------------------------------------