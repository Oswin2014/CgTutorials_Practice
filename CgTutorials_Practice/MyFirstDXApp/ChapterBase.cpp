#include "ChapterBase.h"

#include "Global.h"

CChapterBase::CChapterBase(void)
{
}
//-----------------------------------------------------------------------
CChapterBase::CChapterBase( LPDIRECT3DDEVICE9 pdevice )
{
	mD3DDevice = pdevice;
	mShaderUse = true;
	mCamera = NULL;
}
//-----------------------------------------------------------------------
CChapterBase::~CChapterBase(void)
{
	SAFE_DELETE(mCamera);
}
//-----------------------------------------------------------------------