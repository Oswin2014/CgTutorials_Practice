#include "VertexTransform.h"

#include "OwnCamera.h"

CVertexTransform::CVertexTransform(void)
{
}
//-----------------------------------------------------------------------
CVertexTransform::CVertexTransform( LPDIRECT3DDEVICE9 pdevice )
	: CChapterBase(pdevice)
{
}
//-----------------------------------------------------------------------
CVertexTransform::~CVertexTransform(void)
{
}
//-----------------------------------------------------------------------
void CVertexTransform::draw()
{
	mD3DDevice->SetFVF(D3DFVF_XYZ_DIFFUSE);
	mD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST,3,mAxis, sizeof(VERTEX_TYPE));
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 6, mArrow1, sizeof(VERTEX_TYPE));
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 6, mArrow2, sizeof(VERTEX_TYPE));
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 4, mArrow3, sizeof(VERTEX_TYPE));
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, mTriangleX, sizeof(VERTEX_TYPE));
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, mTriangleZ, sizeof(VERTEX_TYPE));
}
//-----------------------------------------------------------------------
HRESULT CVertexTransform::initScene()
{
	mArrow1[0].position = VERTEX( 0,  0, 0, 1.0f);
	mArrow1[1].position = VERTEX(-1,  0, 0, 1.0f);
	mArrow1[2].position = VERTEX(-1, -1, 0, 1.0f);
	mArrow1[3].position = VERTEX( 0, -1, 0, 1.0f);
	mArrow1[4].position = VERTEX( 0, -1, 4, 1.0f);
	mArrow1[5].position = VERTEX( 0,  0, 4, 1.0f);
	mArrow1[6].position = VERTEX(-1,  0, 4, 1.0f);
	mArrow1[7].position = VERTEX(-1,  0, 0, 1.0f);

	mArrow2[0].position = VERTEX(-1, -1, 4, 1.0f);
	mArrow2[1].position = VERTEX( 0, -1, 4, 1.0f);
	mArrow2[2].position = VERTEX( 0,  0, 4, 1.0f);
	mArrow2[3].position = VERTEX(-1,  0, 4, 1.0f);
	mArrow2[4].position = VERTEX(-1,  0, 0, 1.0f);
	mArrow2[5].position = VERTEX(-1, -1, 0, 1.0f);
	mArrow2[6].position = VERTEX( 0, -1, 0, 1.0f);
	mArrow2[7].position = VERTEX( 0, -1, 4, 1.0f);

	mArrow3[0].position = VERTEX(-0.5f, -0.5f, 6, 1.0f);
	mArrow3[1].position = VERTEX(-1, -1, 4, 1.0f);
	mArrow3[2].position = VERTEX( 0, -1, 4, 1.0f);
	mArrow3[3].position = VERTEX( 0,  0, 4, 1.0f);
	mArrow3[4].position = VERTEX(-1,  0, 4, 1.0f);
	mArrow3[5].position = VERTEX(-1, -1, 4, 1.0f);
	mArrow3[0].color = 0xffff0000;
	mArrow3[1].color = 0xffff0000;
	mArrow3[2].color = 0xffff0000;
	mArrow3[3].color = 0xffff0000;
	mArrow3[4].color = 0xffff0000;
	mArrow3[5].color = 0xffff0000;

	mTriangleX[0].position = VERTEX( 4,  1, 0, 1.0f);
	mTriangleX[1].position = VERTEX( 4, -1, 1, 1.0f);
	mTriangleX[2].position = VERTEX( 4, -1,-2, 1.0f);

	mTriangleZ[0].position = VERTEX(  0,  1, 4, 1.0f);
	mTriangleZ[1].position = VERTEX(  1, -1, 4, 1.0f);
	mTriangleZ[2].position = VERTEX( -2, -1, 4, 1.0f);
	mTriangleZ[0].color = 0xffff0000;
	mTriangleZ[1].color = 0xff00ff00;
	mTriangleZ[2].color = 0xff0000ff;

	mAxis[0].position = VERTEX( 0.0f,   0.0f,  0.0f, 1.0f);
	mAxis[1].position = VERTEX( 50.0f, 0.0f,  0.0f, 1.0f);
	mAxis[2].position = VERTEX( 0.0f,   0.0f,  0.0,  1.0f);
	mAxis[3].position = VERTEX( 0.0f,   50,   0.0f, 1.0f);
	mAxis[4].position = VERTEX( 0.0f,   0.0f,  0.0f, 1.0f);
	mAxis[5].position = VERTEX( 0.0f,   0.0f,  50,  1.0f);
	mAxis[0].color = 0xffff0000;
	mAxis[1].color = 0xffff0000;
	mAxis[2].color = 0xff00ff00;
	mAxis[3].color = 0xff00ff00;
	mAxis[4].color = 0xff0000ff;
	mAxis[5].color = 0xff0000ff;

	return S_OK;
}
//-----------------------------------------------------------------------
void CVertexTransform::shaderConfig( LPD3DXCONSTANTTABLE pVtxConstTable /*= NULL*/, LPD3DXCONSTANTTABLE pPxlConstTable /*= NULL*/ )
{
	float tmp = sqrtf(2) / 2;
	OwnMatrix4x4 modelMtx, viewProjMtx, modelViewProjMtx;
	// 	modelMtx.set(
	// 		 tmp,	-0.5f,	  tmp,	0,
	// 		   0,	  tmp,	 0.5f,	0,
	// 		-tmp,	-0.5f,   0.5f,	0,
	// 		 tmp,	 0.5f,	-0.5f,	1);
	modelMtx.set(
		10, 0,  0, 0,
		0, 10,  0, 0,
		0, 0,  10, 0,
		0, 0,  0, 1);
	//modelMtx.setIdentity();


	OwnCameraNormal cam;
	viewProjMtx.setMultiply(cam.mViewTrans, cam.mProjectionTrans);
	modelViewProjMtx.setMultiply(modelMtx, viewProjMtx);

	//////////////////////////////////////////////////////////////////////////

	OwnVector3 a(-0.5f, -0.5f, 6);
	OwnVector3 b(-1, -1, 4);
	OwnVector3 c(0, -1, 4);

	OwnVertex4 wm = modelMtx.multiplyVertex3LH(a);
	OwnVertex4 vm = cam.mViewTrans.multiplyVertex4LH(wm);
	OwnVertex4 pm = cam.mProjectionTrans.multiplyVertex4LH(vm);

	OwnVector3 real = pm.normalize();

	OwnVertex4 test = modelViewProjMtx.multiplyVertex3LH(a);
	//////////////////////////////////////////////////////////////////////////

	D3DXHANDLE modelViewProj = pVtxConstTable->GetConstantByName(0, "modelViewProj");
	HRESULT hr = pVtxConstTable->SetMatrix(mD3DDevice, modelViewProj, (D3DXMATRIX*)&modelViewProjMtx);
}
//-----------------------------------------------------------------------
std::string CVertexTransform::getVSFunctionName()
{
	return "C4E1_Transform";
}
//-----------------------------------------------------------------------
std::string CVertexTransform::getPSFunctionName()
{
	return "C2E2f_passthrough";
}
//-----------------------------------------------------------------------
void CVertexTransform::releaseForDeviceReset()
{

}
//-----------------------------------------------------------------------
void CVertexTransform::deviceRestore()
{

}
//-----------------------------------------------------------------------