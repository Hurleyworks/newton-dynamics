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

#include "dCoreStdafx.h"
#include "ndNewtonStdafx.h"
#include "ndJointFixDebrisLink.h"

ndJointFixDebrisLink::ndJointFixDebrisLink(ndBodyKinematic* const body0, ndBodyKinematic* const body1)
	:ndJointBilateralConstraint(6, body0, body1, dGetIdentityMatrix())
{
	dAssert(body0->GetInvMass() > dFloat32(0.0f));
	dMatrix dummy;

	dVector posit0(body0->GetMatrix().TransformVector(body0->GetCentreOfMass()));
	dVector posit1(body1->GetMatrix().TransformVector(body1->GetCentreOfMass()));

	dVector dist(posit1 - posit0);
	m_distance = dSqrt(dist.DotProduct(dist).GetScalar());
	dAssert(m_distance > dFloat32(1.0e-3f));

	dMatrix matrix0(dist);
	matrix0.m_posit = posit0;
	CalculateLocalMatrix(matrix0, m_localMatrix0, dummy);

	dMatrix matrix1(matrix0);
	matrix1.m_posit = body1->GetMatrix().TransformVector(body1->GetCentreOfMass());
	CalculateLocalMatrix(matrix1, dummy, m_localMatrix1);
}

ndJointFixDebrisLink::~ndJointFixDebrisLink()
{
}

void ndJointFixDebrisLink::JacobianDerivative(ndConstraintDescritor& desc)
{
	dMatrix matrix0;
	dMatrix matrix1;

	// calculate the position of the pivot point and the Jacobian direction vectors, in global space. 
	CalculateGlobalMatrix(matrix0, matrix1);

	//dVector p0(matrix0.m_posit);
	//dVector p1(matrix1.m_posit);
	//
	//dVector dir(p1 - p0);
	//dFloat32 mag2 = dir.DotProduct(dir).GetScalar();
	//if (mag2 > dFloat32(1.0e-3f))
	//{
	//	dir = dir.Scale(1.0f / dSqrt(mag2));
	//	dFloat32 x = dSqrt (mag2) - m_distance;
	//
	//	dMatrix matrix(dir);
	//	dVector com0(m_body0->GetCentreOfMass());
	//	dMatrix body0Matrix(m_body0->GetMatrix());
	//	dVector veloc0(m_body0->GetVelocityAtPoint(p0));
	//
	//	dVector com1(m_body1->GetCentreOfMass());
	//	dMatrix body1Matrix(m_body1->GetMatrix());
	//	dVector veloc1(m_body1->GetVelocityAtPoint(p1));
	//
	//	dFloat32 v((veloc0 - veloc1).DotProduct(dir).GetScalar());
	//	dFloat32 a = (x - v * desc.m_timestep) * desc.m_invTimestep * desc.m_invTimestep;
	//
	//	dVector r0 ((p0 - body0Matrix.TransformVector(com0)).CrossProduct(matrix.m_front));
	//	dVector r1 ((p1 - body1Matrix.TransformVector(com1)).CrossProduct(matrix.m_front));
	//
	//	AddLinearRowJacobian(desc, p0, p0, matrix0.m_right);
	//	ndJacobian& jacobian0 = desc.m_jacobian[desc.m_rowsCount - 1].m_jacobianM0;
	//	ndJacobian& jacobian1 = desc.m_jacobian[desc.m_rowsCount - 1].m_jacobianM1;
	//
	//	jacobian0.m_linear = matrix[0];
	//	jacobian0.m_angular = r0;
	//
	//	jacobian1.m_linear = matrix[0].Scale(dFloat32 (-1.0f));
	//	jacobian1.m_angular = r1.Scale(dFloat32(-1.0f));
	//
	//	SetMotorAcceleration(desc, a);
	//}

	const dVector veloc0(m_body0->GetVelocity());
	const dVector veloc1(m_body1->GetVelocity());

	const dVector step (matrix1.m_posit - matrix0.m_posit);

	dVector dist(dVector::m_zero);
	dist.m_x = m_distance;
	for (dInt32 i = 0; i < 3; i++)
	{
		const dVector& dir = matrix0[i];
		
		AddLinearRowJacobian(desc, matrix0.m_posit, matrix0.m_posit, dir);
		ndJacobian& jacobian0 = desc.m_jacobian[desc.m_rowsCount - 1].m_jacobianM0;
		ndJacobian& jacobian1 = desc.m_jacobian[desc.m_rowsCount - 1].m_jacobianM1;

		dFloat32 x = step[i] - dist[i];
		dFloat32 v((veloc0 - veloc1).DotProduct(dir).GetScalar());
		dFloat32 a = (x - v * desc.m_timestep) * desc.m_invTimestep * desc.m_invTimestep;

		jacobian0.m_linear = dir;
		jacobian0.m_angular = dVector::m_zero;

		jacobian1.m_linear = dir.Scale(dFloat32(-1.0f));
		jacobian1.m_angular = dVector::m_zero;

		SetMotorAcceleration(desc, a);
	}
}


