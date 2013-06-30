/* Copyright (c) <2003-2013> <Julio Jerez, Newton Game Dynamics>
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

#ifndef _D_NEWTON_COLLISION_H_
#define _D_NEWTON_COLLISION_H_

#include "dStdAfxNewton.h"

class dNewton;

class dNewtonCollision
{
	public:
	enum dCollsionType
	{
		m_box,
		m_mesh,
		m_scene,
	};

	CNEWTON_API void *operator new (size_t size);
	CNEWTON_API void operator delete (void* ptr);

	CNEWTON_API dNewtonCollision (dCollsionType type);
	CNEWTON_API virtual ~dNewtonCollision();

	CNEWTON_API NewtonCollision* GetShape() const;

	protected:
	CNEWTON_API dNewtonCollision (NewtonCollision* const shape, dCollsionType type);

	CNEWTON_API void SetShape (NewtonCollision* const shape) ;
	virtual dNewtonCollision* Clone(NewtonCollision* const shape) const = 0; 

	NewtonCollision* m_shape;
	dCollsionType m_type;

	friend dNewton;
};


class dNewtonCollisionBox: public dNewtonCollision
{
	public: 
	dNewtonCollisionBox (dNewton* const world, dFloat x, dFloat y, dFloat z, int id);
	dNewtonCollision* Clone(NewtonCollision* const shape) const 
	{
		return new dNewtonCollisionBox (shape, m_type);
	}

	protected:
	dNewtonCollisionBox (NewtonCollision* const shape, dCollsionType type)
		:dNewtonCollision (shape, type)
	{
	}
};


class dNewtonCollisionMesh: public dNewtonCollision
{
	public: 
	CNEWTON_API dNewtonCollisionMesh (dNewton* const world);
	dNewtonCollision* Clone(NewtonCollision* const shape) const 
	{
		return new dNewtonCollisionMesh (shape, m_type);
	}

	CNEWTON_API virtual void BeginFace();
	CNEWTON_API virtual void AddFace(int vertexCount, const dFloat* const vertexPtr, int strideInBytes, int faceAttribute);
	CNEWTON_API virtual void EndFace();

	protected:
	dNewtonCollisionMesh (NewtonCollision* const shape, dCollsionType type)
		:dNewtonCollision (shape, type)
	{
	}
};


class dNewtonCollisionScene: public dNewtonCollision
{
	public: 
	CNEWTON_API dNewtonCollisionScene (dNewton* const world);
	dNewtonCollision* Clone(NewtonCollision* const shape) const 
	{
		dAssert (m_type == m_scene);
		return new dNewtonCollisionScene (shape, m_type);
	}


	CNEWTON_API virtual void BeginAddRemoveCollision();
	CNEWTON_API virtual void* AddCollision(const dNewtonCollision* const collision);
	CNEWTON_API virtual void RemoveCollision (void* const handle);
	CNEWTON_API virtual void EndAddRemoveCollision();

	protected:
	dNewtonCollisionScene (NewtonCollision* const shape, dCollsionType type)
		:dNewtonCollision (shape, type)
	{
	}
};



#endif
