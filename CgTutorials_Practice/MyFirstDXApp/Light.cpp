#include "Light.h"

#include <assert.h>
#include "Global.h"
#include "OwnCamera.h"

// #define Shader_SetVector(constable, name, value)			\
// 	handle = constable->GetConstantByName(0, name);		\
// 	hr = constable->SetVector(mD3DDevice, handle, (D3DXVECTOR4*)&value);

//LightObject s_lightObject[16];

//-----------------------------------------------------------------------
CLight::CLight(void)
{
}
//-----------------------------------------------------------------------
CLight::CLight( LPDIRECT3DDEVICE9 pdevice )
	: CTessellateSample(pdevice)
	, mVtxConstTable(NULL)
{
	mShaderUse = true;
	mLightType = CLight::Pixel;

	//mCamera = new OwnCameraNormal();
	mCamera = new OwnCameraQuaternion();
}
//-----------------------------------------------------------------------
CLight::~CLight(void)
{
	for (int i = 0; i < 16; i++)
	{
		SAFE_RELEASE(mLightObject[i].mesh);
	}
}
//-----------------------------------------------------------------------
void CLight::draw()
{
	//shaderApply();

	mD3DDevice->SetFVF(getFVF());

// 	if(mMesh)
// 		mMesh->DrawSubset(0);

	for (int i = 0; i < 16; i++)
	{
		shaderApply(&mLightObject[i].modelMtx);
		mLightObject[i].mesh->DrawSubset(0);
	}
}
//-----------------------------------------------------------------------
std::string CLight::getVSFunctionName()
{
	if(LightType::Vertex == mLightType)
	{
		//return "C5E1v_basicLight";
		return "C5E4v_twoLights";
	}
	else 
	{
		return "C5E2v_fargmentLighting";
	}
}
//-----------------------------------------------------------------------
std::string CLight::getPSFunctionName()
{
	if(LightType::Vertex == mLightType)
	{
		return "C2E2f_passthrough";
	}
	else 
	{
		//return "C5E3f_basicLight";
		return "C5_Lighting";
	}
}
//-----------------------------------------------------------------------
HRESULT CLight::initScene()
{
	mD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	try
	{
		initPatch("lightObj2", 15);
	}
	catch(std::exception e)
	{
		auto test = e;
	}

	return S_OK;
}
//-----------------------------------------------------------------------
void CLight::shaderConfig( LPD3DXCONSTANTTABLE pVtxConstTable /*= NULL*/, LPD3DXCONSTANTTABLE pPxlConstTable /*= NULL*/ )
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

	//singleLightShaderConfig();
	//twoLightShaderConfig();

	attenuationLightShaderConfig();
}
//-----------------------------------------------------------------------
void CLight::shaderApply(OwnMatrix4x4 *pModelMtx)
{
	if(NULL != mVtxConstTable)
	{
		OwnMatrix4x4 modelMtx, viewProjMtx, modelViewProjMtx;

		if(NULL == pModelMtx)
			modelMtx.setIdentity();
		else
			modelMtx = *pModelMtx;

		viewProjMtx.setMultiply(mCamera->mViewTrans, mCamera->mProjectionTrans);
		modelViewProjMtx.setMultiply(modelMtx, viewProjMtx);

		HRESULT hr;
		D3DXHANDLE handle;

		D3DXHANDLE modelViewProj = mVtxConstTable->GetConstantByName(0, "u_ModelViewProjMatrix");
		hr = mVtxConstTable->SetMatrix(mD3DDevice, modelViewProj, (D3DXMATRIX*)&modelViewProjMtx);

		D3DXVECTOR4 EyePosition(mCamera->mCameraPos.m_x, mCamera->mCameraPos.m_y, mCamera->mCameraPos.m_z, 1);

		LPD3DXCONSTANTTABLE constable;
		if(LightType::Vertex == mLightType)
		{
			constable = mVtxConstTable;
		}
		else
		{
			constable = mPxlConstTable;
		}

		Shader_SetVector(mD3DDevice,constable, "u_EyePosition", EyePosition);

		//TODO: model matrix not just translate, rotate also need opposite
		modelMtx.m41 = - modelMtx.m41;
		modelMtx.m42 = - modelMtx.m42;
		modelMtx.m43 = - modelMtx.m43;
		D3DXHANDLE u_ModelMatrix = constable->GetConstantByName(0, "u_ModelMatrix");
		hr = constable->SetMatrix(mD3DDevice, u_ModelMatrix, (D3DXMATRIX*)&modelMtx);
	}
}
//-----------------------------------------------------------------------
void CLight::loadResource( char *filename, int *patches, int *vertices )
{
	int ii;
	float x,y,z;
	int a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;
	OwnVector3 normal;

	FILE *fp;

	if (!(fp = fopen(filename,"r"))) {
		fprintf(stderr,"Load_patch: Can't open %s\n",filename);
		exit(1);
	}

	(void)fscanf(fp,"%i\n",patches);
	for (ii = 0; ii < *patches; ii++) {
		(void)fscanf(fp,"%i, %i, %i, %i,",&a,&b,&c,&d);
		(void)fscanf(fp,"%i, %i, %i, %i,",&e,&f,&g,&h);
		(void)fscanf(fp,"%i, %i, %i, %i,",&i,&j,&k,&l);
		(void)fscanf(fp,"%i, %i, %i, %i\n",&m,&n,&o,&p);
		loadPatch(ii, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
	}
	(void)fscanf(fp,"%i\n",vertices);
	for (ii = 0; ii < *vertices; ii++) {
		(void)fscanf(fp,"%f, %f, %f, %f, %f, %f\n",&x,&y,&z, &normal.m_x, &normal.m_y, &normal.m_z);
		loadVertex(ii, x,y,z, normal);
	}
	
	fclose(fp);
}
//-----------------------------------------------------------------------
void CLight::loadVertex( int ii, float x, float y, float z, const OwnVector3& normal )
{
	//ii--;
	mVertexes[ii].x = x;
	/*** Y AND Z FLIPPED ***/
	// 	mVertexes[ii].y = z;
	// 	mVertexes[ii].z = y;
	mVertexes[ii].y = y;
	mVertexes[ii].z = z;

	mNormal[ii].x = normal.m_x;
	mNormal[ii].y = normal.m_y;
	mNormal[ii].z = normal.m_z;

}
//-----------------------------------------------------------------------
void CLight::copyVertices( LPD3DXPATCHMESH pMesh, DWORD numVertex )
{
	// copy vertices
	LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
	if (!SUCCEEDED(pMesh->GetVertexBuffer(&v_buffer))) 
		assert(FALSE);
	struct VERTEX_XYZ_NORMAL* vertex_data = NULL;
	DWORD number_of_vertices=pMesh->GetNumVertices();
	assert(number_of_vertices==numVertex);
	if (!SUCCEEDED(v_buffer->Lock(0,number_of_vertices*sizeof(struct VERTEX_XYZ_NORMAL),(void **)&vertex_data,D3DLOCK_DISCARD)))
		assert(FALSE);
	for (int i=0; i<numVertex; i++)
	{
		vertex_data[i].position.x = mVertexes[i].x;
		vertex_data[i].position.y = mVertexes[i].y;
		vertex_data[i].position.z = mVertexes[i].z;
		vertex_data[i].normal.x = mNormal[i].x;
		vertex_data[i].normal.y = mNormal[i].y;
		vertex_data[i].normal.z = mNormal[i].z;
	}
	v_buffer->Unlock();
	SAFE_RELEASE(v_buffer);
}
//-----------------------------------------------------------------------
DWORD CLight::getFVF()
{
	return D3DFVF_XYZ_NORMAL;
}
//-----------------------------------------------------------------------
void CLight::singleLightShaderConfig()
{
	HRESULT hr;
	D3DXHANDLE handle;

	D3DXVECTOR4 Ke(0,0,1,1);
	D3DXVECTOR4 Ka(0,1,0,1);
	D3DXVECTOR4 Kd(1,1,0,1);
	D3DXVECTOR4 Ks(1,1,1,1);
	f32 Shininess = 5;

	D3DXVECTOR4 GlobalAmbient(0,1,0,1);
	D3DXVECTOR4 LightPosition(0,0,-10,1);
	D3DXVECTOR4 LightColor(1,1,1,1);

	LPD3DXCONSTANTTABLE constable;
	if(LightType::Vertex == mLightType)
	{
		constable = mVtxConstTable;
	}
	else
	{
		constable = mPxlConstTable;
	}

	//emissive
	//VS_SetVector("u_Ke", Ke);
	//Shader_SetVector(mD3DDevice,constable, "u_Ke", Ke);

	//ambient
	//VS_SetVector("u_Ka", Ka);
	//Shader_SetVector(mD3DDevice,constable, "u_Ka", Ka);

	//diffuse
	//VS_SetVector("u_Kd", Kd);
	//Shader_SetVector(mD3DDevice, constable, "u_Kd", Kd);

	//specular
	//VS_SetVector("u_Ks", Ks);
	Shader_SetVector(mD3DDevice, constable, "u_Ks", Ks);

	D3DXHANDLE u_Shininess = constable->GetConstantByName(0, "u_Shininess");
	hr = constable->SetFloat(mD3DDevice, u_Shininess, Shininess);

	Shader_SetVector(mD3DDevice, constable, "u_GlobalAmbient", GlobalAmbient);

	Shader_SetVector(mD3DDevice, constable, "u_LightColor", LightColor);

	Shader_SetVector(mD3DDevice, constable, "u_LightPosition", LightPosition);
}
//-----------------------------------------------------------------------
void CLight::twoLightShaderConfig()
{
	HRESULT hr;
	D3DXHANDLE handle;

	Material material;
	material.Ke = D3DXVECTOR3(0,0,0);
	material.Ka = D3DXVECTOR3(0,0,0);
	material.Kd = D3DXVECTOR3(1,1,1);
	material.Ks = D3DXVECTOR3(1,1,1);
	material.shininess = 5;

	Light lights[2];
	lights[0].position = D3DXVECTOR4(-10,0,-10, 0);
	lights[0].color = D3DXVECTOR4(1,0,0, 0);

	lights[1].position = D3DXVECTOR4(10,0,-10, 0);
	lights[1].color = D3DXVECTOR4(0,1,0, 0);

	LPD3DXCONSTANTTABLE constable;
	if(LightType::Vertex == mLightType)
	{
		constable = mVtxConstTable;
	}
	else
	{
		constable = mPxlConstTable;
	}

	D3DXHANDLE u_Material = constable->GetConstantByName(0, "u_Material");
	hr = constable->SetValue(mD3DDevice, u_Material, &material, sizeof(material));

	hr = mD3DDevice->SetVertexShaderConstantF(0, (float*)lights, sizeof(lights) / (sizeof(float) * 4));
// 	D3DXHANDLE u_Lights = constable->GetConstantByName(0, "u_Lights");
// 	int st = sizeof(lights);
// 	hr = constable->SetValue(mD3DDevice, u_Lights, lights, sizeof(Light) * 2);

}
//-----------------------------------------------------------------------
void CLight::attenuationLightShaderConfig()
{
	HRESULT hr;
	D3DXHANDLE handle;

	LPD3DXCONSTANTTABLE constable;
	if(LightType::Vertex == mLightType)
	{
		constable = mVtxConstTable;
	}
	else
	{
		constable = mPxlConstTable;
	}

	Material material;
	material.Ke = D3DXVECTOR3(0,0,0);
	material.Ka = D3DXVECTOR3(0,0,0);
	material.Kd = D3DXVECTOR3(1,1,1);
	material.Ks = D3DXVECTOR3(1,1,1);
	material.shininess = 5;

	LightExtra light;
	light.position = D3DXVECTOR3(-30,0,-30);
	light.color = D3DXVECTOR3(1,1,1);
	//attenuation = 1 / (light.kC + light.kL * d + light.kQ * d * d)
	light.kC = 0;
	light.kL = 0;
	light.kQ = 0.02f;
	light.direction = D3DXVECTOR3(1,0,1);
	light.cosInnerCone = 0.998f;
	light.cosOuterCone = 0.866f;

	D3DXHANDLE u_Material = constable->GetConstantByName(0, "u_Material");
	hr = constable->SetValue(mD3DDevice, u_Material, &material, sizeof(material));

	D3DXHANDLE u_Light = constable->GetConstantByName(0, "u_Light");
	hr = constable->SetValue(mD3DDevice, u_Light, &light, sizeof(light));
}
//-----------------------------------------------------------------------
void CLight::initMesh( LPD3DXPATCHMESH pMesh, FLOAT fTessLevel )
{
	f32 start = 15, span = 10;
	f32 x = -start, z = start;
	for (int i = 0; i < 16; i++)
	{
		generateMesh(pMesh, &mLightObject[i].mesh, i + 4);

		OwnVector3 pos(x, 0, z);
		mLightObject[i].modelMtx.makeTrans(pos);

		x += span;
		if(x > start)
		{
			x = -start;
			z -= span;
		}

	}
}
//-----------------------------------------------------------------------
void CLight::releaseForDeviceReset()
{

}
//-----------------------------------------------------------------------
void CLight::deviceRestore()
{
	mD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

}
//-----------------------------------------------------------------------
