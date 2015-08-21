
#ifndef _OWN_CAMERA_H
#define _OWN_CAMERA_H

#include <stdio.h>
#include "OwnMath.h"
#include "OwnQuaternion.h"


class OwnCamera
{
public:
	OwnCamera()
		: mFov(0.85f)
		, mAspect((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)
		, mZNear(0.2f)
		, mZFar(4000)
		, mCameraPos(OwnVector3::ZERO)
		, mYawFixed(true)
		, mYawFixedAxis(OwnVector3::UNIT_Y)
		, mTargetDis(10)
	{
		mProjectionTrans.setProjectionTransLH(mFov, mAspect, mZNear, mZFar);
	};
	virtual ~OwnCamera(){};

	virtual void onInput(OwnVector3 *pDeltaPos = NULL, OwnVector3 *pDeltaRot = NULL, OwnVector3 *pDeltaOrbitRot = NULL){};

	void setPosition(const OwnVector3& vec);

public:
	//field-of-view
	float mFov;

	//aspect ratio
	float mAspect;

	//Z near
	float mZNear;

	//Z far
	float mZFar;

	OwnVector3 mCameraPos;

	//view transform matrix
	OwnMatrix4x4 mViewTrans;

	//projection transform matrix
	OwnMatrix4x4 mProjectionTrans;

	bool mYawFixed;

	OwnVector3 mYawFixedAxis;

	f32 mTargetDis;

};

class OwnCameraNormal : public OwnCamera
{
public:
	OwnCameraNormal(void);
	~OwnCameraNormal(void);

	void onInput(OwnVector3 *pDeltaPos = NULL, OwnVector3 *pDeltaRot = NULL, OwnVector3 *pDeltaOrbitRot = NULL);

public:

	//rotate x axis radians, rotate start form word coordinate, 
	//rotate before y rotate, behind z rotate.
	float mRotX;

	//rotate y axis radians, rotate start form word coordinate, 
	//rotate behind x rotate.
	float mRotY;


};

class OwnCameraQuaternion : public OwnCamera
{
public:
	OwnCameraQuaternion();
	~OwnCameraQuaternion();

	void lookAt(const OwnVector3& lookAt);

	void onInput(OwnVector3 *pDeltaPos = NULL, OwnVector3 *pDeltaRot = NULL, OwnVector3 *pDeltaOrbitRot = NULL);

	void setDirection(const OwnVector3& lookdir);

	//eg: eulerAngle(180, 180, 180)
	void setEulerAngle(const OwnVector3& eulerAngle);

	void setViewTrans();

public:
	OwnQuaternion mOrientation;

};

#endif
