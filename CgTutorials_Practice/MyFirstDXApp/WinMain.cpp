#include "WinMain.h"

#include <d3dx9.h>
#include "resource.h"
#include <mmsystem.h>

#include "DXFrame.h"


CWinMain* CWinMain::mSingleton = NULL;
//--------------------------------------------------------------------------------------
CWinMain::CWinMain()
{

}
//--------------------------------------------------------------------------------------
CWinMain::~CWinMain()
{
	delete CDXFrame::getSingletonPtr();
}
//--------------------------------------------------------------------------------------
LRESULT CALLBACK CWinMain::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message )
	{
	case WM_PAINT:
		hdc = BeginPaint( hWnd, &ps );
		EndPaint( hWnd, &ps );
		break;

	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;

	case WM_SIZE:
		if(SIZE_MINIMIZED != wParam)
			CDXFrame::getSingletonPtr()->deviceReset(lParam);
		break;

	default:
		CDXFrame::getSingletonPtr()->onInput(message, wParam, lParam);
		return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}
//--------------------------------------------------------------------------------------
HRESULT CWinMain::createWnd( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MyDXApp";
	wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	if( !RegisterClassEx( &wcex ) )
		return E_FAIL;

	// Create window
	mHInst = hInstance; 
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	mHWnd = CreateWindow( L"MyDXApp", L"MyDXApp", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL );
	if( !mHWnd )
		return E_FAIL;

	ShowWindow( mHWnd, nCmdShow );

	return S_OK;
}
//--------------------------------------------------------------------------------------
void CWinMain::updateWnd()
{
	CDXFrame::getSingletonPtr()->renderLoop();
}
//--------------------------------------------------------------------------------------
HRESULT CWinMain::initWnd()
{
	if( FAILED( CDXFrame::getSingletonPtr()->init() ) )
	{
		return E_FAIL;
	}
}
//--------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	if( FAILED( CWinMain::getSingletonPtr()->createWnd( hInstance, nCmdShow ) ) )
		return 0;

	if( FAILED( CWinMain::getSingletonPtr()->initWnd() ) )
		return 0;

	// Main message loop
	MSG msg = {0};
	while( WM_QUIT != msg.message )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			CWinMain::getSingletonPtr()->updateWnd();
		}
	}
	delete CWinMain::getSingletonPtr();

	return ( int )msg.wParam;
}
////////////////////////////////////////////////////////////////////////////////////////



