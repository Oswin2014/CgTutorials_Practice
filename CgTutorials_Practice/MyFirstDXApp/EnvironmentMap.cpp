#include "EnvironmentMap.h"

#include "OwnCamera.h"
#include "Global.h"
#include "DXFrame.h"

#define ENVMAPSIZE 2048

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
void CEnvironmentMap::draw()
{
	draw(true);
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

	OwnMatrix4x4 modelMtx;
	modelMtx.setIdentity();
	modelMtx.m43 = -30.0f;
	modelShaderApply(&modelMtx, true);

	mD3DDevice->SetStreamSource(0, mVBQuad, 0, sizeof(VERTEX_XYZ_NORMAL_TEX1));
	mD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


	for (int i = 0; i < s_ObjCount; i++)
	{
		//if (4 == i)
		//{
		//	if (!bRenderEnvMappedMesh)
		//		continue;
		//
		//	modelShaderApply(&mEnvironmentObj[i].modelMtx, true);
		//}
		//else
			modelShaderApply(&mEnvironmentObj[i].modelMtx, false);

		mEnvironmentObj[i].mesh->DrawSubset(0);
	}
	
	/*
	if (bRenderEnvMappedMesh)
	{
		modelShaderApply(&mEnvironmentObj[4].modelMtx, true);
		mEnvironmentObj[4].mesh->DrawSubset(0);
	}

	modelShaderApply(&mEnvironmentObj[5].modelMtx, false);
	mEnvironmentObj[5].mesh->DrawSubset(0);
	*/
}
//-----------------------------------------------------------------------
void CEnvironmentMap::deviceRestore()
{
	mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	HRESULT hr;

	//ZeroMemory(mCubeMap, sizeof(mCubeMap));

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

	createVertexBuff();

	if (FAILED(D3DXCreateTextureFromFile(mD3DDevice, L"..\\..\\Media\\she says.jpg", &mTextureShesays)))
		return E_FAIL;
	mD3DDevice->SetTexture(1, mTextureShesays);

	return S_OK;
}

//-----------------------------------------------------------------------
void CEnvironmentMap::releaseForDeviceReset()
{
	SAFE_RELEASE(mCubeMap);
	SAFE_RELEASE(mDepthCube);
	SAFE_RELEASE(mVBQuad);
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
HRESULT CEnvironmentMap::createVertexBuff()
{
	VERTEX_XYZ_NORMAL_TEX1 quad[4];
	float halfWidth = 25.0f;
	quad[0].position = D3DXVECTOR3(-halfWidth, halfWidth, 0.0f);
	quad[1].position = D3DXVECTOR3(-halfWidth, -halfWidth, 0.0f);
	quad[2].position = D3DXVECTOR3(halfWidth, halfWidth, 0.0f);
	quad[3].position = D3DXVECTOR3(halfWidth, -halfWidth, 0.0f);

	quad[0].texCoord.x = 0.0f;	quad[0].texCoord.y = 0.0f;
	quad[1].texCoord.x = 0.0f;	quad[1].texCoord.y = 1.0f;
	quad[2].texCoord.x = 1.0f;	quad[2].texCoord.y = 0.0f;
	quad[3].texCoord.x = 1.0f;	quad[3].texCoord.y = 1.0f;

	quad[0].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	quad[1].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	quad[2].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	quad[3].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	if (FAILED(mD3DDevice->CreateVertexBuffer(4 * sizeof(VERTEX_XYZ_NORMAL_TEX1),
		0, D3DFVF_XYZ_TEX1,
		D3DPOOL_DEFAULT, &mVBQuad, NULL)))
	{
		return E_FAIL;
	}
	VOID* pVertices2;
	if (FAILED(mVBQuad->Lock(0, sizeof(quad), (void**)&pVertices2, 0)))
		return E_FAIL;
	memcpy(pVertices2, quad, sizeof(quad));
	mVBQuad->Unlock();
}

//-----------------------------------------------------------------------
DWORD CEnvironmentMap::getFVF()
{
	return D3DFVF_XYZ_NORMAL_TEX1;
}

//-----------------------------------------------------------------------
void CEnvironmentMap::initMesh(LPD3DXPATCHMESH pMesh, FLOAT fTessLevel)
{
	f32 start = 8, span = 8;
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
	light.color = D3DXVECTOR3(0, 1, 0);

	LPD3DXCONSTANTTABLE constable = mVtxConstTable;

	D3DXHANDLE u_Material = constable->GetConstantByName(0, "u_Material");
	hr = constable->SetValue(mD3DDevice, u_Material, &material, sizeof(material));

	D3DXHANDLE u_Light = constable->GetConstantByName(0, "u_Light");
	hr = constable->SetValue(mD3DDevice, u_Light, &light, sizeof(light));
}

//-----------------------------------------------------------------------
void CEnvironmentMap::modelShaderApply(OwnMatrix4x4 *pModelMtx /*= NULL*/, bool bRenderEnvMappedMesh)
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

	D3DXHANDLE u_ModelMatrix = constable->GetConstantByName(0, "u_ModelToWorld");
	hr = constable->SetMatrix(mD3DDevice, u_ModelMatrix, (D3DXMATRIX*)&modelMtx);

	LPD3DXCONSTANTTABLE pixConstable = mPxlConstTable;

	float value = bRenderEnvMappedMesh ? 0.5f : 0;
	D3DXHANDLE reflectivity = pixConstable->GetConstantByName(0, "$reflectivity");
	hr = pixConstable->SetFloat(mD3DDevice, reflectivity, value);

}

//-----------------------------------------------------------------------
void CEnvironmentMap::renderSceneIntoCubeMap(IDirect3DDevice9* pdevice)
{
	HRESULT hr;

	OwnCameraQuaternion* camera = dynamic_cast<OwnCameraQuaternion*>(mCamera);

	if (NULL == camera)
		return;

	OwnCameraQuaternion cameraMap(*camera);

	LPDIRECT3DSURFACE9 pRTOld = NULL;
	hr = pdevice->GetRenderTarget(0, &pRTOld);

	LPDIRECT3DSURFACE9 pDSOld = NULL;

	if (SUCCEEDED(pdevice->GetDepthStencilSurface(&pDSOld)))
	{
		// If the device has a depth-stencil buffer, use
		// the depth stencil buffer created for the cube textures.
		hr = pdevice->SetDepthStencilSurface(mDepthCube);
	}

	// The projection matrix has a FOV of 90 degrees and asp ratio of 1
	cameraMap.mProjectionTrans.setProjectionTransLH(Pai * 0.5f, 1.0f, cameraMap.mZNear, cameraMap.mZFar);

	//cameraMap.setPosition(OwnVector3::ZERO);
	cameraMap.setPosition(OwnVector3(0,0,-30.0f));

	for (int nFace = 0; nFace < 6; ++nFace)
	{
		LPDIRECT3DSURFACE9 pSurf;

		hr = mCubeMap->GetCubeMapSurface((D3DCUBEMAP_FACES)nFace, 0, &pSurf);
		hr = pdevice->SetRenderTarget(0, pSurf);
		SAFE_RELEASE(pSurf);

		setCubeMapCamera(&cameraMap, nFace);

		mCamera = &cameraMap;
		hr = pdevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0x000000ff, 1.0f, 0L);

		if (SUCCEEDED(pdevice->BeginScene()))
		{
			draw(false);

			pdevice->EndScene();
		}
	}

	if (pDSOld)
	{
		pdevice->SetDepthStencilSurface(pDSOld);
		SAFE_RELEASE(pDSOld);
	}

	pdevice->SetRenderTarget(0, pRTOld);
	SAFE_RELEASE(pRTOld);

	mCamera = camera;

}

//-----------------------------------------------------------------------
void CEnvironmentMap::renderPrevious()
{
	renderSceneIntoCubeMap(mD3DDevice);
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

	camera->setDirection(lookDir);
}

//-----------------------------------------------------------------------



