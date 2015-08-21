#include "EnvironmentMap.h"

#include "OwnCamera.h"
#include "Global.h"
#include "DXFrame.h"

#define ENVMAPSIZE 256

//-----------------------------------------------------------------------
CEnvironmentMap::CEnvironmentMap()
{
}

//-----------------------------------------------------------------------
CEnvironmentMap::CEnvironmentMap(LPDIRECT3DDEVICE9 pdevice)
:CAnimation(pdevice)
, mCubeMap(NULL)
{
}

//-----------------------------------------------------------------------
CEnvironmentMap::~CEnvironmentMap()
{
	for (int i = 0; i < s_ObjCount; i++)
	{
		SAFE_RELEASE(mEnvironmentObj[i].mesh);
	}

	releaseForDeviceReset();
}
//-----------------------------------------------------------------------
void CEnvironmentMap::draw(bool bRenderEnvMappedMesh)
{
	mD3DDevice->SetFVF(getFVF());

	if (bRenderEnvMappedMesh)
	{
		mD3DDevice->SetTexture(2, mCubeMap);
	}

	shaderApply();

	for (int i = 0; i < s_ObjCount; i++)
	{
		modelShaderApply(&mEnvironmentObj[i].modelMtx);
		mEnvironmentObj[i].mesh->DrawSubset(0);
	}

}
//-----------------------------------------------------------------------
void CEnvironmentMap::deviceRestore()
{
	mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	HRESULT hr;

	ZeroMemory(mCubeMap, sizeof(mCubeMap));

	hr = mD3DDevice->CreateCubeTexture(ENVMAPSIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &mCubeMap, NULL);

	hr = mD3DDevice->CreateDepthStencilSurface(ENVMAPSIZE, ENVMAPSIZE, CDXFrame::getSingletonPtr()->mD3Dpp.AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &mDepthCube, NULL);

}
//-----------------------------------------------------------------------
std::string CEnvironmentMap::getVSFunctionName()
{
	return "C7E1v_reflection";
}
//-----------------------------------------------------------------------
std::string CEnvironmentMap::getPSFunctionName()
{
	return "C7E2f_reflection";
}

//-----------------------------------------------------------------------
HRESULT CEnvironmentMap::initScene()
{
	deviceRestore();

	initPatch("AnimationObj", 20);

	return S_OK;
}

//-----------------------------------------------------------------------
void CEnvironmentMap::releaseForDeviceReset()
{
	SAFE_RELEASE(mCubeMap);
	SAFE_RELEASE(mDepthCube);
}

//-----------------------------------------------------------------------
void CEnvironmentMap::shaderConfig(LPD3DXCONSTANTTABLE pVtxConstTable /*= NULL*/, LPD3DXCONSTANTTABLE pPxlConstTable /*= NULL*/)
{
	mVtxConstTable = pVtxConstTable;
	mPxlConstTable = pPxlConstTable;

	OwnMatrix4x4 modelMtx, viewProjMtx, modelViewProjMtx;
	modelMtx.setIdentity();

	viewProjMtx.setMultiply(mCamera->mViewTrans, mCamera->mProjectionTrans);
	modelViewProjMtx.setMultiply(modelMtx, viewProjMtx);

	HRESULT hr;
	D3DXHANDLE handle;

	D3DXHANDLE modelViewProj = mVtxConstTable->GetConstantByName(0, "u_ModelViewProjMatrix");
	hr = mVtxConstTable->SetMatrix(mD3DDevice, modelViewProj, (D3DXMATRIX*)&modelViewProjMtx);

	lightShaderConfig();
}

//-----------------------------------------------------------------------
DWORD CEnvironmentMap::getFVF()
{
	return D3DFVF_XYZ_NORMAL_TEX1;
}

//-----------------------------------------------------------------------
void CEnvironmentMap::initMesh(LPD3DXPATCHMESH pMesh, FLOAT fTessLevel)
{
	f32 start = 15, span = 15;
	f32 x = -start, z = start;
	for (int i = 0; i < s_ObjCount; i++)
	{
		generateMesh(pMesh, &mEnvironmentObj[i].mesh, fTessLevel);

		OwnVector3 pos(x, 0, z);
		mEnvironmentObj[i].modelMtx.makeTrans(pos);

		x += span;
		if (x > start)
		{
			x = -start;
			z -= span;
		}

	}
}

//-----------------------------------------------------------------------
void CEnvironmentMap::lightShaderConfig()
{
	HRESULT hr;
	D3DXHANDLE handle;

	Material material;
	material.Ke = D3DXVECTOR3(0, 0, 0);
	material.Ka = D3DXVECTOR3(0, 0, 0);
	material.Kd = D3DXVECTOR3(1, 1, 1);
	material.Ks = D3DXVECTOR3(1, 1, 1);
	material.shininess = 5;

	Light light;
	light.position = D3DXVECTOR3(-10, 0, -10);
	light.color = D3DXVECTOR3(1, 0, 0);

	LPD3DXCONSTANTTABLE constable = mVtxConstTable;

	D3DXHANDLE u_Material = constable->GetConstantByName(0, "u_Material");
	hr = constable->SetValue(mD3DDevice, u_Material, &material, sizeof(material));

	D3DXHANDLE u_Light = constable->GetConstantByName(0, "u_Light");
	hr = constable->SetValue(mD3DDevice, u_Light, &light, sizeof(light));
}

//-----------------------------------------------------------------------
void CEnvironmentMap::modelShaderApply(OwnMatrix4x4 *pModelMtx /*= NULL*/)
{
	LPD3DXCONSTANTTABLE constable = mVtxConstTable;

	OwnMatrix4x4 modelMtx, viewProjMtx, modelViewProjMtx;

	if (NULL == pModelMtx)
		modelMtx.setIdentity();
	else
		modelMtx = *pModelMtx;

	viewProjMtx.setMultiply(mCamera->mViewTrans, mCamera->mProjectionTrans);
	modelViewProjMtx.setMultiply(modelMtx, viewProjMtx);

	HRESULT hr;
	D3DXHANDLE handle;

	D3DXHANDLE modelViewProj = mVtxConstTable->GetConstantByName(0, "u_ModelViewProjMatrix");
	hr = mVtxConstTable->SetMatrix(mD3DDevice, modelViewProj, (D3DXMATRIX*)&modelViewProjMtx);

	//TODO: world space to model space matrix not just translate, rotate also need opposite
	modelMtx.m41 = -modelMtx.m41;
	modelMtx.m42 = -modelMtx.m42;
	modelMtx.m43 = -modelMtx.m43;
	D3DXHANDLE u_ModelMatrix = constable->GetConstantByName(0, "u_ModelMatrix");
	hr = constable->SetMatrix(mD3DDevice, u_ModelMatrix, (D3DXMATRIX*)&modelMtx);

}

//-----------------------------------------------------------------------
void CEnvironmentMap::renderSceneIntoCubeMap(IDirect3DDevice9* pdevice)
{
	HRESULT hr;

	LPDIRECT3DSURFACE9 pRTOld = NULL;
	hr = mD3DDevice->GetRenderTarget(0, &pRTOld);

	LPDIRECT3DSURFACE9 pDSOld = NULL;

	if (SUCCEEDED(mD3DDevice->GetDepthStencilSurface(&pDSOld)))
	{
		// If the device has a depth-stencil buffer, use
		// the depth stencil buffer created for the cube textures.
		hr = mD3DDevice->SetDepthStencilSurface(mDepthCube);
	}

	// The projection matrix has a FOV of 90 degrees and asp ratio of 1
	float oldFov = mCamera->mFov;
	float oldAsp = mCamera->mAspect;

	mCamera->mProjectionTrans.setProjectionTransLH(Pai * 0.5f, 1.0f, mCamera->mZNear, mCamera->mZFar);

	OwnMatrix4x4 oldViewTrans = mCamera->mViewTrans;

	mCamera->setPosition(OwnVector3::ZERO);

	for (int nFace = 0; nFace < 6; ++nFace)
	{
		LPDIRECT3DSURFACE9 pSurf;

		hr = mCubeMap->GetCubeMapSurface((D3DCUBEMAP_FACES)nFace, 0, &pSurf);
		hr = mD3DDevice->SetRenderTarget(0, pSurf);
		SAFE_RELEASE(pSurf);

		setCubeMapCamera(dynamic_cast<OwnCameraQuaternion*>(mCamera), nFace);

		hr = mD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0x000000ff, 1.0f, 0L);

		if ( SUCCEEDED( mD3DDevice->BeginScene() ) )
		{
			draw();

			mD3DDevice->EndScene();
		}
	}

	if (pDSOld)
	{
		mD3DDevice->SetDepthStencilSurface(pDSOld);
		SAFE_RELEASE(pDSOld);
	}

	mD3DDevice->SetRenderTarget(0, pRTOld);
	SAFE_RELEASE(pRTOld);
}

//-----------------------------------------------------------------------
void CEnvironmentMap::shaderApply()
{
	LPD3DXCONSTANTTABLE constable = mVtxConstTable;

	HRESULT hr;
	D3DXHANDLE handle;

	if (NULL == constable)
		return;

	D3DXVECTOR4 EyePosition(mCamera->mCameraPos.m_x, mCamera->mCameraPos.m_y, mCamera->mCameraPos.m_z, 1);

	Shader_SetVector(mD3DDevice, constable, "u_EyePosition", EyePosition);

}

//-----------------------------------------------------------------------
void CEnvironmentMap::setCubeMapCamera(OwnCameraQuaternion* camera, int face)
{
	OwnVector3 lookDir;

	switch (face)
	{
	case D3DCUBEMAP_FACE_POSITIVE_X:
		lookDir = OwnVector3::UNIT_X;
		break;
	case D3DCUBEMAP_FACE_NEGATIVE_X:
		lookDir = -OwnVector3::UNIT_X;
		break;
	case D3DCUBEMAP_FACE_POSITIVE_Y:
		lookDir = OwnVector3::UNIT_Y;
		break;
	case D3DCUBEMAP_FACE_NEGATIVE_Y:
		lookDir = -OwnVector3::UNIT_Y;
		break;
	case D3DCUBEMAP_FACE_POSITIVE_Z:
		lookDir = OwnVector3::UNIT_Z;
		break;
	case D3DCUBEMAP_FACE_NEGATIVE_Z:
		lookDir = -OwnVector3::UNIT_Z;
		break;
	}

	camera.setDirection(lookDir);
}

//-----------------------------------------------------------------------



