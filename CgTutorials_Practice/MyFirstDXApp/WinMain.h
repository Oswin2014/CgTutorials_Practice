#pragma once

#include <Windows.h>

class CWinMain
{
public:
	~CWinMain();

	static CWinMain* getSingletonPtr()
	{
		if (NULL == mSingleton)
		{
			mSingleton = new CWinMain;
		}
		return mSingleton;
	}

	LRESULT createWnd( HINSTANCE hInstance, int nCmdShow );

	HWND getHWnd(){ return mHWnd; }

	HRESULT initWnd();

	void updateWnd();

	static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

private:
	CWinMain();

	static CWinMain *mSingleton;

	HWND mHWnd;

	HINSTANCE mHInst;
};
