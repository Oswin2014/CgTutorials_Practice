

#ifndef _OWN_QUATERNION_H
#define _OWN_QUATERNION_H

#include "OwnMath.h"

class OwnQuaternion
{
public:
	inline OwnQuaternion(void)
		: mW(1), mX(0), mY(0), mZ(0)
	{
	}
	~OwnQuaternion(void);

	inline OwnQuaternion(f32 fW, f32 fX, f32 fY, f32 fZ)
		: mW(fW), mX(fX), mY(fY), mZ(fZ)
	{
	}

	inline OwnQuaternion(f32 radian, const OwnVector3& rotateAxis)
	{
		this->fromAngleAxis(radian, rotateAxis);
	}

	inline OwnQuaternion(const OwnMatrix4x4& rotMtx)
	{
		this->fromRotateMatrix(rotMtx);
	}

	inline OwnQuaternion(const OwnVector3& axisX, const OwnVector3& axisY, const OwnVector3& axisZ)
	{
		this->fromAxis(axisX, axisY, axisZ);
	}

	OwnQuaternion operator * (const OwnQuaternion& rhs) const;

	OwnQuaternion operator * (f32 scalar) const;

	void fromAngleAxis(f32 radian, const OwnVector3& rotateAxis);

	void fromRotateMatrix(const OwnMatrix4x4& rotMtx);

	void fromAxis(const OwnVector3& axisX, const OwnVector3& axisY, const OwnVector3& axisZ);

	f32 norm() const;

	void normalize();

	void toRotateMatrix(OwnMatrix4x4& rotMtx) const;

	void toAxes(OwnVector3& axisX, OwnVector3& axisY, OwnVector3& axisZ) const;


public:
	f32 mW, mX, mY, mZ;
};

#endif