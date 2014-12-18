#include "TessellateSample.h"

#include <assert.h>
#include "Global.h"

CTessellateSample::CTessellateSample(void)
{
}
//-----------------------------------------------------------------------
CTessellateSample::CTessellateSample( LPDIRECT3DDEVICE9 pdevice )
	: CChapterBase(pdevice)
	, mMesh(NULL)
{
	mShaderUse = false;
}
//-----------------------------------------------------------------------
CTessellateSample::~CTessellateSample(void)
{
	SAFE_RELEASE(mMesh);
}
//-----------------------------------------------------------------------
void CTessellateSample::draw()
{
	mD3DDevice->SetFVF(getFVF());

	// set the world transform
// 	static float index = 0.0f; index+=0.03f; // an ever-increasing float value
// 	D3DXMATRIX matRotateY;    // a matrix to store the rotation for each triangle
// 	D3DXMatrixRotationY(&matRotateY, index);    // the rotation matrix
// 	mD3DDevice->SetTransform(D3DTS_WORLD, &(matRotateY));    // set the world transform

	if(mMesh)
		mMesh->DrawSubset(0);
}
//-----------------------------------------------------------------------
HRESULT CTessellateSample::initScene()
{
	mD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	initPatch("teacup", 10);

	createMatricesTransform();

	return S_OK;
}
//-----------------------------------------------------------------------
void CTessellateSample::loadResource( char *filename, int *patches, int *vertices )
{
	int ii;
	float x,y,z;
	int a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;

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
	for (ii = 1; ii <= *vertices; ii++) {
		(void)fscanf(fp,"%f, %f, %f\n",&x,&y,&z);
		loadVertex(ii, (double)x,(double)y,(double)z);
	}

	fclose(fp);
}
//-----------------------------------------------------------------------
void CTessellateSample::loadPatch( int ii, int a, int b, int c, int d, int e, int f, int g, 
	int h, int i, int j, int k, int l, int m, int n, int o, int p )
{
	assert(ii < MAX_PATCHES);
	mPatches[ii][0] = a-1;
	mPatches[ii][1] = b-1;
	mPatches[ii][2] = c-1;
	mPatches[ii][3] = d-1;
	mPatches[ii][4] = e-1;
	mPatches[ii][5] = f-1;
	mPatches[ii][6] = g-1;
	mPatches[ii][7] = h-1;
	mPatches[ii][8] = i-1;
	mPatches[ii][9] = j-1;
	mPatches[ii][10] = k-1;
	mPatches[ii][11] = l-1;
	mPatches[ii][12] = m-1;
	mPatches[ii][13] = n-1;
	mPatches[ii][14] = o-1;
	mPatches[ii][15] = p-1;
	assert(VERTEX_PER_PATCH==16);
}
//-----------------------------------------------------------------------
void CTessellateSample::loadVertex( int ii, double x, double y, double z )
{
	ii--;
	assert(ii < MAX_VERTEX);
	mVertexes[ii].x = x;
	/*** Y AND Z FLIPPED ***/
// 	mVertexes[ii].y = z;
// 	mVertexes[ii].z = y;
	mVertexes[ii].y = y;
	mVertexes[ii].z = z;
}
//-----------------------------------------------------------------------
void CTessellateSample::createMatricesTransform()
{
	// set the view transform
	D3DXMATRIX matView;    // the view transform matrix
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3 (0.0f, 5.0f, 10.0f),    // the camera position
		&D3DXVECTOR3 (0.0f, 0.0f, 0.0f),      // the look-at position
		&D3DXVECTOR3 (0.0f, 1.0f, 0.0f));    // the up direction
	mD3DDevice->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

	// set the projection transform
	D3DXMATRIX matProjection;    // the projection transform matrix
	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),    // the horizontal field of view
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
		1.0f,   // the near view-plane
		100.0f);    // the far view-plane
	mD3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection); // set the projection
}
//-----------------------------------------------------------------------
void CTessellateSample::initPatch(const std::string& fileName, float tessLevel)
{
	// load patch
	char filename[255];
	sprintf_s(filename,fileName.c_str());

	int numPatches = -1, numVertex;

	loadResource(filename,&numPatches,&numVertex);
	printf("Loaded patch %s with %d patches and %d vertices.\n",
		filename,numPatches,numVertex);

	// create declarator from FVF
	D3DVERTEXELEMENT9 inDecl[MAX_FVF_DECL_SIZE];
	if (!SUCCEEDED(D3DXDeclaratorFromFVF(getFVF(),inDecl)))
		assert(FALSE);    

	// create patch mesh
	LPD3DXPATCHMESH p_mesh;
	D3DXPATCHINFO info;
	info.PatchType = D3DXPATCHMESH_RECT;
	info.Degree = D3DDEGREE_CUBIC;
	info.Basis = D3DBASIS_BEZIER;

	if (!SUCCEEDED(D3DXCreatePatchMesh(&info,numPatches,numVertex,0,inDecl,mD3DDevice,&p_mesh))) //numPatches-32 numVertex-306
		assert(FALSE);
	assert(p_mesh->GetControlVerticesPerPatch()==VERTEX_PER_PATCH);

	try
	{
		copyVertices(p_mesh, numVertex);

		// copy indices
		LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;
		if (!SUCCEEDED(p_mesh->GetIndexBuffer(&i_buffer))) 
			assert(FALSE);
		D3DINDEXBUFFER_DESC i_buffer_desc;
		if (!SUCCEEDED(i_buffer->GetDesc(&i_buffer_desc)))
			assert(FALSE);
		assert(i_buffer_desc.Size==numPatches*VERTEX_PER_PATCH*sizeof(WORD));
		WORD* index_data = NULL;
		if (!SUCCEEDED(i_buffer->Lock(0,0,(void **)&index_data,0/*D3DLOCK_DISCARD*/)))
			assert(FALSE);
		int idx=0;
		for (int i=0; i<numPatches; i++)
		{
			for (int j=0; j<VERTEX_PER_PATCH; j++)
			{
				index_data[idx] = mPatches[i][j];
				idx++;
			}
		}
		//memcpy(index_data, mPatches, i_buffer_desc.Size);
		i_buffer->Unlock();

		SAFE_RELEASE(i_buffer);

	}
	catch (std::exception e)
	{
		auto test = e;
	}
	// create mMesh for tesselation

	//FLOAT fTessLevel=10.0f;
	FLOAT fTessLevel=tessLevel;

	try
	{
		initMesh(p_mesh, fTessLevel);
	}
	catch (std::exception e)
	{
		auto test = e;
	}
	catch(...)
	{
		int a = 0;
		exit(0);
	}

	SAFE_RELEASE(p_mesh);
}
//-----------------------------------------------------------------------
void CTessellateSample::copyVertices( LPD3DXPATCHMESH pMesh, DWORD numVertex )
{
	// copy vertices
	LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
	if (!SUCCEEDED(pMesh->GetVertexBuffer(&v_buffer))) 
		assert(FALSE);
	struct Vertex_XYZ_Diffuse* vertex_data = NULL;
	DWORD number_of_vertices=pMesh->GetNumVertices();
	assert(number_of_vertices==numVertex);
	if (!SUCCEEDED(v_buffer->Lock(0,number_of_vertices*sizeof(struct Vertex_XYZ_Diffuse),(void **)&vertex_data,0/*D3DLOCK_DISCARD*/)))
		assert(FALSE);
// 	Vertex_XYZ_Diffuse tempVertex[1000];
// 	for (int i=0; i<numVertex; i++)
// 	{
// 		tempVertex[i].position.x = mVertexes[i].x;
// 		tempVertex[i].position.y = mVertexes[i].y;
// 		tempVertex[i].position.z = mVertexes[i].z;
// 		tempVertex[i].color = D3DCOLOR_XRGB(255,0,0);
// 	}
	for (int i=0; i<numVertex; i++)
	{
		vertex_data[i].position.x = mVertexes[i].x;
		vertex_data[i].position.y = mVertexes[i].y;
		vertex_data[i].position.z = mVertexes[i].z;
		vertex_data[i].color = D3DCOLOR_XRGB(255,0,0);
	}
	//memcpy(vertex_data, tempVertex, number_of_vertices*sizeof(struct Vertex_XYZ_Diffuse));
	v_buffer->Unlock();

	SAFE_RELEASE(v_buffer);
}
//-----------------------------------------------------------------------
DWORD CTessellateSample::getFVF()
{
	return D3DFVF_XYZ_DIFFUSE;
}
//-----------------------------------------------------------------------
void CTessellateSample::generateMesh(LPD3DXPATCHMESH pMesh, LPD3DXMESH *dstMesh, FLOAT fTessLevel)
{
	DWORD Adaptive=FALSE;
	DWORD NumTriangles,NumVertices;
	if (!SUCCEEDED(pMesh->GetTessSize(fTessLevel,Adaptive,&NumTriangles,&NumVertices)))
		assert(FALSE);
	if (!SUCCEEDED(D3DXCreateMeshFVF(NumTriangles,NumVertices,D3DXMESH_MANAGED /*D3DXMESH_DYNAMIC*/,getFVF(),mD3DDevice, dstMesh)))
		assert(FALSE);

	// tesselate
	assert(Adaptive==FALSE);
	//!!!!!!! An unhandled exception occurred when IDirect3DIndexBuffer9::Lock in this step.
	if (!SUCCEEDED(pMesh->Tessellate(fTessLevel, *dstMesh)))
		assert(FALSE);
}
//-----------------------------------------------------------------------
void CTessellateSample::initMesh( LPD3DXPATCHMESH pMesh, FLOAT fTessLevel )
{
	generateMesh(pMesh, &mMesh, fTessLevel);
}
//-----------------------------------------------------------------------
void CTessellateSample::releaseForDeviceReset()
{
	//SAFE_RELEASE(mMesh);
}
//-----------------------------------------------------------------------
void CTessellateSample::deviceRestore()
{
	mD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	createMatricesTransform();

	//D3DXMESH_MANAGED the vertex buff data will not lost, but D3DXMESH_DYNAMIC will lost.
	//initPatch("teacup", 10);
}
//-----------------------------------------------------------------------
