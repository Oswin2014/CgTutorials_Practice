
#ifndef _OWN_MATH_H
#define _OWN_MATH_H


#include <math.h>
#include <memory.h>
#include "Global.h"

const double Pai = 3.141592653f;
const f32 Deg2Rad = Pai / (180.0f);
const f32 Rad2Deg = (180.0f) / Pai;

class OwnVector2
{
public: 
	f32 m_x, m_y;

public: 
	OwnVector2(){}
	~OwnVector2(){};

	inline OwnVector2(f32 x, f32 y)
	{
		m_x = x;
		m_y = y;
	}
};

class OwnVector3
{
public:
	f32 m_x, m_y, m_z;

	static const OwnVector3 ZERO;
	static const OwnVector3 UNIT_X;
	static const OwnVector3 UNIT_Y;
	static const OwnVector3 UNIT_Z;

public:
	OwnVector3(){ m_x = m_y = m_z = 0; }
	~OwnVector3(){};

	OwnVector3(f32 x, f32 y, f32 z)
	{
		set(x, y, z);
	}

	inline OwnVector3 operator - () const
	{
		return OwnVector3(-m_x, -m_y, -m_z);
	}

	OwnVector3 operator-(const OwnVector3& rhs) const
	{
		return OwnVector3(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z);
	}

	OwnVector3 operator + (const OwnVector3 rhs) const
	{
		return OwnVector3(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z);
	}

	OwnVector3 operator * (const f32 rhs) const
	{
		return OwnVector3(m_x * rhs, m_y * rhs, m_z * rhs);
	}

	OwnVector3& operator += (const OwnVector3& vec)
	{
		m_x += vec.m_x;
		m_y += vec.m_y;
		m_z += vec.m_z;

		return *this;
	}

	OwnVector3& operator /= (f32 div)
	{
		m_x /= div;
		m_y /= div;
		m_z /= div;
		return *this;
	}

	bool operator == (const OwnVector3& rhs) const
	{
		return (m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z);
	}

	f32 dotProduct(const OwnVector3& vec) const
	{
		return m_x * vec.m_x + m_y * vec.m_y + m_z * vec.m_z;
	}

	f32 length() const
	{
		return sqrtf(m_x * m_x + m_y * m_y + m_z * m_z);
	}

	void normalize()
	{
		f32 len = length();
		if(len > f32(0.0f))
		{
			*this /= len;
		}
	}

	void set(f32 x, f32 y, f32 z)
	{
		m_x = x; m_y = y; m_z = z;
	}

	inline void set(const OwnVector3& src)
	{
		m_x = src.m_x;
		m_y = src.m_y;
		m_z = src.m_z;
	}

	void setCross(const OwnVector3& lhs, const OwnVector3& rhs)
	{
		m_x = lhs.m_y * rhs.m_z - lhs.m_z * rhs.m_y;
		m_y = lhs.m_z * rhs.m_x - lhs.m_x * rhs.m_z;
		m_z = lhs.m_x * rhs.m_y - lhs.m_y * rhs.m_x;
	}
};


class OwnVertex4
{
public:
	f32 m_x, m_y, m_z, m_w;

public:
	OwnVertex4(){}
	~OwnVertex4(){};

	OwnVector3& normalize()
	{
		OwnVector3 dst;
		dst.m_x = m_x / m_w;
		dst.m_y = m_y / m_w;
		dst.m_z = m_z / m_w;
		return dst;
	}
};

class OwnMatrix4x4
{
public:
	union {
		struct {
			f32        m11, m12, m13, m14;
			f32        m21, m22, m23, m24;
			f32        m31, m32, m33, m34;
			f32        m41, m42, m43, m44;

		};
		f32 mtx[4][4];
	};

	OwnMatrix4x4()	{};
	~OwnMatrix4x4()	{};
	OwnMatrix4x4(const OwnMatrix4x4& src)	{ copy(src); }
	OwnMatrix4x4& operator= (const OwnMatrix4x4& src)	{ copy(src); return *this; }

	// member access, allows use of construct mtx[r][c]
	inline f32* operator[] (size_t row) const
	{
		return (f32*)mtx[row];
	}

	void copy(const OwnMatrix4x4& src)
	{
		memcpy(mtx, src.mtx, sizeof(mtx));
	}

	OwnVertex4& multiplyVertex3LH(const OwnVector3& vtx3)
	{
		OwnVertex4 dst;
		dst.m_x = vtx3.m_x * m11 + vtx3.m_y * m21 + vtx3.m_z * m31 + m41;
		dst.m_y = vtx3.m_x * m12 + vtx3.m_y * m22 + vtx3.m_z * m32 + m42;
		dst.m_z = vtx3.m_x * m13 + vtx3.m_y * m23 + vtx3.m_z * m33 + m43;
		dst.m_w = vtx3.m_x * m14 + vtx3.m_y * m24 + vtx3.m_z * m34 + m44;
		return dst;
	}

	OwnVertex4& multiplyVertex4LH(const OwnVertex4& vtx4)
	{
		OwnVertex4 dst;
		dst.m_x = vtx4.m_x * m11 + vtx4.m_y * m21 + vtx4.m_z * m31 + vtx4.m_w * m41;
		dst.m_y = vtx4.m_x * m12 + vtx4.m_y * m22 + vtx4.m_z * m32 + vtx4.m_w * m42;
		dst.m_z = vtx4.m_x * m13 + vtx4.m_y * m23 + vtx4.m_z * m33 + vtx4.m_w * m43;
		dst.m_w = vtx4.m_x * m14 + vtx4.m_y * m24 + vtx4.m_z * m34 + vtx4.m_w * m44;
		return dst;
	}

	void makeTrans(const OwnVector3& v);

	void rotateX(f32 radianX);

	void rotateY(f32 radianY);

	void rotateZ(f32 radianZ);

	void set(f32 _11, f32 _12, f32 _13, f32 _14,
		f32 _21, f32 _22, f32 _23, f32 _24,
		f32 _31, f32 _32, f32 _33, f32 _34,
		f32 _41, f32 _42, f32 _43, f32 _44){
			m11 = _11; m12 = _12; m13 = _13; m14 = _14;
			m21 = _21; m22 = _22; m23 = _23; m24 = _24;
			m31 = _31; m32 = _32; m33 = _33; m34 = _34;
			m41 = _41; m42 = _42; m43 = _43; m44 = _44;
	}

	void setIdentity(){
// 		m11 = 1; m12 = 0; m13 = 0; m14 = 0;
// 		m21 = 0; m22 = 1; m23 = 0; m24 = 0;
// 		m31 = 0; m32 = 0; m33 = 1; m34 = 0;
// 		m41 = 0; m42 = 0; m43 = 0; m44 = 1;

		int row, col;
		for(row = 0; row < 4; row++)
			for(col = 0; col < 4; col++)
				mtx[row][col] = (row == col);
	}

	void setProjectionTransLH(f32 radianFov, f32 aspectRatio, f32 zNear, f32 zFar);

	void setMultiply(const OwnMatrix4x4& mtx1, const OwnMatrix4x4& mtx2);

	void setLookAtLH(const OwnVector3&, const OwnVector3&, const OwnVector3&);

	void setPureRotateZXY( f32 radianX, f32 radianY, f32 radianZ);

	void setPureRotateZ(f32 radianY);

	inline OwnVector3& transformAffine(const OwnVector3& v) const
	{
		return OwnVector3(
			m11 * v.m_x + m21 * v.m_y + m31 * v.m_z + m41,
			m12 * v.m_x + m22 * v.m_y + m32 * v.m_z + m42,
			m13 * v.m_x + m23 * v.m_y + m33 * v.m_z + m43);
	}

	inline OwnMatrix4x4 transpose() const;
};

inline void OwnMatrix4x4::makeTrans(const OwnVector3& v)
{
	m11 = 1; m12 = 0; m13 = 0; m14 = 0;
	m21 = 0; m22 = 1; m23 = 0; m24 = 0;
	m31 = 0; m32 = 0; m33 = 1; m34 = 0;
	m41 = v.m_x; m42 = v.m_y; m43 = v.m_z; m44 = 1;
}

inline void OwnMatrix4x4::rotateX(f32 radianX)
{
	if(abs(radianX) < 0.0001f)
		return;

	f32 sin = sinf(radianX);
	f32 cos = cosf(radianX);

	f32 f1, f2;
	f1 = m12; f2 = m13;
	m12 = f1 * cos + f2 * (-sin);
	m13 = f1 * sin + f2 * cos;

	f1 = m22; f2 = m23;
	m22 = f1 * cos + f2 * (-sin);
	m23 = f1 * sin + f2 * cos;

	f1 = m32; f2 = m33;
	m32 = f1 * cos + f2 * (-sin);
	m33 = f1 * sin + f2 * cos;
}

inline void OwnMatrix4x4::rotateY(f32 radianY)
{
	if(abs(radianY) < 0.0001f)
		return;

	f32 sin = sinf(radianY);
	f32 cos = cosf(radianY);
	f32 f1, f2;
	
	f1 = m11; f2 = m13;
	m11 = f1 * cos + f2 * sin;
	m13 = f1 * (-sin) + f2 * cos;

	f1 = m21; f2 = m23;
	m21 = f1 * cos + f2 * sin;
	m23 = f1 * (-sin) + f2 * cos;

	f1 = m31; f2 = m33;
	m31 = f1 * cos + f2 * sin;
	m33 = f1 * (-sin) + f2 * cos;
}

inline void OwnMatrix4x4::rotateZ(f32 radianZ)
{
	if(abs(radianZ) < 0.0001f)
		return;

	f32 sin = sinf(radianZ);
	f32 cos = cosf(radianZ);
	f32 f1, f2;

	f1 = m11; f2 = m12;
	m11 = f1 * cos + f2 * (-sin);
	m12 = f1 * sin + f2 * cos;

	f1 = m21; f2 = m22;
	m21 = f1 * cos + f2 * (-sin);
	m22 = f1 * sin + f2 * cos;

	f1 = m31; f2 = m32;
	m31 = f1 * cos + f2 * (-sin);
	m32 = f1 * sin + f2 * cos;
}

inline void OwnMatrix4x4::setProjectionTransLH(float radianFov, float aspectRatio, float zNear, float zFar)
{
	float    h;
	float    w;
	float halffov = radianFov*0.5f;
	float s = sinf(halffov);
	float c = cosf(halffov);

	h = c / s;
	w = h / aspectRatio;

	m11 = w; m12 = 0; m13 = 0; m14 = 0;
	m21 = 0; m22 = h; m23 = 0; m24 = 0;

//	m31 = 0; m32 = 0; m33 = zFar / (zNear - zFar); m34 = -1;	//RH(Normal)
//	m41 = 0; m42 = 0; m43 = -zNear*m33; m44 = 0;

//	m41 = 0; m42 = 0; m43 = zNear*m33; m44 = 0;	//RH(DX)


	m31 = 0; m32 = 0; m33 = zFar / (zFar - zNear); m34 = 1;		//LH(DX)
	m41 = 0; m42 = 0; m43 = -zNear*m33; m44 = 0;
}

//right multiply
inline void OwnMatrix4x4::setMultiply(const OwnMatrix4x4& mtx1, const OwnMatrix4x4& mtx2)
{
	/*
	float x = mtx1.m11;
	float y = mtx1.m12;
	float z = mtx1.m13;
	float w = mtx1.m14;
	m11 = mtx2.m11*x + mtx2.m21*y + mtx2.m31*z + mtx2.m41*w;
	m12 = mtx2.m12*x + mtx2.m22*y + mtx2.m32*z + mtx2.m42*w;
	m13 = mtx2.m13*x + mtx2.m23*y + mtx2.m33*z + mtx2.m43*w;
	m14 = mtx2.m14*x + mtx2.m24*y + mtx2.m34*z + mtx2.m44*w;
	x = mtx1.m21;
	y = mtx1.m22;
	z = mtx1.m23;
	w = mtx1.m24;
	m21 = mtx2.m11*x + mtx2.m21*y + mtx2.m31*z + mtx2.m41*w;
	m22 = mtx2.m12*x + mtx2.m22*y + mtx2.m32*z + mtx2.m42*w;
	m23 = mtx2.m13*x + mtx2.m23*y + mtx2.m33*z + mtx2.m43*w;
	m24 = mtx2.m14*x + mtx2.m24*y + mtx2.m34*z + mtx2.m44*w;
	x = mtx1.m31;
	y = mtx1.m32;
	z = mtx1.m33;
	w = mtx1.m34;
	m31 = mtx2.m11*x + mtx2.m21*y + mtx2.m31*z + mtx2.m41*w;
	m32 = mtx2.m12*x + mtx2.m22*y + mtx2.m32*z + mtx2.m42*w;
	m33 = mtx2.m13*x + mtx2.m23*y + mtx2.m33*z + mtx2.m43*w;
	m34 = mtx2.m14*x + mtx2.m24*y + mtx2.m34*z + mtx2.m44*w;
	x = mtx1.m41;
	y = mtx1.m42;
	z = mtx1.m43;
	w = mtx1.m44;
	m41 = mtx2.m11*x + mtx2.m21*y + mtx2.m31*z + mtx2.m41*w;
	m42 = mtx2.m12*x + mtx2.m22*y + mtx2.m32*z + mtx2.m42*w;
	m43 = mtx2.m13*x + mtx2.m23*y + mtx2.m33*z + mtx2.m43*w;
	m44 = mtx2.m14*x + mtx2.m24*y + mtx2.m34*z + mtx2.m44*w;
	*/

	int row, col;

	for(row = 0; row < 4; row++)
		for(col = 0; col < 4; col++)
			mtx[row][col] = 
			mtx1.mtx[row][0] * mtx2.mtx[0][col] + 
			mtx1.mtx[row][1] * mtx2.mtx[1][col] + 
			mtx1.mtx[row][2] * mtx2.mtx[2][col] + 
			mtx1.mtx[row][3] * mtx2.mtx[3][col];
}

inline void OwnMatrix4x4::setLookAtLH(const OwnVector3& cameraPos, const OwnVector3& upDir, const OwnVector3& lookAtPos)
{
	OwnVector3 viewX, viewY, viewZ;
	viewZ = lookAtPos - cameraPos;
	viewZ.normalize();
	//no matter what the coordinate, the right hand cross multiply is always upDir to Zdir.
	//because the corss multiply use the z axis value in right hand coordinate.
	viewX.setCross(upDir, viewZ);
	viewX.normalize();
	viewY.setCross(viewZ, viewX);
	
	m11 = viewX.m_x; m12 = viewY.m_x; m13 = viewZ.m_x; m14 = 0;
	m21 = viewX.m_y; m22 = viewY.m_y; m23 = viewZ.m_y; m24 = 0;
	m31 = viewX.m_z; m32 = viewY.m_z; m33 = viewZ.m_z; m34 = 0;
	m41 = -viewX.dotProduct(cameraPos); m42 = -viewY .dotProduct(cameraPos); m43 = -viewZ.dotProduct(cameraPos); m44 = 1;
}

//just for camera control rotate style
//let camera z axis overlap world z axis, because y axis is always updir, 
//z rotate is not need, so don't set radianZ;
//after rotate x first, because y axis is always updir, so rotate x not influence y rotate,
//but if you rotate y first, then rotate world x axis is not like rotate self x axis.
inline void OwnMatrix4x4::setPureRotateZXY(f32 radianX, f32 radianY, f32 radianZ)
{
	setPureRotateZ(radianZ);
	rotateX(radianX);
	rotateY(radianY);
}

inline void OwnMatrix4x4::setPureRotateZ(f32 radianY)
{
	f32 sin = sinf(radianY);
	f32 cos = cosf(radianY);

	setIdentity();

	m11 = cos; m12 = sin;
	m21 = -sin; m22 = cos;
}

inline OwnMatrix4x4 OwnMatrix4x4::transpose() const
{
	OwnMatrix4x4 transpose;

	for(int row = 0; row < 4; row++)
		for(int col = 0; col < 4; col++)
			transpose[row][col] = mtx[col][row];

	return transpose;
}

#endif