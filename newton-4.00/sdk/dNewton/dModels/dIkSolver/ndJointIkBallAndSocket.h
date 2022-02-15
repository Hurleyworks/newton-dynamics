/* Copyright (c) <2003-2021> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#ifndef __ND_JOINT_IK_BALL_AND_SOCKET_H__
#define __ND_JOINT_IK_BALL_AND_SOCKET_H__

#include "ndNewtonStdafx.h"
#include "ndJointBallAndSocket.h"

class ndJointIkBallAndSocket: public ndJointBallAndSocket
{
	public:
	D_CLASS_REFLECTION(ndJointIkBallAndSocket);
	D_NEWTON_API ndJointIkBallAndSocket(const ndLoadSaveBase::ndLoadDescriptor& desc);
	D_NEWTON_API ndJointIkBallAndSocket(const ndMatrix& pinAndPivotFrame, ndBodyKinematic* const child, ndBodyKinematic* const parent);
	D_NEWTON_API virtual ~ndJointIkBallAndSocket();

	//D_NEWTON_API virtual ndFloat32 GetMaxConeAngle() const;
	//D_NEWTON_API virtual void SetConeLimit(ndFloat32 maxConeAngle);
	//D_NEWTON_API virtual void SetConeFriction(ndFloat32 regularizer, ndFloat32 viscousFriction);
	//
	//D_NEWTON_API virtual void SetTwistLimits(ndFloat32 minAngle, ndFloat32 maxAngle);
	//D_NEWTON_API virtual void GetTwistLimits(ndFloat32& minAngle, ndFloat32& maxAngle) const;
	//D_NEWTON_API virtual void SetTwistFriction(ndFloat32 regularizer, ndFloat32 viscousFriction);

	protected:
	D_NEWTON_API void JacobianDerivative(ndConstraintDescritor& desc);
	D_NEWTON_API void Save(const ndLoadSaveBase::ndSaveDescriptor& desc) const;
	D_NEWTON_API void DebugJoint(ndConstraintDebugCallback& debugCallback) const;

	//void SubmitTwistAngle(const ndVector& pin, ndFloat32 angle, ndConstraintDescritor& desc);
	//void SubmitAngularAxis(const ndMatrix& matrix0, const ndMatrix& matrix1, ndConstraintDescritor& desc);
	//void SubmitAngularAxisCartesianApproximation(const ndMatrix& matrix0, const ndMatrix& matrix1, ndConstraintDescritor& desc);
	//void SubmitConeAngleOnlyRows(const ndMatrix& matrix0, const ndMatrix& matrix1, ndConstraintDescritor& desc);
	//
	//ndFloat32 m_maxConeAngle;
	//ndFloat32 m_coneFriction;
	//ndFloat32 m_minTwistAngle;
	//ndFloat32 m_maxTwistAngle;
	//ndFloat32 m_twistFriction;
	//ndFloat32 m_coneFrictionRegularizer;
	//ndFloat32 m_twistFrictionRegularizer;
};

#endif 
