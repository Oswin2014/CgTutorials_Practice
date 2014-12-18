#include "D3D9Tutorials.h"
#include "Global.h"

CD3D9Tutorials::CD3D9Tutorials(void)
{
}
//-----------------------------------------------------------------------
CD3D9Tutorials::CD3D9Tutorials( LPDIRECT3DDEVICE9 pdevice )
	: CChapterBase(pdevice)
	,mVBTestPointShader(NULL)
	,mVBXYZAxis(NULL)
	,mVBLight(NULL)
	,mVBTexture(NULL)
	,mTextureShesays(NULL)
{
	mShaderUse = false;
}
//-----------------------------------------------------------------------
CD3D9Tutorials::~CD3D9Tutorials(void)
{
	SAFE_RELEASE(mVBTestPointShader);
	SAFE_RELEASE(mVBXYZAxis);
	SAFE_RELEASE(mVBLight);
	SAFE_RELEASE(mVBTexture);
	SAFE_RELEASE(mTextureShesays);
}
//-----------------------------------------------------------------------
void CD3D9Tutorials::createLight()
{
	D3DMATERIAL9 matl;
	ZeroMemory( &matl, sizeof(D3DMATERIAL9) );
	matl.Diffuse.r = matl.Ambient.r = 0.0f;
	matl.Diffuse.g = matl.Ambient.g = 0.0f;
	matl.Diffuse.b = matl.Ambient.b = 1.0f;
	matl.Diffuse.a = matl.Ambient.a = 1.0f;
	mD3DDevice->SetMaterial(&matl);

	D3DLIGHT9 light1;
	ZeroMemory( &light1, sizeof(D3DLIGHT9) );
	light1.Type = D3DLIGHT_DIRECTIONAL;
	light1.Diffuse.r = 1.0f;
	light1.Diffuse.g = 1.0f;
	light1.Diffuse.b = 1.0f;
	D3DXVECTOR3 direct = D3DXVECTOR3( cosf( timeGetTime() / 360 ), 1.0f, sinf( timeGetTime() / 360) );
	D3DXVec3Normalize( ( D3DXVECTOR3* )&light1.Direction, &direct);
	light1.Range = 1000;
	mD3DDevice->SetLight( 0, &light1 );
	mD3DDevice->LightEnable( 0, TRUE );
	mD3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	mD3DDevice->SetRenderState( D3DRS_AMBIENT, 0x00ffffff );
}
//-----------------------------------------------------------------------
void CD3D9Tutorials::createMatricesTransform()
{
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	mD3DDevice->SetTransform( D3DTS_VIEW, &matView );

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f );
	mD3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}
//-----------------------------------------------------------------------
HRESULT CD3D9Tutorials::createVertexBuff()
{
	VERTEX_XYZRHW_DIFFUSE vertices[] =
	{
		{ -0.2f,0.2f,0.2f , 0xffff0000, }, 
		{ 0.2f,0.2f,0.2f , 0xff00ffff,  },

		{ -0.2f,-0.2f,0.2f, 0xff00ff00, },
		//		{ 50.0f,-50.0f,0.0f ,0xffff0000,  },
	};

	VERTEX_TEXTURE axis[6];
	axis[0].position = D3DXVECTOR3( 0.0f,0.0f,0.0f );
	axis[1].position = D3DXVECTOR3( 500.0f,0.0f,0.0f );
	axis[2].position = D3DXVECTOR3( 0.0f,0.0f,0.0f );
	axis[3].position = D3DXVECTOR3( 0.0f,500.0f,0.0f );
	axis[4].position = D3DXVECTOR3( 0.0f,0.0f,0.0f );
	axis[5].position = D3DXVECTOR3( 0.0f,0.0f,500.0f );


	VERTEX_TEXTURE quad[4];
	quad[0].position = D3DXVECTOR3( -1.0f, 1.0f, 2.0f );
	quad[1].position = D3DXVECTOR3( -1.0f,-1.0f, 2.0f );
	quad[2].position = D3DXVECTOR3(  1.0f, 1.0f, 2.0f );
	quad[3].position = D3DXVECTOR3(  1.0f,-1.0f, 2.0f );
	quad[0].tu = 0.0f;	quad[0].tv = 0.0f;
	quad[1].tu = 0.0f;	quad[1].tv = 1.0f;
	quad[2].tu = 1.0f;	quad[2].tv = 0.0f;
	quad[3].tu = 1.0f;	quad[3].tv = 1.0f;

	VERTEX_XYZ_NORMAL quad1[4];
	quad1[0].position = D3DXVECTOR3( -1.0f, 1.0f, -0.5f );
	quad1[1].position = D3DXVECTOR3( -1.0f,-1.0f, -0.5f );
	quad1[2].position = D3DXVECTOR3(  1.0f, 1.0f, -0.5f );
	quad1[3].position = D3DXVECTOR3(  1.0f,-1.0f, -0.5f );
	quad1[0].normal = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	quad1[1].normal = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	quad1[2].normal = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	quad1[3].normal = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

	if( FAILED( mD3DDevice->CreateVertexBuffer( 3 * sizeof( VERTEX_XYZRHW_DIFFUSE ),
		0, D3DFVF_XYZ_DIFFUSE,
		D3DPOOL_DEFAULT, &mVBTestPointShader, NULL ) ) )
	{
		return E_FAIL;
	}
	VOID* pVertices;
	if( FAILED( mVBTestPointShader->Lock( 0, sizeof( vertices ), ( void** )&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, vertices, sizeof( vertices ) );
	mVBTestPointShader->Unlock();

	if( FAILED( mD3DDevice->CreateVertexBuffer( 6 * sizeof( VERTEX_TEXTURE ),
		0, D3DFVF_XYZ_TEX1,
		D3DPOOL_DEFAULT, &mVBXYZAxis, NULL ) ) )
	{
		return E_FAIL;
	}
	VOID* pVertices1;
	if( FAILED( mVBXYZAxis->Lock( 0, sizeof( axis ), ( void** )&pVertices1, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices1, axis, sizeof( axis ) );
	mVBXYZAxis->Unlock();

	if( FAILED( mD3DDevice->CreateVertexBuffer( 4 * sizeof( VERTEX_TEXTURE ),
		0, D3DFVF_XYZ_TEX1,
		D3DPOOL_DEFAULT, &mVBTexture, NULL ) ) )
	{
		return E_FAIL;
	}
	VOID* pVertices2;
	if( FAILED( mVBTexture->Lock( 0, sizeof( quad ), ( void** )&pVertices2, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices2, quad, sizeof( quad ) );
	mVBTexture->Unlock();

	if (FAILED(mD3DDevice->CreateVertexBuffer( 4 * sizeof(VERTEX_XYZ_NORMAL),
		0,D3DFVF_XYZ_NORMAL,
		D3DPOOL_DEFAULT, &mVBLight , NULL)))
		return E_FAIL;
	VOID* pVertices3;
	if(FAILED( mVBLight->Lock( 0,sizeof(quad1), (void**)&pVertices3, 0) ))
		return E_FAIL;
	memcpy(pVertices3, quad1, sizeof(quad1) );
	mVBLight->Unlock();
}
//-----------------------------------------------------------------------
void CD3D9Tutorials::draw()
{
// 	D3DXMATRIXA16 matWorld;
// 	D3DXMatrixRotationY( &matWorld, timeGetTime() / 1000.0f );
// 	mD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );

	 	static float index = 0.0f; index+=0.015f; // an ever-increasing float value
	 	D3DXMATRIX matRotateY;    // a matrix to store the rotation for each triangle
	 	D3DXMatrixRotationY(&matRotateY, index);    // the rotation matrix
	 	mD3DDevice->SetTransform(D3DTS_WORLD, &(matRotateY));    // set the world transform

	mD3DDevice->SetTexture( 0, mTextureShesays );

	mD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	//Rendering of scene objects happens here
	mD3DDevice->SetStreamSource(0,mVBTestPointShader,0,sizeof(VERTEX_XYZRHW_DIFFUSE));
	mD3DDevice->SetFVF(D3DFVF_XYZ_DIFFUSE);
	mD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,1);

	mD3DDevice->SetStreamSource(0,mVBXYZAxis,0,sizeof(VERTEX_TEXTURE));
	mD3DDevice->SetFVF(D3DFVF_XYZ_TEX1);
	mD3DDevice->DrawPrimitive(D3DPT_LINELIST,0,3);

	mD3DDevice->SetStreamSource(0,mVBTexture,0,sizeof(VERTEX_TEXTURE));
	mD3DDevice->SetFVF(D3DFVF_XYZ_TEX1);
	mD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

	mD3DDevice->SetStreamSource(0,mVBLight,0,sizeof(VERTEX_XYZ_NORMAL));
	mD3DDevice->SetFVF(D3DFVF_XYZ_NORMAL);
	mD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

}
//-----------------------------------------------------------------------
HRESULT CD3D9Tutorials::initScene()
{
	createMatricesTransform();

	createLight();

	if(FAILED( D3DXCreateTextureFromFile( mD3DDevice, L"..\\..\\Media\\she says.jpg", &mTextureShesays )))
		return E_FAIL;

	if( FAILED( createVertexBuff() ) )
		return E_FAIL;

	return S_OK;
}
//-----------------------------------------------------------------------
void CD3D9Tutorials::releaseForDeviceReset()
{
	SAFE_RELEASE(mVBTestPointShader);
	SAFE_RELEASE(mVBXYZAxis);
	SAFE_RELEASE(mVBLight);
	SAFE_RELEASE(mVBTexture);
}
//-----------------------------------------------------------------------
void CD3D9Tutorials::deviceRestore()
{
	createMatricesTransform();

	createLight();

	createVertexBuff();
}
//-----------------------------------------------------------------------