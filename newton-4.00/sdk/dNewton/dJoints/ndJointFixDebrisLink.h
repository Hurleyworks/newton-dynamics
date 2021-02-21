/* Copyright (c) <2003-2019> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#ifndef __D_JOINT_DEBRIS_LINK_H_
#define __D_JOINT_DEBRIS_LINK_H_

#include "ndNewtonStdafx.h"
#include "ndJointBilateralConstraint.h"

class ndJointFixDebrisLink: public ndJointBilateralConstraint
{
	public:
	D_NEWTON_API ndJointFixDebrisLink(const dVector& childPivotInGlobalSpace, const dVector& parentPivotInGlobalSpace, ndBodyKinematic* const child, ndBodyKinematic* const parent);
	D_NEWTON_API virtual ~ndJointFixDebrisLink();

	protected:
	void JacobianDerivative(ndConstraintDescritor& desc);

	dFloat32 m_distance;
};
#endif 

