#include "Animation.h"

#include <assert.h>
#include "OwnCamera.h"
#include "Global.h"


//-----------------------------------------------------------------------
CAnimation::CAnimation(void)
{
}
//-----------------------------------------------------------------------
CAnimation::CAnimation( LPDIRECT3DDEVICE9 pdevice )
	: CTessellateSample(pdevice)
	, mVtxConstTable(NULL)
	, mPxlConstTable(NULL)
	, mTextureShesays(NULL)
{
	mShaderUse = true;

	mCamera = new OwnCameraQuaternion();
	mCamera->setPosition(OwnVector3(0,0,-10));
}
//-----------------------------------------------------------------------
CAnimation::~CAnimation(void)
{
	SAFE_RELEASE(mTextureShesays);
}
//-----------------------------------------------------------------------
void CAnimation::draw()
{
	shaderApply(NULL);

	CTessellateSample::draw();

}
//-----------------------------------------------------------------------
void CAnimation::deviceRestore()
{
	mD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	mD3DDevice->SetTexture( 1, mTextureShesays );
}
//-----------------------------------------------------------------------
std::string CAnimation::getVSFunctionName()
{
	return "C6E1_pulsate";
}
//-----------------------------------------------------------------------
std::string CAnimation::getPSFunctionName()
{
	return "Color_Texture";
}
//-----------------------------------------------------------------------
HRESULT CAnimation::initScene()
{
	mD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	initPatch("AnimationObj", 15);

	if(FAILED( D3DXCreateTextureFromFile( mD3DDevice, L"..\\..\\Media\\she says.jpg", &mTextureShesays )))
		return E_FAIL;
	mD3DDevice->SetTexture( 1, mTextureShesays );

	return S_OK;
}
//-----------------------------------------------------------------------
void CAnimation::releaseForDeviceReset()
{

}
//-----------------------------------------------------------------------
void CAnimation::shaderConfig( LPD3DXCONSTANTTABLE pVtxConstTable /*= NULL*/, LPD3DXCONSTANTTABLE pPxlConstTable /*= NULL*/ )
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

	twoLightShaderConfig();

	mTimer.timeStart();
}
//-----------------------------------------------------------------------
void CAnimation::copyVertices( LPD3DXPATCHMESH pMesh, DWORD numVertex )
{
	// copy vertices
	LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
	if (!SUCCEEDED(pMesh->GetVertexBuffer(&v_buffer))) 
		assert(FALSE);
	struct VERTEX_XYZ_NORMAL_TEX1* vertex_data = NULL;
	DWORD number_of_vertices=pMesh->GetNumVertices();
	assert(number_of_vertices==numVertex);
	if (!SUCCEEDED(v_buffer->Lock(0,number_of_vertices*sizeof(struct VERTEX_XYZ_NORMAL_TEX1),(void **)&vertex_data,D3DLOCK_DISCARD)))
		assert(FALSE);
	for (int i=0; i<numVertex; i++)
	{
		vertex_data[i].position.x = mVertexes[i].x;
		vertex_data[i].position.y = mVertexes[i].y;
		vertex_data[i].position.z = mVertexes[i].z;
		vertex_data[i].normal.x = mNormal[i].x;
		vertex_data[i].normal.y = mNormal[i].y;
		vertex_data[i].normal.z = mNormal[i].z;
		vertex_data[i].texCoord.x = mTexCoords[i].x;
		vertex_data[i].texCoord.y = mTexCoords[i].y;
	}
	v_buffer->Unlock();
	SAFE_RELEASE(v_buffer);
}
//-----------------------------------------------------------------------
DWORD CAnimation::getFVF()
{
	return D3DFVF_XYZ_NORMAL_TEX1;
}
//-----------------------------------------------------------------------
void CAnimation::loadResource( char *filename, int *patches, int *vertices )
{
	int ii;
	float x,y,z;
	int a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;
	OwnVector3 normal;
	OwnVector2 texCoord;

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
		(void)fscanf(fp,"%f, %f, %f, %f, %f, %f, %f, %f\n",
			&x,&y,&z, &normal.m_x, &normal.m_y, &normal.m_z, &texCoord.m_x, &texCoord.m_y);
		loadVertex(ii, x,y,z, normal, texCoord);
	}

	fclose(fp);
}
//-----------------------------------------------------------------------
void CAnimation::loadVertex( int ii, float x, float y, float z, const OwnVector3& normal, const OwnVector2& texCoord)
{
	mVertexes[ii].x = x;
	mVertexes[ii].y = y;
	mVertexes[ii].z = z;

	mNormal[ii].x = normal.m_x;
	mNormal[ii].y = normal.m_y;
	mNormal[ii].z = normal.m_z;

	mTexCoords[ii].x = texCoord.m_x;
	mTexCoords[ii].y = texCoord.m_y;
}
//-----------------------------------------------------------------------
void CAnimation::twoLightShaderConfig()
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

	LPD3DXCONSTANTTABLE constable = mVtxConstTable;

	D3DXHANDLE u_Material = constable->GetConstantByName(0, "u_Material");
	hr = constable->SetValue(mD3DDevice, u_Material, &material, sizeof(material));

	//SetVertexShaderConstantF first paramater is shader data constant start register,
	//if there use fix value like 0, we need "Light u_Lights[2] : register(c0);" in shader script.
	hr = mD3DDevice->SetVertexShaderConstantF(0, (float*)lights, sizeof(lights) / (sizeof(float) * 4));

}
//-----------------------------------------------------------------------
void CAnimation::shaderApply( OwnMatrix4x4 *pModelMtx /*= NULL*/ )
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

		LPD3DXCONSTANTTABLE constable = mVtxConstTable;

		Shader_SetVector(mD3DDevice, constable, "u_EyePosition", EyePosition);

		mTimer.timeEnd();
		hr = mVtxConstTable->SetFloat( mD3DDevice, 
			mVtxConstTable->GetConstantByName(0, "$time"), mTimer.mSec );
	}

}
//-----------------------------------------------------------------------