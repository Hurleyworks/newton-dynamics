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

#include "ndSandboxStdafx.h"
#include "ndPhysicsWorld.h"
#include "ndContactCallback.h"
#include "ndDemoEntityManager.h"
#include "ndDemoCameraManager.h"
#include "ndDemoMeshInterface.h"
#include "ndBasicPlayerCapsule.h"
#include "ndArchimedesBuoyancyVolume.h"

#define MAX_PHYSICS_STEPS			1
#define MAX_PHYSICS_FPS				60.0f
//#define MAX_PHYSICS_RECOVER_STEPS	2

ndPhysicsWorld::ndPhysicsWorld(ndDemoEntityManager* const manager)
	:ndWorld()
	,m_manager(manager)
	,m_timeAccumulator(0.0f)
	,m_deletedBodies()
	,m_hasPendingObjectToDelete(false)
	,m_deletedLock()
{
	ClearCache();
	SetContactNotify(new ndContactCallback);
}

ndPhysicsWorld::~ndPhysicsWorld()
{
}

ndDemoEntityManager* ndPhysicsWorld::GetManager() const
{
	return m_manager;
}

void ndPhysicsWorld::QueueBodyForDelete(ndBody* const body)
{
	dScopeSpinLock lock(m_deletedLock);
	m_hasPendingObjectToDelete.store(true);
	m_deletedBodies.PushBack(body);
}

void ndPhysicsWorld::DeletePendingObjects()
{
	if (m_hasPendingObjectToDelete.load())
	{
		Sync();
		m_hasPendingObjectToDelete.store(false);
		for (dInt32 i = 0; i < m_deletedBodies.GetCount(); i++)
		{
			DeleteBody(m_deletedBodies[i]);
		}
		m_deletedBodies.SetCount(0);
	}
}

void ndPhysicsWorld::AdvanceTime(dFloat32 timestep)
{
	const dFloat32 descreteStep = (1.0f / MAX_PHYSICS_FPS);

	dInt32 maxSteps = MAX_PHYSICS_STEPS;
	m_timeAccumulator += timestep;

	// if the time step is more than max timestep par frame, throw away the extra steps.
	if (m_timeAccumulator > descreteStep * maxSteps)
	{
		dFloat32 steps = dFloor(m_timeAccumulator / descreteStep) - maxSteps;
		dAssert(steps >= 0.0f);
		m_timeAccumulator -= descreteStep * steps;
	}
	
	while (m_timeAccumulator > descreteStep)
	{
		Update(descreteStep);
		m_timeAccumulator -= descreteStep;

		DeletePendingObjects();
	}
	if (m_manager->m_synchronousPhysicsUpdate)
	{
		Sync();
	}
}

void ndPhysicsWorld::OnPostUpdate(dFloat32 timestep)
{
	m_manager->m_cameraManager->FixUpdate(m_manager, timestep);
	if (m_manager->m_updateCamera)
	{
		m_manager->m_updateCamera(m_manager, m_manager->m_updateCameraContext, timestep);
	}
}

ndBody* ndPhysicsWorld::LoadUserDefinedBody(const nd::TiXmlNode* const parentNode, const char* const bodyClassName, dTree<const ndShape*, dUnsigned32>& shapesCache, const char* const assetPath) const
{
	if (!strcmp(bodyClassName, "ndArchimedesBuoyancyVolume"))
	{
		return new ndArchimedesBuoyancyVolume(parentNode, shapesCache);
	}
	else if (!strcmp(bodyClassName, "ndBasicPlayerCapsule"))
	{
		return new ndBasicPlayerCapsule(parentNode, shapesCache, (ndPhysicsWorld*)this);
	}

	return nullptr;
}