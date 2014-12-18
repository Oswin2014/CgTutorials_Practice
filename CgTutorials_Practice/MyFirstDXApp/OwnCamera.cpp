#include "OwnCamera.h"


//-----------------------------------------------------------------------
void OwnCamera::setPosition( const OwnVector3& vec )
{
	mCameraPos = vec;
	onInput(&OwnVector3(0,0,0));
}
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
OwnCameraNormal::OwnCameraNormal(void)
	: mRotX(0)
	, mRotY(0)
{
	mViewTrans.setLookAtLH(mCameraPos, OwnVector3::UNIT_Y, OwnVector3::UNIT_Z);
}
//-----------------------------------------------------------------------
OwnCameraNormal::~OwnCameraNormal(void)
{
}
//-----------------------------------------------------------------------
void OwnCameraNormal::onInput( OwnVector3 *pDeltaPos, OwnVector3 *pDeltaRot, OwnVector3 *pDeltaOrbitRot )
{
	OwnVector3 upDir = OwnVector3::UNIT_Y;
	OwnVector3 lookDir = OwnVector3::UNIT_Z;

	if(NULL == pDeltaPos && NULL == pDeltaRot && NULL == pDeltaOrbitRot)
		return;

	if(pDeltaOrbitRot)
	{
		OwnVector3 lookDir2;
		lookDir.m_z += mTargetDis;

		OwnMatrix4x4 rotateMatrix;
		rotateMatrix.setPureRotateZXY(mRotX, mRotY, 0);
		lookDir2 = rotateMatrix.transformAffine(lookDir);
		OwnVector3 lookTarget = mCameraPos + lookDir2;

		mRotX += pDeltaOrbitRot->m_x;
		mRotY += pDeltaOrbitRot->m_y;
		f32 halfPai = Pai / 2.0f;
		if(mRotX > halfPai)
			mRotX = halfPai;
		else if(mRotX < -halfPai)
			mRotX = -halfPai;

		rotateMatrix.setPureRotateZXY(mRotX, mRotY, 0);
		lookDir = rotateMatrix.transformAffine(lookDir);

		upDir = rotateMatrix.transformAffine(upDir);

		mCameraPos = lookTarget - lookDir;
		lookDir.normalize();
	}
	else if(pDeltaPos || pDeltaRot)
	{
		if(pDeltaRot)
		{
			//Why negative the rotate radian value?
			//Because: use right hand coordinate rotate matrix. like lookdir(0,0,1)
			//when rotate X axis, in right hand coordinate, the positive radian will let lookdir y axis value down, 
			//but I want the positive radian let lookdir up, so I negative it.
			//when rotate Y axis, in right hand coordinate, the positive radian will let lookdir x value add, 
			//but the lookdir x axis value add, means that the lookdir rotate to right, so I negative it.
			mRotX -= pDeltaRot->m_x;
			mRotY -= pDeltaRot->m_y;
			f32 halfPai = Pai / 2.0f;
			if(mRotX > halfPai)
				mRotX = halfPai;
			else if(mRotX < -halfPai)
				mRotX = -halfPai;
		}
		OwnMatrix4x4 rotateMatrix;
		rotateMatrix.setPureRotateZXY(mRotX, mRotY, 0);

		lookDir = rotateMatrix.transformAffine(lookDir);

		//need do updir transform, because move y need right updir;
		//and rotate lookdir at y axis, because the error, the cross can't make sure which side less than 180
		//so will make a wrong x axis.
		upDir = rotateMatrix.transformAffine(upDir);

		if(pDeltaPos)
		{
			OwnVector3 rightDir;
			rightDir.setCross(upDir, lookDir);
			mCameraPos += rightDir * pDeltaPos->m_x + upDir * pDeltaPos->m_y + lookDir * pDeltaPos->m_z;
		}
	}

	OwnVector3 lookAt = mCameraPos + lookDir;
	mViewTrans.setLookAtLH(mCameraPos, upDir, lookAt);
}
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
OwnCameraQuaternion::OwnCameraQuaternion()
{
	setViewTrans();


	//setDirection(OwnVector3(0,1,1));
	//lookAt(OwnVector3(0,-1,1));

	//setEulerAngle(OwnVector3(45,-45,0));
}
//-----------------------------------------------------------------------
OwnCameraQuaternion::~OwnCameraQuaternion()
{

}
//-----------------------------------------------------------------------
void OwnCameraQuaternion::lookAt( const OwnVector3& lookAt )
{
	this->setDirection(lookAt - mCameraPos);
}
//-----------------------------------------------------------------------
void OwnCameraQuaternion::setDirection( const OwnVector3& lookdir )
{
	if(OwnVector3::ZERO == lookdir)
		return;

	OwnVector3 viewX, viewY, viewZ;
	viewZ = lookdir;
	viewZ.normalize();

	OwnQuaternion targetWorldOrientation;

	if(mYawFixed)
	{
		viewX.setCross(mYawFixedAxis, viewZ);
		viewX.normalize();
		viewY.setCross(viewZ, viewX);
		viewY.normalize();

		targetWorldOrientation.fromAxis(viewX, viewY, viewZ);
	}

	mOrientation = targetWorldOrientation;
	setViewTrans();
}
//-----------------------------------------------------------------------
void OwnCameraQuaternion::setEulerAngle( const OwnVector3& eulerAngle )
{
	OwnQuaternion qX, qY;

	qX.fromAngleAxis(-eulerAngle.m_x * Deg2Rad, OwnVector3::UNIT_X);
	qX.normalize();

	if(mYawFixed)
	{
		qY.fromAngleAxis(-eulerAngle.m_y * Deg2Rad, mYawFixedAxis);
		qY.normalize();
	}
	
	// save a row vector(vertex matrix type) transform matrix
	mOrientation = qY * qX;
	//= RotY * RotX = RotX.transpose * RotY.transpose.

	setViewTrans();
}
//-----------------------------------------------------------------------
void OwnCameraQuaternion::setViewTrans()
{
	OwnMatrix4x4 rotMtx, translateMtx;
	mOrientation.toRotateMatrix(rotMtx);

	translateMtx.makeTrans(-mCameraPos);
	mViewTrans.setMultiply(translateMtx, rotMtx);
}
//-----------------------------------------------------------------------
void OwnCameraQuaternion::onInput( OwnVector3 *pDeltaPos, OwnVector3 *pDeltaRot, OwnVector3 *pDeltaOrbitRot)
{
	if(NULL == pDeltaPos && NULL == pDeltaRot && NULL == pDeltaOrbitRot)
		return;

	OwnVector3 axisX, axisY, axisZ;
	mOrientation.toAxes(axisX, axisY, axisZ);

	if(pDeltaOrbitRot)
	{
		OwnVector3 lookDir = axisZ;
		lookDir.normalize();
		lookDir = lookDir * mTargetDis;
		OwnVector3 lookTarget = mCameraPos + lookDir;

		OwnQuaternion qX, qY;
		qX.fromAngleAxis(pDeltaOrbitRot->m_x, axisX);
		qX.normalize();

		if(mYawFixed)
		{
			qY.fromAngleAxis(pDeltaOrbitRot->m_y, mYawFixedAxis);
			qY.normalize();
		}

		mOrientation = qY * qX * mOrientation;
		mOrientation.toAxes(axisX, axisY, axisZ);

		lookDir = axisZ;
		lookDir.normalize();
		lookDir = lookDir * mTargetDis;
		mCameraPos = lookTarget - lookDir;
	}
	else if (pDeltaPos || pDeltaRot)
	{
		if(pDeltaRot)
		{
			OwnQuaternion qX, qY;
			qX.fromAngleAxis(-pDeltaRot->m_x, axisX);
			qX.normalize();

			if(mYawFixed)
			{
 				qY.fromAngleAxis(-pDeltaRot->m_y, mYawFixedAxis);
 				qY.normalize();
			}

			//right multiply let the Y axis rotate use the view coordinate Y axis not the world axis.
			//mOrientation = mOrientation * qX * qY;

			// save a row vector(vertex matrix type) transform matrix
			mOrientation = qY * qX * mOrientation;
			//= RotY * RotX = RotX.transpose * RotY.transpose.
		}

		if(pDeltaPos)
		{
			mOrientation.toAxes(axisX, axisY, axisZ);
			mCameraPos += axisX * pDeltaPos->m_x + axisY * pDeltaPos->m_y + axisZ * pDeltaPos->m_z;
		}
	}

	setViewTrans();
}
//-----------------------------------------------------------------------