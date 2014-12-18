#include "ShaderBase.h"


CShaderBase::CShaderBase(void)
{
}
//-----------------------------------------------------------------------
CShaderBase::CShaderBase( LPDIRECT3DDEVICE9 pdevice )
	: CChapterBase(pdevice)
	, mTextureShesays(NULL)
{
}
//-----------------------------------------------------------------------
CShaderBase::~CShaderBase(void)
{
	SAFE_RELEASE(mTextureShesays);
}
//-----------------------------------------------------------------------
HRESULT CShaderBase::initSceneT2()
{
	mVertexT2[0] = D3DXVECTOR4(-0.8f, -0.8f, 0.f, 1.f);
	mVertexT2[1] = D3DXVECTOR4( 0.8f, -0.8f, 0.f, 1.f);
	mVertexT2[2] = D3DXVECTOR4( 0.f,   0.8f, 0.f, 1.f);

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CShaderBase::initSceneT3()
{
	mVertexT3[0].position = D3DXVECTOR4(-0.8f,  0.8f, 0.f, 1.0f);
	mVertexT3[1].position = D3DXVECTOR4(-0.8f, -0.8f, 0.f, 1.0f);
	mVertexT3[2].position = D3DXVECTOR4( 0.8f,  0.8f, 0.f, 1.0f);
	mVertexT3[3].position = D3DXVECTOR4( 0.8f, -0.8f, 0.f, 1.0f);
	mVertexT3[0].color = 0xff0000ff;
	mVertexT3[1].color = 0xff0000ff;
	mVertexT3[2].color = 0xff0000ff;
	mVertexT3[3].color = 0xff0000ff;
	mVertexT3[0].tu = 0.0f; mVertexT3[0].tv = 0.0f;
	mVertexT3[1].tu = 0.0f; mVertexT3[1].tv = 1.0f;
	mVertexT3[2].tu = 1.0f; mVertexT3[2].tv = 0.0f;
	mVertexT3[3].tu = 1.0f; mVertexT3[3].tv = 1.0f;

	if(FAILED( D3DXCreateTextureFromFile( mD3DDevice, L"..\\..\\Media\\she says.jpg", &mTextureShesays )))
		return E_FAIL;
	mD3DDevice->SetTexture( 0, mTextureShesays );

	return S_OK;
}
//-----------------------------------------------------------------------
void CShaderBase::drawT2()
{
	mD3DDevice->SetFVF(D3DFVF_XYZW_DIFFUSE);
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, mVertexT2, sizeof(D3DXVECTOR4));
}
//-----------------------------------------------------------------------
void CShaderBase::drawT3()
{
	//mD3DDevice->SetTexture( 0, mTextureShesays );
	mD3DDevice->SetFVF(D3DFVF_XYZW_DIFFUSE_TEX1);
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, mVertexT3, sizeof(VERTEX_XYZW_DIFFUSE_TEX1));
}
//-----------------------------------------------------------------------
std::string CShaderBase::getVSFunctionName()
{
	//return "C2E1v_green";
// 	return "C3E2v_varying";
// 	return "C3E2v_input";
 	//return "C3E4v_twist";
 	return "C3E5v_twoTextures";
}
//-----------------------------------------------------------------------
std::string CShaderBase::getPSFunctionName()
{
	//return "C2E2f_passthrough";
 	//return "C3E3f_texture";
 	return "C3E6f_twoTextures";
}
//-----------------------------------------------------------------------
void CShaderBase::shaderConfigT2( LPD3DXCONSTANTTABLE pVtxConstTable, LPD3DXCONSTANTTABLE pPxlConstTable )
{
}
//-----------------------------------------------------------------------
void CShaderBase::shaderConfigT3( LPD3DXCONSTANTTABLE pVtxConstTable, LPD3DXCONSTANTTABLE pPxlConstTable )
{
	HRESULT hr;
	//DWORD size = pVtxConstTable->GetBufferSize();

	hr = pVtxConstTable->SetVector( mD3DDevice, 
		pVtxConstTable->GetConstantByName(0, "$constantColor"), &D3DXVECTOR4(1,1,0,1) );

	hr = pVtxConstTable->SetFloat( mD3DDevice, 
		pVtxConstTable->GetConstantByName(0, "$twisting"), 2.5f );

	hr = pVtxConstTable->SetVector( mD3DDevice, 
		pVtxConstTable->GetConstantByName(0, "$leftSeparation"), &D3DXVECTOR4(-0.3f,0,0,0) );
	hr = pVtxConstTable->SetVector( mD3DDevice, 
		pVtxConstTable->GetConstantByName(0, "$rightSeparation"), &D3DXVECTOR4(+0.3f,0,0,0) );

}
//-----------------------------------------------------------------------
void CShaderBase::draw()
{
	//drawT2();
	drawT3();
}
//-----------------------------------------------------------------------
HRESULT CShaderBase::initScene()
{
	//HRESULT hr = initSceneT2();
	HRESULT hr = initSceneT3();

	return hr;
}
//-----------------------------------------------------------------------
void CShaderBase::shaderConfig( LPD3DXCONSTANTTABLE pVtxConstTable /*= NULL*/, LPD3DXCONSTANTTABLE pPxlConstTable /*= NULL*/ )
{
	shaderConfigT3(pVtxConstTable);
}
//-----------------------------------------------------------------------
void CShaderBase::releaseForDeviceReset()
{

}
//-----------------------------------------------------------------------
void CShaderBase::deviceRestore()
{
	mD3DDevice->SetTexture( 0, mTextureShesays );
}
//-----------------------------------------------------------------------