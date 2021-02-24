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

#include "dCoreStdafx.h"
#include "ndNewtonStdafx.h"
#include "ndWorld.h"
#include "ndModel.h"
#include "ndWorldScene.h"
#include "ndBodyDynamic.h"
#include "ndSkeletonList.h"
#include "ndDynamicsUpdate.h"
#include "ndBodyParticleSet.h"
#include "ndDynamicsUpdateAvx2.h"
#include "ndJointBilateralConstraint.h"

ndWorld::ndWorld()
	:dClassAlloc()
	,m_scene(nullptr)
	,m_sentinelBody(nullptr)
	,m_solver(nullptr)
	,m_jointList()
	,m_modelList()
	,m_skeletonList()
	,m_particleSetList()
	,m_timestep(dFloat32 (0.0f))
	,m_freezeAccel2(D_FREEZE_ACCEL2)
	,m_freezeAlpha2(D_FREEZE_ACCEL2)
	,m_freezeSpeed2(D_FREEZE_SPEED2)
	,m_freezeOmega2(D_FREEZE_SPEED2)
	,m_averageUpdateTime(dFloat32(0.0f))
	,m_averageTimestepAcc(dFloat32(0.0f))
	,m_averageFramesCount(dFloat32(0.0f))
	,m_lastExecutionTime(dFloat32(0.0f))
	,m_subSteps(1)
	,m_solverMode(ndStandardSolver)
	,m_solverIterations(4)
	,m_frameIndex(0)
	,m_collisionUpdate(true)
{
	// start the engine thread;
	//m_scene = new ndSceneMixed();
	m_scene = new ndWorldMixedScene(this);
	m_solver = new ndDynamicsUpdate(this);

	dInt32 steps = 1;
	dFloat32 freezeAccel2 = m_freezeAccel2;
	dFloat32 freezeAlpha2 = m_freezeAlpha2;
	dFloat32 freezeSpeed2 = m_freezeSpeed2;
	dFloat32 freezeOmega2 = m_freezeOmega2;
	for (dInt32 i = 0; i < D_SLEEP_ENTRIES; i++) 
	{
		m_sleepTable[i].m_maxAccel = freezeAccel2;
		m_sleepTable[i].m_maxAlpha = freezeAlpha2;
		m_sleepTable[i].m_maxVeloc = freezeSpeed2;
		m_sleepTable[i].m_maxOmega = freezeOmega2;
		m_sleepTable[i].m_steps = steps;
		steps += 7;
		freezeAccel2 *= dFloat32(1.5f);
		freezeAlpha2 *= dFloat32(1.5f);
		freezeSpeed2 *= dFloat32(1.5f);
		freezeOmega2 *= dFloat32(1.5f);
	}

	m_sleepTable[0].m_maxAccel *= dFloat32(0.009f);
	m_sleepTable[0].m_maxAlpha *= dFloat32(0.009f);

	steps += 300;
	m_sleepTable[D_SLEEP_ENTRIES - 1].m_maxAccel *= dFloat32(100.0f);
	m_sleepTable[D_SLEEP_ENTRIES - 1].m_maxAlpha *= dFloat32(100.0f);
	m_sleepTable[D_SLEEP_ENTRIES - 1].m_maxVeloc = 0.25f;
	m_sleepTable[D_SLEEP_ENTRIES - 1].m_maxOmega = 0.1f;
	m_sleepTable[D_SLEEP_ENTRIES - 1].m_steps = steps;

	m_sentinelBody = new ndBodyDynamic;
}

ndWorld::~ndWorld()
{
	Sync();

	while (m_skeletonList.GetFirst())
	{
		m_skeletonList.Remove(m_skeletonList.GetFirst());
	}

	while (m_jointList.GetFirst())
	{
		ndJointBilateralConstraint* const joint = m_jointList.GetFirst()->GetInfo();
		RemoveJoint(joint);
		delete joint;
	}

	while (m_particleSetList.GetFirst())
	{
		ndBodyParticleSet* const body = m_particleSetList.GetFirst()->GetInfo();
		RemoveBody(body);
		delete body;
	}

	while (m_modelList.GetFirst())
	{
		ndModel* const model = m_modelList.GetFirst()->GetInfo();
		RemoveModel(model);
		delete model;
	}

	const ndBodyList& bodyList = GetBodyList();
	while (bodyList.GetFirst())
	{
		ndBodyKinematic* const body = bodyList.GetFirst()->GetInfo();
		RemoveBody(body);
		delete body;
	}

	dAssert(!m_scene->GetContactList().GetCount());

	delete m_sentinelBody;
	delete m_scene;
	delete m_solver;
	ClearCache();
}

void ndWorld::SelectSolver(dInt32 solverMode)
{
	if (solverMode != m_solverMode)
	{
		delete m_solver;
		switch (solverMode)
		{
			case ndSimdAvx2Solver:
				m_solverMode = (ndSolverModes)solverMode;
				m_solver = new ndDynamicsUpdateAvx2(this);
				break;

			case ndStandardSolver:
			default:
				m_solverMode = (ndSolverModes)solverMode;
				m_solver = new ndDynamicsUpdate(this);
				break;
		}
	}
	
}


void ndWorld::ClearCache()
{
	ndContact::FlushFreeList();
	ndBodyList::FlushFreeList();
	ndModelList::FlushFreeList();
	ndJointList::FlushFreeList();
	ndContactList::FlushFreeList();
	ndSkeletonList::FlushFreeList();
	ndContactPointList::FlushFreeList();
	ndBodyParticleSetList::FlushFreeList();
	ndScene::ndFitnessList::FlushFreeList();
	dIsoSurface::dIsoVertexMap::FlushFreeList();
	ndBodyKinematic::ndContactMap::FlushFreeList();
	ndSkeletonContainer::ndNodeList::FlushFreeList();
}

void ndWorld::UpdateTransforms()
{
	for (ndBodyParticleSetList::dListNode* node = m_particleSetList.GetFirst(); node; node = node->GetNext())
	{
		ndBodyParticleSet* const particleSet = node->GetInfo();
		particleSet->GetNotifyCallback()->OnTranform(0, particleSet->GetMatrix());
	}

	m_scene->UpdateTransform();
}

void ndWorld::ApplyExternalForces()
{
	D_TRACKTIME();
	class ndApplyExternalForces: public ndScene::ndBaseJob
	{
		public:
		virtual void Execute()
		{
			D_TRACKTIME();
			const dArray<ndBodyKinematic*>& bodyArray = m_owner->GetActiveBodyArray();
			const dInt32 threadIndex = GetThreadId();
			const dInt32 threadCount = m_owner->GetThreadCount();
			const dInt32 bodyCount = bodyArray.GetCount() - 1;
			const dInt32 step = bodyCount / threadCount;
			const dInt32 start = threadIndex * step;
			const dInt32 count = ((threadIndex + 1) < threadCount) ? step : bodyCount - start;
			const dFloat32 timestep = m_timestep;

			for (dInt32 i = 0; i < count; i++)
			{
				ndBodyDynamic* const body = bodyArray[start + i]->GetAsBodyDynamic();
				if (body)
				{
					body->ApplyExternalForces(threadIndex, timestep);
				}
			}
		}
	};
	m_scene->SubmitJobs<ndApplyExternalForces>();
}

void ndWorld::PostUpdate(dFloat32 timestep)
{
	D_TRACKTIME();
	OnPostUpdate(m_timestep);
}

bool ndWorld::AddBody(ndBody* const body)
{
	ndBodyKinematic* const kinematicBody = body->GetAsBodyKinematic();
	dAssert(kinematicBody != m_sentinelBody);
	if (kinematicBody)
	{
		return m_scene->AddBody(kinematicBody);
	}
	else if (body->GetAsBodyParticleSet())
	{
		ndBodyParticleSet* const particleSet = body->GetAsBodyParticleSet();
		dAssert(particleSet->m_listNode == nullptr);
		ndBodyParticleSetList::dListNode* const node = m_particleSetList.Append(particleSet);
		particleSet->m_listNode = node;
	}
	return false;
}

void ndWorld::RemoveBody(ndBody* const body)
{
	ndBodyKinematic* const kinematicBody = body->GetAsBodyKinematic();
	dAssert(kinematicBody != m_sentinelBody);
	if (kinematicBody)
	{
		m_scene->RemoveBody(kinematicBody);
	}
	else if (body->GetAsBodyParticleSet())
	{
		ndBodyParticleSet* const particleSet = body->GetAsBodyParticleSet();
		dAssert(particleSet->m_listNode);
		m_particleSetList.Remove(particleSet->m_listNode);
	}
}

void ndWorld::DeleteBody(ndBody* const body)
{
	RemoveBody(body);
	ndBodyKinematic* const kinematicBody = body->GetAsBodyKinematic();
	if (kinematicBody)
	{
		while (kinematicBody->m_jointList.GetFirst())
		{
			ndJointBilateralConstraint* const joint = kinematicBody->m_jointList.GetFirst()->GetInfo();
			RemoveJoint(joint);
			delete joint;
		}
	}
	delete body;
}

void ndWorld::AddJoint(ndJointBilateralConstraint* const joint)
{
	dAssert(joint->m_worldNode == nullptr);
	if (joint->GetSolverModel() < m_secundaryCloseLoop)
	{
		m_skeletonList.m_skelListIsDirty = true;
	}
	joint->m_worldNode = m_jointList.Append(joint);
	joint->m_body0Node = joint->GetBody0()->AttachJoint(joint);
	joint->m_body1Node = joint->GetBody1()->AttachJoint(joint);
}

void ndWorld::RemoveJoint(ndJointBilateralConstraint* const joint)
{
	dAssert(joint->m_worldNode != nullptr);
	dAssert(joint->m_body0Node != nullptr);
	dAssert(joint->m_body1Node != nullptr);
	joint->GetBody0()->DetachJoint(joint->m_body0Node);
	joint->GetBody1()->DetachJoint(joint->m_body1Node);

	m_jointList.Remove(joint->m_worldNode);
	if (joint->GetSolverModel() < m_secundaryCloseLoop)
	{
		m_skeletonList.m_skelListIsDirty = true;
	}
	joint->m_worldNode = nullptr;
	joint->m_body0Node = nullptr;
	joint->m_body1Node = nullptr;
}

void ndWorld::AddModel(ndModel* const model)
{
	if (!model->m_node)
	{
		model->m_node = m_modelList.Append(model);
	}
}

void ndWorld::RemoveModel(ndModel* const model)
{
	if (model->m_node)
	{
		m_modelList.Remove(model->m_node);
		model->m_node = nullptr;
	}
}

dInt32 ndWorld::CompareJointByInvMass(const ndJointBilateralConstraint* const jointA, const ndJointBilateralConstraint* const jointB, void* notUsed)
{
	dInt32 modeA = jointA->GetSolverModel();
	dInt32 modeB = jointB->GetSolverModel();

	if (modeA < modeB) 
	{
		return -1;
	}
	else if (modeA > modeB) 
	{
		return 1;
	}
	else 
	{
		dFloat32 invMassA = dMin(jointA->GetBody0()->GetInvMass(), jointA->GetBody1()->GetInvMass());
		dFloat32 invMassB = dMin(jointB->GetBody0()->GetInvMass(), jointB->GetBody1()->GetInvMass());
		if (invMassA < invMassB) 
		{
			return -1;
		}
		else if (invMassA > invMassB) 
		{
			return 1;
		}
	}
	return 0;
}

dInt32 ndWorld::CompareIslandMember(const ndIslandMember* const A, const ndIslandMember* const B, void* const context)
{
	//dInt32 ida = A->m_root->m_index * 2 + ((A->m_body->GetInvMass() == dFloat32(0.0f)) ? 0 : 1);
	//dInt32 idb = B->m_root->m_index * 2 + ((B->m_body->GetInvMass() == dFloat32(0.0f)) ? 0 : 1);

	const dInt32 ida = A->m_root->m_index;
	const dInt32 idb = B->m_root->m_index;
	if (ida > idb)
	{
		return 1;
	}
	else if (ida < idb)
	{
		return -1;
	}

	const dFloat32 invMass0 = A->m_body->GetInvMass();
	const dFloat32 invMass1 = B->m_body->GetInvMass();
	if (invMass0 > invMass1)
	{
		return 1;
	}
	else if (invMass0 < invMass1)
	{
		return -1;
	}
	return 0;
}

void ndWorld::Save(const char* const path) const
{
	char* const oldloc = setlocale(LC_ALL, 0);
	setlocale(LC_ALL, "C");

	nd::TiXmlDocument asciifile;
	nd::TiXmlDeclaration* const decl = new nd::TiXmlDeclaration("1.0", "", "");
	asciifile.LinkEndChild(decl);

	nd::TiXmlElement* const worldNode = new nd::TiXmlElement("ndWorld");
	asciifile.LinkEndChild(worldNode);

	char assetPath[1024];
	strcpy(assetPath, path);

	char* name = strrchr(assetPath, '/');
	if (!name)
	{
		name = strrchr(assetPath, '\\');
	}
	if (!name)
	{
		name = assetPath;
	}

	char* ext = strrchr(name, '.');
	if (!ext)
	{
		ext = name;
	}
	ext[0] = 0;
	_mkdir(assetPath);

	Save(worldNode, assetPath);

	asciifile.SaveFile(path);
	setlocale(LC_ALL, oldloc);
}

void ndWorld::Save(nd::TiXmlElement* const worldNode, const char* const assetPath) const
{
	nd::TiXmlElement* const config = new nd::TiXmlElement("settings");
	worldNode->LinkEndChild(config);

	xmlSaveParam(config, "description", "string", "Newton Dynamics 4.00");
	xmlSaveParam(config, "revision", "string", "1.00");
	xmlSaveParam(config, "solverSubsteps", m_subSteps);
	xmlSaveParam(config, "solverIterations", m_solverIterations);

	dInt32 shapesCount = 0;
	dTree<dUnsigned32, const ndShape*> uniqueShapes;
	const ndBodyList& bodyList = GetBodyList();
	for (ndBodyList::dListNode* bodyNode = bodyList.GetFirst(); bodyNode; bodyNode = bodyNode->GetNext())
	{
		ndBodyKinematic* const body = bodyNode->GetInfo();
		ndShape* const shape = body->GetCollisionShape().GetShape();
		if (!uniqueShapes.Find(shape))
		{
			uniqueShapes.Insert(shapesCount, shape);
			shapesCount++;
		}
	}

	if (uniqueShapes.GetCount())
	{
		nd::TiXmlElement* const shapesNode = new nd::TiXmlElement("ndShapes");
		worldNode->LinkEndChild(shapesNode);

		dTree<dUnsigned32, const ndShape*>::Iterator it(uniqueShapes);
		for (it.Begin(); it; it ++)
		{
			dInt32 nodeId = *it;
			const ndShape* const shape = it.GetKey();
			shape->Save(shapesNode, assetPath, nodeId);
		}
	}

	if (bodyList.GetCount())
	{
		dInt32 bodyIndex = 0;
		nd::TiXmlElement* const bodiesNode = new nd::TiXmlElement("ndBodies");
		worldNode->LinkEndChild(bodiesNode);

		for (ndBodyList::dListNode* bodyNode = bodyList.GetFirst(); bodyNode; bodyNode = bodyNode->GetNext())
		{
			ndBodyKinematic* const body = bodyNode->GetInfo();
			body->Save(bodiesNode, assetPath, bodyIndex, uniqueShapes);
			bodyIndex++;
		}
	}
}

void ndWorld::Load(const char* const path)
{
	char* const oldloc = setlocale(LC_ALL, 0);
	setlocale(LC_ALL, "C");

	nd::TiXmlDocument doc(path);
	doc.LoadFile();
	dAssert(!doc.Error());

	const nd::TiXmlElement* const rootNode = doc.RootElement();
	if (doc.FirstChild("ndWorld"))
	{
		char assetPath[1024];
		strcpy(assetPath, path);

		char* name = strrchr(assetPath, '/');
		if (!name)
		{
			name = strrchr(assetPath, '\\');
		}
		if (!name)
		{
			name = assetPath;
		}

		char* ext = strrchr(name, '.');
		if (!ext)
		{
			ext = name;
		}
		ext[0] = 0;

		Load(rootNode, assetPath);
	}

	setlocale(LC_ALL, oldloc);
}

void ndWorld::LoadSettings(const nd::TiXmlNode* const rootNode)
{
	//const nd::TiXmlNode* const settings = rootNode->FirstChild("settings");
	//dAssert(settings);
}

void ndWorld::LoadShapes(const nd::TiXmlNode* const rootNode, dTree<const ndShape*, dUnsigned32>& shapesCache, const char* const assetPath)
{
	const nd::TiXmlNode* const shapes = rootNode->FirstChild("ndShapes");
	dAssert(shapes);

	for (const nd::TiXmlNode* node = shapes->FirstChild(); node; node = node->NextSibling())
	{
		ndShape* shape = nullptr;
		const char* const name = node->Value();
		if (!strcmp(name, "ndShapeBox"))
		{
			shape = new ndShapeBox(node);
		}
		else if (!strcmp(name, "ndShapeSphere"))
		{
			shape = new ndShapeSphere(node);
		}
		else if (!strcmp(name, "ndShapeCapsule"))
		{
			shape = new ndShapeCapsule(node);
		}
		else if (!strcmp(name, "ndShapeConvexHull"))
		{
			shape = new ndShapeConvexHull(node);
		}
		else if (!strcmp(name, "ndShapeStaticBVH"))
		{
			shape = new ndShapeStaticBVH(node, assetPath);
		}
		else
		{
			dAssert(0);
		}
		if (shape)
		{
			dInt32 shapeId;
			const nd::TiXmlElement* const element = (nd::TiXmlElement*) node;
			element->Attribute("nodeId", &shapeId);
			shapesCache.Insert(shape->AddRef(), shapeId);
		}
	}
}

void ndWorld::LoadBodies(const nd::TiXmlNode* const rootNode, dTree<const ndShape*, dUnsigned32>& shapesCache, const char* const assetPath)
{
	const nd::TiXmlNode* const shapes = rootNode->FirstChild("ndBodies");
	dAssert(shapes);

	for (const nd::TiXmlNode* parentNode = shapes->FirstChild(); parentNode; parentNode = parentNode->NextSibling())
	{
		ndBody* body = nullptr;
		const char* const bodyClassName = parentNode->Value();
		if (!strcmp(bodyClassName, "ndBodyDynamic"))
		{
			body = new ndBodyDynamic(parentNode, shapesCache);
		}
		else if (!strcmp(bodyClassName, "ndBodyTriggerVolume"))
		{
			body = new ndBodyTriggerVolume(parentNode, shapesCache);
		}
		else if (!strcmp(bodyClassName, "ndBodyPlayerCapsule"))
		{
			body = new ndBodyPlayerCapsule(parentNode, shapesCache);
		}
		else
		{
			body = LoadUserDefinedBody(parentNode, bodyClassName, shapesCache, assetPath);
		}
		if (body)
		{
			AddBody(body);
		}
	}
}

ndBody* ndWorld::LoadUserDefinedBody(const nd::TiXmlNode* const parentNode, const char* const bodyClassName, dTree<const ndShape*, dUnsigned32>& shapesCache, const char* const assetPath) const
{
	dAssert(0);
	return nullptr;
}

void ndWorld::Load(const nd::TiXmlElement* const rootNode, const char* const assetPath)
{
	dTree<const ndShape*, dUnsigned32> uniqueShapes;

	LoadSettings(rootNode);
	LoadShapes(rootNode, uniqueShapes, assetPath);
	LoadBodies(rootNode, uniqueShapes, assetPath);

	while (uniqueShapes.GetRoot())
	{
		const ndShape* const shape = uniqueShapes.GetRoot()->GetInfo();
		shape->Release();
		uniqueShapes.Remove(uniqueShapes.GetRoot());
	}
}

void ndWorld::ThreadFunction()
{
	dUnsigned64 timeAcc = dGetTimeInMicrosenconds();
	const bool collisionUpdate = m_collisionUpdate;
	m_collisionUpdate = true;
	if (collisionUpdate)
	{
		m_scene->CollisionOnlyUpdate();
	}
	else
	{
		D_TRACKTIME();
		m_scene->Begin();
		m_scene->BalanceScene();

		dInt32 const steps = m_subSteps;
		dFloat32 timestep = m_timestep / steps;
		for (dInt32 i = 0; i < steps; i++)
		{
			SubStepUpdate(timestep);
		}

		m_scene->SetTimestep(m_timestep);
		UpdateTransforms();
		//UpdateListenersPostTransform();
		PostUpdate(m_timestep);
		m_scene->End();
	}
	
	m_frameIndex++;
	m_lastExecutionTime = (dGetTimeInMicrosenconds() - timeAcc) * dFloat32(1.0e-6f);
	CalculateAverageUpdateTime();
}

void ndWorld::CalculateAverageUpdateTime()
{
	m_averageFramesCount += dFloat32 (1.0f);
	m_averageTimestepAcc += m_lastExecutionTime;

	dAssert(m_averageTimestepAcc >= dFloat32(0.0f));
	const dFloat32 movingAverageFrames = dFloat32 (16);
	if (m_averageFramesCount >= movingAverageFrames)
	{
		m_averageUpdateTime = m_averageTimestepAcc/m_averageFramesCount;
		m_averageTimestepAcc = dFloat32 (0.0f);
		m_averageFramesCount -= movingAverageFrames;
	}
}

void ndWorld::SubStepUpdate(dFloat32 timestep)
{
	D_TRACKTIME();

	// do the a pre-physics step
	m_scene->m_lru = m_scene->m_lru + 1;
	m_scene->SetTimestep(timestep);

	UpdateSkeletons();
	m_scene->BuildBodyArray();

	ndBodyKinematic* sentinelBody = m_sentinelBody;
	sentinelBody->PrepareStep(m_scene->GetActiveBodyArray().GetCount());
	sentinelBody->m_resting = 1;
	sentinelBody->m_autoSleep = 1;
	sentinelBody->m_equilibrium = 1;
	sentinelBody->m_solverSleep0 = 1;
	sentinelBody->m_solverSleep1 = 1;
	m_scene->GetActiveBodyArray().PushBack(sentinelBody);

	ApplyExternalForces();

	// update the collision system
	m_scene->UpdateAabb();
	m_scene->FindCollidingPairs();
	m_scene->BuildContactArray();
	m_scene->CalculateContacts();
	m_scene->DeleteDeadContact();

	// update all special models.
	//UpdatePrelisteners();

	// Update Particle base physics
	ParticleUpdate();

	// Update all models
	ModelUpdate();

	// calculate internal forces, integrate bodies and update matrices.
	dAssert(m_solver);
	m_solver->Update();

	//UpdatePostlisteners();
}

void ndWorld::ParticleUpdate()
{
	D_TRACKTIME();
	for (ndBodyParticleSetList::dListNode* node = m_particleSetList.GetFirst(); node; node = node->GetNext())
	{
		ndBodyParticleSet* const body = node->GetInfo();
		body->Update(this, m_timestep);
	}
}

void ndWorld::ModelUpdate()
{
	D_TRACKTIME();
	for (ndModelList::dListNode* node = m_modelList.GetFirst(); node; node = node->GetNext())
	{
		ndModel* const model = node->GetInfo();
		model->Update(this, m_timestep);
	}
}

void ndWorld::UpdateSkeletons()
{
	D_TRACKTIME();

	if (m_skeletonList.m_skelListIsDirty)
	{
		m_skeletonList.m_skelListIsDirty = false;
		while (m_skeletonList.GetFirst())
		{
			m_skeletonList.Remove(m_skeletonList.GetFirst());
		}

		m_scene->BuildBodyArray();
		ndDynamicsUpdate& solverUpdate = *m_solver;
		for (ndJointList::dListNode* node = m_jointList.GetFirst(); node; node = node->GetNext())
		{
			ndJointBilateralConstraint* const constraint = node->GetInfo();
			dAssert(constraint && constraint->GetAsBilateral());
			bool test = constraint->GetSolverModel() < m_closeLoop;
			test = test && (constraint->m_preconditioner0 == dFloat32(1.0f));
			test = test && (constraint->m_preconditioner1 == dFloat32(1.0f));
			test = test && (constraint->GetRowsCount() > 0);

			constraint->m_mark = 1;
			if (test)
			{
				ndBodyKinematic* const body0 = constraint->GetBody0();
				ndBodyKinematic* const body1 = constraint->GetBody1();
				if (body1->GetInvMass() > dFloat32(0.0f))
				{
					ndBodyKinematic* root0 = solverUpdate.FindRootAndSplit(body0);
					ndBodyKinematic* root1 = solverUpdate.FindRootAndSplit(body1);
					if (root0 != root1)
					{
						if (root0->m_rank > root1->m_rank)
						{
							dSwap(root0, root1);
						}
						root0->m_islandParent = root1;
						if (root0->m_rank == root1->m_rank)
						{
							root1->m_rank += 1;
							dAssert(root1->m_rank <= 6);
						}
					}
				}
			}
		}

		const dArray<ndBodyKinematic*>& bodyArray = m_scene->GetActiveBodyArray();
		solverUpdate.m_leftHandSide.SetCount(dMax (bodyArray.GetCount() * 2,  D_DEFAULT_BUFFER_SIZE));
		ndIslandMember* const islands = (ndIslandMember*)&solverUpdate.m_leftHandSide[0];

		dInt32 entriesCount = 0;
		for (dInt32 i = bodyArray.GetCount() - 1; i >= 0; i--)
		{
			ndBodyKinematic* const body = bodyArray[i];
			body->m_index = -1;
			body->m_skeletonMark = 1;
			if (body->GetInvMass() > dFloat32(0.0f))
			{
				ndIslandMember& entry = islands[entriesCount];
				entry.m_body = body;
				entry.m_root = solverUpdate.FindRootAndSplit(body);
				entriesCount++;
			}
		}

		dInt32 inslandCount = 0;
		for (ndJointList::dListNode* node = m_jointList.GetFirst(); node; node = node->GetNext())
		{
			ndJointBilateralConstraint* const constraint = node->GetInfo();
			ndBodyKinematic* const root = solverUpdate.FindRootAndSplit(constraint->GetBody0());
			if (root->m_index < 0)
			{
				root->m_index = inslandCount;
				inslandCount++;
			}

			if (constraint->GetSolverModel() < m_closeLoop)
			{
				ndBodyKinematic* const body = constraint->GetBody1();
				if (body->GetInvMass() == dFloat32(0.0f))
				{
					if (root->m_rank >= 0)
					{
						root->m_rank = -1;
						ndIslandMember& entry = islands[entriesCount];
						entry.m_body = body;
						entry.m_root = root;
						entriesCount++;
					}
				}
			}
		}

		dSort(islands, entriesCount, CompareIslandMember);

		dInt32* const scans = dAlloca(dInt32, inslandCount + 256);
		memset(scans, 0, sizeof(dInt32) * (inslandCount + 256));
		for (dInt32 i = 0; i < entriesCount; i++)
		{
			dInt32 index = islands[i].m_root->m_index;
			scans[index] ++;
		}

		dInt32 acc = 0;
		for (dInt32 i = 0; i < inslandCount + 1; i++)
		{
			dInt32 count = scans[i];
			scans[i] = acc;
			acc += count;
		}

		class ndQueue : public dFixSizeBuffer<ndSkeletonContainer::ndNode*, 1024 * 4>
		{
			public:
			ndQueue()
				:dFixSizeBuffer<ndSkeletonContainer::ndNode*, 1024 * 4>()
				,m_mod(sizeof(m_array) / sizeof(m_array[0]))
			{
				m_lastIndex = 0;
				m_firstIndex = 0;
			}

			void Push(ndSkeletonContainer::ndNode* const node)
			{
				m_firstIndex++;
				const dInt32 count = m_firstIndex - m_lastIndex;
				dInt32 slot = count - 1;
				for (; (slot > 0) && (m_array[m_lastIndex + slot - 1]->m_joint->GetSolverModel()); slot--)
				{
					m_array[m_lastIndex + slot] = m_array[m_lastIndex + slot - 1];
				}
				m_array[m_lastIndex + slot] = node;

				if (m_firstIndex >= m_mod)
				{
					m_firstIndex = 0;
				}
				dAssert(m_firstIndex != m_lastIndex);
			}

			void Reset()
			{
				m_lastIndex = m_firstIndex;
			}

			bool IsEmpty() const
			{
				return (m_firstIndex == m_lastIndex);
			}

			dInt32 m_lastIndex;
			dInt32 m_firstIndex;
			dInt32 m_mod;
		};

		for (dInt32 i = 0; i < inslandCount; i++)
		{
			ndQueue queuePool;
			ndJointBilateralConstraint* loopJoints[128];

			dInt32 loopCount = 0;
			dInt32 start = scans[i];
			ndBodyKinematic* const rootBody = islands[start].m_body;
			dInt32 key = solverUpdate.FindRootAndSplit(islands[start].m_root)->m_index;
			ndSkeletonContainer* const skeleton = m_skeletonList.CreateContatiner(rootBody);
			ndSkeletonContainer::ndNode* const rootNode = skeleton->GetRoot();
			for (ndJointList::dListNode* jointNode = rootBody->m_jointList.GetFirst(); jointNode; jointNode = jointNode->GetNext())
			{
				ndJointBilateralConstraint* const constraint = jointNode->GetInfo();
				if (constraint->GetSolverModel() < m_closeLoop)
				{
					ndBodyKinematic* const childBody = constraint->GetBody0();
					dInt32 childKey = solverUpdate.FindRootAndSplit(childBody)->m_index;
					if (childKey == key)
					{
						dAssert(constraint->m_mark);
						dAssert(childBody->GetInvMass() != dFloat32(0.0f));
						constraint->m_mark = 0;
						childBody->m_skeletonMark = 0;
						ndSkeletonContainer::ndNode* const node = skeleton->AddChild((ndJointBilateralConstraint*)constraint, rootNode);
						queuePool.Push(node);
					}
				}
			}

			while (!queuePool.IsEmpty())
			{
				dInt32 count = queuePool.m_firstIndex - queuePool.m_lastIndex;
				if (count < 0)
				{
					count += queuePool.m_mod;
				}

				dInt32 index = queuePool.m_lastIndex;
				queuePool.Reset();

				for (dInt32 j = 0; j < count; j++)
				{
					ndSkeletonContainer::ndNode* const parentNode = queuePool[index];
					ndBodyKinematic* const parentBody = parentNode->m_body;

					for (ndJointList::dListNode* jointNode1 = parentBody->m_jointList.GetFirst(); jointNode1; jointNode1 = jointNode1->GetNext())
					{
						ndJointBilateralConstraint* const constraint1 = jointNode1->GetInfo();
						if (constraint1->m_mark)
						{
							constraint1->m_mark = 0;

							ndBodyKinematic* const childBody = (constraint1->GetBody0() == parentBody) ? constraint1->GetBody1() : constraint1->GetBody0();
							if (childBody->m_skeletonMark && (childBody->GetInvMass() != dFloat32(0.0f)))
							{
								childBody->m_skeletonMark = 0;
								ndSkeletonContainer::ndNode* const childNode = skeleton->AddChild(constraint1, parentNode);
								queuePool.Push(childNode);
							}
							else if (loopCount < (sizeof(loopJoints) / sizeof(loopJoints[0])))
							{
								loopJoints[loopCount] = (ndJointBilateralConstraint*)constraint1;
								loopCount++;
							}
						}
					}

					index++;
					if (index >= queuePool.m_mod)
					{
						index = 0;
					}
				}
			}
			skeleton->Finalize(loopCount, loopJoints);
		}
	}

	ndSkeletonList::Iterator iter(m_skeletonList);
	for (iter.Begin(); iter; iter++)
	{
		ndSkeletonContainer* const skeleton = &iter.GetNode()->GetInfo();
		skeleton->ClearSelfCollision();
	}
}
