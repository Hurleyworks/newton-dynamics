/* Copyright (c) <2003-2019> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _D_DYNAMIC_BODY_H_
#define _D_DYNAMIC_BODY_H_

#include "ndNewtonStdafx.h"

//#define DG_MAX_SPEED_ATT	dgFloat32(0.02f)
////#define DG_FREEZE_ACCEL	dgFloat32(0.1f)
//#define DG_FREEZE_ACCEL		dgFloat32(1.0f)
//#define DG_FREEZE_SPEED		dgFloat32(0.032f)
//
//#define DG_FREEZE_ACCEL2	(DG_FREEZE_ACCEL * DG_FREEZE_ACCEL)
//#define DG_FREEZE_SPEED2	(DG_FREEZE_SPEED * DG_FREEZE_SPEED)
//
//#define DG_FREEZE_MAG		DG_FREEZE_ACCEL
//#define DG_FREEZE_MAG2		(DG_FREEZE_MAG * DG_FREEZE_MAG)
//
//#define DG_ERR_TOLERANCE	dgFloat32(1.0e-2f)
//#define DG_ERR_TOLERANCE2	(DG_ERR_TOLERANCE * DG_ERR_TOLERANCE)
//
//class dgSkeletonContainer;

D_MSV_NEWTON_ALIGN_32
class ndBodyDynamic: public ndBodyKinematic
{
	public:
	D_NEWTON_API ndBodyDynamic();
	D_NEWTON_API virtual ~ndBodyDynamic ();
	D_NEWTON_API virtual ndBodyDynamic* GetAsBodyDynamic() { return this; }
	D_NEWTON_API virtual void ApplyExternalForces(dInt32 threadIndex, dFloat32 timestep);
	D_NEWTON_API void AddDampingAcceleration(dFloat32 timestep);

	dVector GetForce() const;
	void SetForce(const dVector& force);
	
	dVector GetToque() const;
	void SetTorque(const dVector& torque);

	dVector GetAccel() const;
	void SetAccel(const dVector& accel);

	dVector GetAlpha() const;
	void SetAlpha(const dVector& alpha);

	protected:
	dVector m_accel;
	dVector m_alpha;
	dVector m_externalForce;
	dVector m_externalTorque;
	dArray<ndBilateralJoint*> m_jointArray;
	friend class ndDynamicsUpdate;
} D_GCC_NEWTON_ALIGN_32 ;

inline dVector ndBodyDynamic::GetForce() const
{
	return m_externalForce;
}

inline void ndBodyDynamic::SetForce(const dVector& force)
{
	m_externalForce = force;
}

inline dVector ndBodyDynamic::GetToque() const
{
	return m_externalTorque;
}

inline void ndBodyDynamic::SetTorque(const dVector& torque)
{
	m_externalTorque = torque;
}

inline dVector ndBodyDynamic::GetAccel() const
{
	return m_accel;
}

inline void ndBodyDynamic::SetAccel(const dVector& accel)
{
	m_accel = accel;
}

inline dVector ndBodyDynamic::GetAlpha() const
{
	return m_alpha;
}

inline void ndBodyDynamic::SetAlpha(const dVector& alpha)
{
	m_alpha = alpha;
}


#endif 


