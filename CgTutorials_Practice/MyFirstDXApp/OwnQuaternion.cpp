#include "OwnQuaternion.h"



//-----------------------------------------------------------------------
OwnQuaternion::~OwnQuaternion(void)
{
}
//-----------------------------------------------------------------------
void OwnQuaternion::fromAngleAxis( f32 radian, const OwnVector3& rotateAxis )
{
	//q (cos(r/2), rotateAxis * sin(r/2))

	f32 halfRadian = 0.5 * radian;
	f32 sin = sinf(halfRadian);

	mW = cosf(halfRadian);
	mX = sin * rotateAxis.m_x;
	mY = sin * rotateAxis.m_y;
	mZ = sin * rotateAxis.m_z;
}
//-----------------------------------------------------------------------
void OwnQuaternion::fromRotateMatrix( const OwnMatrix4x4& rotMtx )
{
	OwnMatrix4x4 rotateMtx = rotMtx;

	f32 fTrace = rotateMtx[0][0]+rotateMtx[1][1]+rotateMtx[2][2];
	f32 fRoot;

	if ( fTrace > 0.0 )
	{
		// |w| > 1/2, may as well choose w > 1/2
		fRoot = sqrtf(fTrace + 1.0f);  // 2w
		mW = 0.5f*fRoot;
		fRoot = 0.5f/fRoot;  // 1/(4w)
		mX = (rotateMtx[2][1]-rotateMtx[1][2])*fRoot;
		mY = (rotateMtx[0][2]-rotateMtx[2][0])*fRoot;
		mZ = (rotateMtx[1][0]-rotateMtx[0][1])*fRoot;
	}
	else
	{
		// |w| <= 1/2
		static size_t s_iNext[3] = { 1, 2, 0 };
		size_t i = 0;
		if ( rotateMtx[1][1] > rotateMtx[0][0] )
			i = 1;
		if ( rotateMtx[2][2] > rotateMtx[i][i] )
			i = 2;
		size_t j = s_iNext[i];
		size_t k = s_iNext[j];

		fRoot = sqrtf(rotateMtx[i][i]-rotateMtx[j][j]-rotateMtx[k][k] + 1.0f);
		f32* apkQuat[3] = { &mX, &mY, &mZ };
		*apkQuat[i] = 0.5f*fRoot;
		fRoot = 0.5f/fRoot;
		mW = (rotateMtx[k][j]-rotateMtx[j][k])*fRoot;
		*apkQuat[j] = (rotateMtx[j][i]+rotateMtx[i][j])*fRoot;
		*apkQuat[k] = (rotateMtx[k][i]+rotateMtx[i][k])*fRoot;
	}
}
//-----------------------------------------------------------------------
void OwnQuaternion::fromAxis( const OwnVector3& axisX, const OwnVector3& axisY, const OwnVector3& axisZ )
{
	OwnMatrix4x4 rotMtx;
	//vertex matrix is row vector state, the rotate matrix column 1,2,3 corresponding to axis x,y,z.
	//save a row vector transform matrix to a quaternion.

	rotMtx[0][0] = axisX.m_x;
	rotMtx[1][0] = axisX.m_y;
	rotMtx[2][0] = axisX.m_z;

	rotMtx[0][1] = axisY.m_x;
	rotMtx[1][1] = axisY.m_y;
	rotMtx[2][1] = axisY.m_z;

	rotMtx[0][2] = axisZ.m_x;
	rotMtx[1][2] = axisZ.m_y;
	rotMtx[2][2] = axisZ.m_z;
	
	fromRotateMatrix(rotMtx);
}
//-----------------------------------------------------------------------
OwnQuaternion OwnQuaternion::operator*( const OwnQuaternion& rhs ) const
{
	return OwnQuaternion
		(
		mW * rhs.mW - mX * rhs.mX - mY * rhs.mY - mZ * rhs.mZ,
		mW * rhs.mX + mX * rhs.mW + mY * rhs.mZ - mZ * rhs.mY,
		mW * rhs.mY + mY * rhs.mW + mZ * rhs.mX - mX * rhs.mZ,
		mW * rhs.mZ + mZ * rhs.mW + mX * rhs.mY - mY * rhs.mX
		);
}
//-----------------------------------------------------------------------
OwnQuaternion OwnQuaternion::operator*( f32 scalar ) const
{
	return OwnQuaternion(scalar * mW, scalar * mX, scalar * mY, scalar * mZ);
}
//-----------------------------------------------------------------------
void OwnQuaternion::toRotateMatrix( OwnMatrix4x4& rotMtx ) const
{
	rotMtx.setIdentity();

	f32 fTx  = mX+mX;
	f32 fTy  = mY+mY;
	f32 fTz  = mZ+mZ;
	f32 fTwx = fTx*mW;
	f32 fTwy = fTy*mW;
	f32 fTwz = fTz*mW;
	f32 fTxx = fTx*mX;
	f32 fTxy = fTy*mX;
	f32 fTxz = fTz*mX;
	f32 fTyy = fTy*mY;
	f32 fTyz = fTz*mY;
	f32 fTzz = fTz*mZ;

	rotMtx[0][0] = 1.0f-(fTyy+fTzz);
	rotMtx[0][1] = fTxy-fTwz;
	rotMtx[0][2] = fTxz+fTwy;
	rotMtx[1][0] = fTxy+fTwz;
	rotMtx[1][1] = 1.0f-(fTxx+fTzz);
	rotMtx[1][2] = fTyz-fTwx;
	rotMtx[2][0] = fTxz-fTwy;
	rotMtx[2][1] = fTyz+fTwx;
	rotMtx[2][2] = 1.0f-(fTxx+fTyy);

}
//-----------------------------------------------------------------------
void OwnQuaternion::toAxes( OwnVector3& axisX, OwnVector3& axisY, OwnVector3& axisZ ) const
{
	OwnMatrix4x4 rotMtx;
	toRotateMatrix(rotMtx);

	//vertex matrix is row vector state, the rotate matrix column 1,2,3 corresponding to axis x,y,z.
	//quaternion save a row vector transform matrix.
	
	axisX.m_x = rotMtx[0][0];
	axisX.m_y = rotMtx[1][0];
	axisX.m_z = rotMtx[2][0];

	axisY.m_x = rotMtx[0][1];
	axisY.m_y = rotMtx[1][1];
	axisY.m_z = rotMtx[2][1];

	axisZ.m_x = rotMtx[0][2];
	axisZ.m_y = rotMtx[1][2];
	axisZ.m_z = rotMtx[2][2];
}
//-----------------------------------------------------------------------
f32 OwnQuaternion::norm() const
{
	return mW * mW + mX * mX + mY * mY + mZ * mZ;
}
//-----------------------------------------------------------------------
void OwnQuaternion::normalize()
{
	f32 len = norm();
	f32 factor = 1.0f / sqrtf(len);
	*this = *this * factor;
}
//-----------------------------------------------------------------------