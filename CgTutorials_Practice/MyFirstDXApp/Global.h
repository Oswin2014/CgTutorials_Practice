
#ifndef _Global_H
#define _Global_H

#include <d3dx9.h>

typedef unsigned int u32;
typedef unsigned __int64 u64;

typedef float f32;
typedef double f64;

struct VERTEX_XYZRHW_DIFFUSE
{
	FLOAT x, y, z/*, rhw*/; // The transformed position for the vertex.
	DWORD color;        // The vertex color.
};

struct VERTEX_TEXTURE
{
	D3DXVECTOR3 position;
	FLOAT		tu,tv;
	VERTEX_TEXTURE()
	{
		position = D3DXVECTOR3(0.0f,0.0f,0.0f);
		tu = tv = 0.0f;
	}
};

struct VERTEX_XYZ_NORMAL
{
	D3DXVECTOR3	position;
	D3DXVECTOR3	normal;
};

struct VERTEX_XYZ_NORMAL_TEX1
{
	D3DXVECTOR3	position;
	D3DXVECTOR3	normal;
	D3DXVECTOR2 texCoord;
};

struct VERTEX_XYZW_DIFFUSE
{
	D3DXVECTOR4 position;
	DWORD color;
	VERTEX_XYZW_DIFFUSE()
	{
		color = 0xff0000ff;
	}
};

struct Vertex_XYZ_Diffuse
{
	D3DXVECTOR3 position;
	DWORD color;
	Vertex_XYZ_Diffuse()
	{
		color = 0xff0000ff;
	}
};

//vertex has one texture.
struct VERTEX_XYZW_DIFFUSE_TEX1
{
	//D3DXVECTOR3 position;
	D3DXVECTOR4 position;
	DWORD color;
	FLOAT		tu,tv;
	VERTEX_XYZW_DIFFUSE_TEX1()
	{
		color = 0xff0000ff;
	}
};

struct Material
{
	D3DXVECTOR3 Ke;
	D3DXVECTOR3 Ka;
	D3DXVECTOR3 Kd;
	D3DXVECTOR3 Ks;
	float  shininess;
};

struct Light
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 color;
};

enum LightType
{
	Null,
	Vertex,
	Pixel,
};

struct LightExtra
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 color;
	f32			kC;
	f32			kL;
	f32			kQ;
	D3DXVECTOR3	direction;
	f32			cosInnerCone;
	f32			cosOuterCone;
};


#define D3DFVF_XYZ_DIFFUSE (D3DFVF_XYZ|D3DFVF_DIFFUSE)

#define D3DFVF_XYZ_TEX1 (D3DFVF_XYZ|D3DFVF_TEX1)
#define D3DFVF_XYZ_NORMAL (D3DFVF_XYZ|D3DFVF_NORMAL)
#define D3DFVF_XYZ_NORMAL_TEX1 (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

#define D3DFVF_XYZW_DIFFUSE ( D3DFVF_XYZW | D3DFVF_DIFFUSE )

//D3DFVF_TEX0 through D3DFVF_TEX8.
//Describle a vertex format includes no texture coordinates, or as many as eight sets.
#define D3DFVF_XYZW_DIFFUSE_TEX1 ( D3DFVF_XYZW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

//#define VERTEX_TYPE VERTEX_XYZW_DIFFUSE
#define VERTEX_TYPE Vertex_XYZ_Diffuse

//#define VERTEX(x, y, z, w) D3DXVECTOR4(x, y, z, w)
#define VERTEX(mX, mY, mZ, mW) D3DXVECTOR3(mX, mY, mZ)

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define SAFE_DELETE(p) { if(p) { delete p; p = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] p; p = NULL; } }
#define SAFE_RELEASE(p) { if(p) { p->Release(); p = NULL; } }

#define Shader_SetVector(device, constable, name, value)			\
	handle = constable->GetConstantByName(0, name);		\
	hr = constable->SetVector(device, handle, (D3DXVECTOR4*)&value);


#endif