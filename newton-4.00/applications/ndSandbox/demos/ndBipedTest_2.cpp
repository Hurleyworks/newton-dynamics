/* Copyright (c) <2003-2022> <Newton Game Dynamics>
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
#include "ndSkyBox.h"
#include "ndTargaToOpenGl.h"
#include "ndDemoMesh.h"
#include "ndDemoCamera.h"
#include "ndLoadFbxMesh.h"
#include "ndPhysicsUtils.h"
#include "ndPhysicsWorld.h"
#include "ndMakeStaticMap.h"
#include "ndDemoEntityManager.h"
#include "ndDemoInstanceEntity.h"

class ndAiBipedTest_2_Definition
{
	public:
	enum ndjointType
	{
		m_root,
		m_hinge,
		m_spherical,
		m_doubleHinge,
		m_effector
	};

	struct ndJointLimit
	{
		ndFloat32 m_minTwistAngle;
		ndFloat32 m_maxTwistAngle;
		ndFloat32 m_coneAngle;
	};

	struct ndFrameMatrix
	{
		ndFloat32 m_pitch;
		ndFloat32 m_yaw;
		ndFloat32 m_roll;
	};

	char m_boneName[32];
	ndjointType m_type;
	ndJointLimit m_jointLimits;
	ndFrameMatrix m_frameBasics;
};

static ndAiBipedTest_2_Definition mannequinDefinition[] =
{
	{ "root", ndAiBipedTest_2_Definition::m_root, {}, {} },
	//{ "Bip001 Spine1", 1.0f,{ -30.0f, 30.0f, 60.0f },{ 0.0f, 90.0f, 0.0f } },
	//{ "Bip001 Head", 1.0f,{ -60.0f, 60.0f, 60.0f },{ 0.0f, 90.0f, 0.0f } },

	{ "rhipjoint", ndAiBipedTest_2_Definition::m_spherical, { -45.0f, 45.0f, 80.0f }, { 0.0f, 0.0f, 0.0f } },
	{ "rfemur", ndAiBipedTest_2_Definition::m_hinge, { 0.0f, 150.0f, 0.0f }, { 0.0f, 90.0f, 0.0f } },
	{ "rtibia", ndAiBipedTest_2_Definition::m_doubleHinge, { 0.0f, 0.0f, 60.0f }, { 90.0f, 0.0f, 90.0f } },
	{ "rightCalfEffector", ndAiBipedTest_2_Definition::m_effector, { 0.0f, 0.0f, 60.0f }, { 0.0f, 0.0f, 0.0f } },

	{ "lhipjoint", ndAiBipedTest_2_Definition::m_spherical, { -45.0f, 45.0f, 80.0f }, { 0.0f, 0.0f, 0.0f } },
	{ "lfemur", ndAiBipedTest_2_Definition::m_hinge, { 0.0f, 150.0f, 0.0f }, { 0.0f, 90.0f, 0.0f } },
	{ "ltibia", ndAiBipedTest_2_Definition::m_doubleHinge, { 0.0f, 0.0f, 60.0f }, { 90.0f, 0.0f, 90.0f } },
	{ "leftCalfEffector", ndAiBipedTest_2_Definition::m_effector,{ 0.0f, 0.0f, 60.0f },{ 0.0f, 90.0f, 0.0f } },

	{ "", ndAiBipedTest_2_Definition::m_root,{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f } },
};

class ndAiBipedTest_2 : public ndModel
{
	public:

	class ndParamMapper
	{
		public:
		ndParamMapper()
			:m_x0(0.0f)
			,m_scale(0.0f)
		{
		}

		ndParamMapper(ndFloat32 x0, ndFloat32 x1)
			:m_x0(x0 + (x1 - x0) * 0.5f)
			,m_scale((x1 - x0) * 0.5f)
		{
		}

		ndFloat32 Interpolate(const ndFloat32 t)
		{
			return m_x0 + m_scale * t;
		}

		ndFloat32 m_x0;
		ndFloat32 m_scale;
	};

	class ndEffectorInfo
	{
		public:
		ndEffectorInfo()
			:m_basePosition(ndVector::m_wOne)
			,m_effector(nullptr)
			,m_swivel(0.0f)
			,m_x(0.0f)
			,m_y(0.0f)
			,m_z(0.0f)
		{
		}

		ndEffectorInfo(ndIkSwivelPositionEffector* const effector)
			:m_basePosition(effector->GetPosition())
			,m_effector(effector)
			,m_swivel(0.0f)
			,m_x(0.0f)
			,m_y(0.0f)
			,m_z(0.0f)
		{
		}

		ndVector m_basePosition;
		ndIkSwivelPositionEffector* m_effector;
		ndReal m_swivel;
		ndReal m_x;
		ndReal m_y;
		ndReal m_z;
		ndParamMapper m_x_mapper;
		ndParamMapper m_y_mapper;
		ndParamMapper m_z_mapper;
		ndParamMapper m_swivel_mapper;
	};

	ndAiBipedTest_2(ndDemoEntityManager* const scene, ndDemoEntity* const model, const ndMatrix& location, ndAiBipedTest_2_Definition* const definition)
		:ndModel()
	{
		// make a clone of the mesh and add it to the scene
		ndDemoEntity* const rootEntity = (ndDemoEntity*)model->CreateClone();
		scene->AddEntity(rootEntity);
		ndWorld* const world = scene->GetWorld();

		ndMatrix matrix(rootEntity->CalculateGlobalMatrix() * location);
		rootEntity->ResetMatrix(ndGetIdentityMatrix());

		// find the floor location 
		ndVector floor(FindFloor(*world, matrix.m_posit + ndVector(0.0f, 100.0f, 0.0f, 0.0f), 200.0f));
		matrix.m_posit.m_y = floor.m_y + 1.15f;

		rootEntity->ResetMatrix(matrix);

		ndFixSizeArray<ndBodyDynamic*, 64> bodies;
		// add the root body
		ndBodyDynamic* const rootBody = CreateBodyPart(scene, rootEntity, nullptr);
		m_rootBody = rootBody;
		bodies.PushBack(m_rootBody);

		ndInt32 stack = 0;
		ndFixSizeArray<ndBodyDynamic*, 32> parentBones;
		ndFixSizeArray<ndDemoEntity*, 32> childEntities;
		parentBones.SetCount(32);
		childEntities.SetCount(32);
		for (ndDemoEntity* child = rootEntity->GetChild(); child; child = child->GetSibling()) 
		{
			childEntities[stack] = child;
			parentBones[stack] = rootBody;
			stack++;
		}
		
		// walk model hierarchic adding all children designed as rigid body bones. 
		while (stack) 
		{
			stack--;
			ndBodyDynamic* parentBody = parentBones[stack];
			ndDemoEntity* const childEntity = childEntities[stack];
			const char* const name = childEntity->GetName().GetStr();
			ndTrace(("name: %s\n", name));
			for (ndInt32 i = 0; definition[i].m_boneName[0]; ++i)
			{
				if (!strcmp(definition[i].m_boneName, name))
				{
					if (definition[i].m_type != ndAiBipedTest_2_Definition::m_effector)
					{
						ndBodyDynamic* const childBody = CreateBodyPart(scene, childEntity, parentBody);
						bodies.PushBack(childBody);

						// connect this body part to its parentBody with a robot joint
						ndJointBilateralConstraint* const joint = ConnectBodyParts(childBody, parentBody, definition[i]);
						world->AddJoint(joint);
						parentBody = childBody;
					}
					else
					{ 
						ndDemoEntityNotify* notify = (ndDemoEntityNotify*)parentBody->GetNotifyCallback();
						notify = (ndDemoEntityNotify*)notify->m_parentBody->GetNotifyCallback();

						ndMatrix pivotFrame(notify->GetBody()->GetMatrix());
						ndMatrix effectorFrame(pivotFrame);
						pivotFrame = ndYawMatrix(90.0f * ndDegreeToRad) * pivotFrame;
						pivotFrame = ndPitchMatrix(-90.0f * ndDegreeToRad) * pivotFrame;
						effectorFrame.m_posit = childEntity->CalculateGlobalMatrix().m_posit;

						ndMatrix swivelFrame(ndGetIdentityMatrix());
						swivelFrame.m_front = (effectorFrame.m_posit - pivotFrame.m_posit).Normalize();
						swivelFrame.m_up = m_rootBody->GetMatrix().m_front;
						swivelFrame.m_right = (swivelFrame.m_front.CrossProduct(swivelFrame.m_up)).Normalize();
						swivelFrame.m_up = swivelFrame.m_right.CrossProduct(swivelFrame.m_front);

						ndFloat32 regularizer = 0.001f;
						ndIkSwivelPositionEffector* const effector = new ndIkSwivelPositionEffector(effectorFrame, pivotFrame, swivelFrame, parentBody, m_rootBody);
						effector->SetLinearSpringDamper(regularizer, 2000.0f, 50.0f);
						effector->SetAngularSpringDamper(regularizer, 2000.0f, 50.0f);

						const ndVector kneePoint(childEntity->GetParent()->CalculateGlobalMatrix().m_posit);
						const ndVector dist0(effectorFrame.m_posit - kneePoint);
						const ndVector dist1(kneePoint - pivotFrame.m_posit);
						const ndFloat32 workSpace = ndSqrt(dist0.DotProduct(dist0).GetScalar()) + ndSqrt(dist1.DotProduct(dist1).GetScalar());
						effector->SetWorkSpaceConstraints(0.0f, workSpace * 0.999f);

						world->AddJoint(effector);

						ndEffectorInfo info(effector);
						info.m_x_mapper = ndParamMapper(0.0f, workSpace * 0.999f);
						info.m_y_mapper = ndParamMapper(-80.0f * ndDegreeToRad, 80.0f * ndDegreeToRad);
						info.m_z_mapper = ndParamMapper(-90.0f * ndDegreeToRad, 90.0f * ndDegreeToRad);
						info.m_swivel_mapper = ndParamMapper(-90.0f * ndDegreeToRad, 90.0f * ndDegreeToRad);

						info.m_x = 0.98f;
						m_effectors.PushBack(info);
					}
					break;
				}
			}
		
			for (ndDemoEntity* child = childEntity->GetChild(); child; child = child->GetSibling())
			{
				childEntities[stack] = child;
				parentBones[stack] = parentBody;
				stack++;
			}
		}
		
		SetModelMass(bodies, 100.0f);
	}

	void SetModelMass(const ndFixSizeArray<ndBodyDynamic*, 64>& bodies, ndFloat32 mass) const
	{
		ndFloat32 maxVolume = -1.0e10f;
		for (ndInt32 i = 0; i < bodies.GetCount(); ++i)
		{
			ndFloat32 volume = bodies[i]->GetCollisionShape().GetVolume();
			maxVolume = ndMax(maxVolume, volume);
		}

		ndFloat32 totalVolume = 0.0f;
		for (ndInt32 i = 0; i < bodies.GetCount(); ++i)
		{
			ndFloat32 volume = bodies[i]->GetCollisionShape().GetVolume();
			if (volume < 0.01f * maxVolume)
			{
				volume = 0.01f * maxVolume;
			}
			totalVolume += volume;
		}

		ndFloat32 density = mass / totalVolume;

		for (ndInt32 i = 0; i < bodies.GetCount(); ++i)
		{
			ndBodyDynamic* const body = bodies[i];
			ndFloat32 volume = body->GetCollisionShape().GetVolume();
			if (volume < 0.01f * maxVolume)
			{
				volume = 0.01f * maxVolume;
			}
			ndFloat32 normalMass = density * volume;
			body->SetMassMatrix(normalMass, body->GetCollisionShape());
			ndVector inertia(body->GetMassMatrix());
			ndFloat32 maxInertia = ndMax(ndMax(inertia.m_x, inertia.m_y), inertia.m_z);
			ndFloat32 minInertia = ndMin(ndMin(inertia.m_x, inertia.m_y), inertia.m_z);
			if (minInertia < maxInertia * 0.125f)
			{
				minInertia = maxInertia * 0.125f;
				for (ndInt32 j = 0; j < 3; ++j)
				{
					if (inertia[j] < minInertia)
					{
						inertia[j] = minInertia;
					}
				}
			}
			body->SetMassMatrix(inertia);
		}
	}
	
	ndBodyDynamic* CreateBodyPart(ndDemoEntityManager* const scene, ndDemoEntity* const entityPart, ndBodyDynamic* const parentBone)
	{
		ndShapeInstance* const shape = entityPart->CreateCollisionFromChildren();
		ndAssert(shape);

		// create the rigid body that will make this body
		ndMatrix matrix(entityPart->CalculateGlobalMatrix());

		ndBodyDynamic* const body = new ndBodyDynamic();
		body->SetMatrix(matrix);
		body->SetCollisionShape(*shape);
		body->SetMassMatrix(1.0f, *shape);
		body->SetNotifyCallback(new ndDemoEntityNotify(scene, entityPart, parentBone));

		scene->GetWorld()->AddBody(body);
		delete shape;
		return body;
	}

	ndJointBilateralConstraint* ConnectBodyParts(ndBodyDynamic* const childBody, ndBodyDynamic* const parentBone, const ndAiBipedTest_2_Definition& definition)
	{
		ndMatrix matrix(childBody->GetMatrix());
		ndAiBipedTest_2_Definition::ndFrameMatrix frameAngle(definition.m_frameBasics);
		ndMatrix pinAndPivotInGlobalSpace(ndPitchMatrix(frameAngle.m_pitch * ndDegreeToRad) * ndYawMatrix(frameAngle.m_yaw * ndDegreeToRad) * ndRollMatrix(frameAngle.m_roll * ndDegreeToRad) * matrix);

		switch (definition.m_type)
		{
			case ndAiBipedTest_2_Definition::m_spherical:
			{
				ndIkJointSpherical* const joint = new ndIkJointSpherical(pinAndPivotInGlobalSpace, childBody, parentBone);
				//ndAiBipedTest_2_Definition::ndJointLimit jointLimits(definition.m_jointLimits);
				//joint->SetConeLimit(jointLimits.m_coneAngle * ndDegreeToRad);
				//joint->SetTwistLimits(jointLimits.m_minTwistAngle * ndDegreeToRad, jointLimits.m_maxTwistAngle * ndDegreeToRad);
				return joint;
			}

			case ndAiBipedTest_2_Definition::m_hinge:
			{
				ndIkJointHinge* const joint = new ndIkJointHinge(pinAndPivotInGlobalSpace, childBody, parentBone);

				ndAiBipedTest_2_Definition::ndJointLimit jointLimits(definition.m_jointLimits);
				joint->SetLimitState(true);
				joint->SetLimits(jointLimits.m_minTwistAngle * ndDegreeToRad, jointLimits.m_maxTwistAngle * ndDegreeToRad);
				return joint;
			}

			case ndAiBipedTest_2_Definition::m_doubleHinge:
			{
				ndJointDoubleHinge* const joint = new ndJointDoubleHinge(pinAndPivotInGlobalSpace, childBody, parentBone);

				ndAiBipedTest_2_Definition::ndJointLimit jointLimits(definition.m_jointLimits);
				joint->SetLimits0(-30.0f * ndDegreeToRad, 30.0f * ndDegreeToRad);
				joint->SetLimits1(-45.0f * ndDegreeToRad, 45.0f * ndDegreeToRad);

				joint->SetAsSpringDamper0(0.01f, 0.0f, 10.0f);
				joint->SetAsSpringDamper1(0.01f, 0.0f, 10.0f);
				return joint;
			}

			default:
				ndAssert(0);
		}
		return nullptr;
	}

	void Debug(ndConstraintDebugCallback& context) const
	{
		for (ndInt32 i = 0; i < m_effectors.GetCount(); ++i)
		{
			const ndEffectorInfo& info = m_effectors[i];
			ndJointBilateralConstraint* const joint = info.m_effector;
			joint->DebugJoint(context);
		}
	}

	void Update(ndWorld* const world, ndFloat32 timestep) 
	{
		ndModel::Update(world, timestep);

		m_rootBody->SetSleepState(false);
		const ndVector frontVector(m_rootBody->GetMatrix().m_front);
		for (ndInt32 i = 0; i < m_effectors.GetCount(); ++i)
		{
			ndEffectorInfo& info = m_effectors[i];
			const ndMatrix yaw(ndYawMatrix(-info.m_y_mapper.Interpolate(info.m_y)));
			const ndMatrix roll(ndRollMatrix(info.m_y_mapper.Interpolate(info.m_z)));

			ndVector posit(info.m_x_mapper.Interpolate(info.m_x), 0.0f, 0.0f, 1.0f);
			posit = roll.RotateVector(posit);
			posit = yaw.RotateVector(posit);
			//posit = roll.RotateVector(posit);

			info.m_effector->SetPosition(posit);

			ndMatrix swivelMatrix0;
			ndMatrix swivelMatrix1;
			info.m_effector->CalculateSwivelMatrices(swivelMatrix0, swivelMatrix1);
			const ndFloat32 angle = info.m_effector->CalculateAngle(frontVector, swivelMatrix1[1], swivelMatrix1[0]);
			info.m_effector->SetSwivelAngle(info.m_swivel_mapper.Interpolate(info.m_swivel) - angle);
		}
	}

	void ApplyControls(ndDemoEntityManager* const scene)
	{
		ndVector color(1.0f, 1.0f, 0.0f, 0.0f);
		scene->Print(color, "Control panel");

		ndEffectorInfo& info = m_effectors[0];

		bool change = false;
		ImGui::Text("distance");
		change = change | ImGui::SliderFloat("##x", &info.m_x, -0.5f, 1.0f);
		ImGui::Text("roll");
		change = change | ImGui::SliderFloat("##z", &info.m_z, -1.0f, 1.0f);
		ImGui::Text("yaw");
		change = change | ImGui::SliderFloat("##y", &info.m_y, -1.0f, 1.0f);

		ImGui::Text("swivel");
		change = change | ImGui::SliderFloat("##swivel", &info.m_swivel, -1.0f, 1.0f);

		ndEffectorInfo& info1 = m_effectors[1];
		info1.m_x = info.m_x;
		info1.m_y = info.m_y;
		info1.m_z = info.m_z;
		info1.m_swivel = info.m_swivel;

		static ndOUNoise xxxxxxx0(0.0f, 0.5f, 0.0f, 0.1f);
		static ndOUNoise xxxxxxx1(0.0f, 0.5f, 0.0f, 0.3f);
		//info.m_z = xxxxxxx0.Evaluate(1.0f / 500.0f);
		//info1.m_z = xxxxxxx1.Evaluate(1.0f / 500.0f);

		if (change)
		{
			m_rootBody->SetSleepState(false);
		}
	}

	static void ControlPanel(ndDemoEntityManager* const scene, void* const context)
	{
		ndAiBipedTest_2* const me = (ndAiBipedTest_2*)context;
		me->ApplyControls(scene);
	}

	void PostUpdate(ndWorld* const world, ndFloat32 timestep)
	{
		ndModel::PostUpdate(world, timestep);
	}

	void PostTransformUpdate(ndWorld* const world, ndFloat32 timestep)
	{
		ndModel::PostTransformUpdate(world, timestep);
	}
	
	static void BuildMannequin(ndDemoEntityManager* const scene, const ndVector& origin)
	{
		ndMatrix matrix(ndGetIdentityMatrix());
		matrix.m_posit = origin;
		matrix.m_posit.m_w = 1.0f;

		ndDemoEntity* const modelMesh = scene->LoadFbxMesh("walker.fbx");
		
		ndWorld* const world = scene->GetWorld();
		ndAiBipedTest_2* const model = new ndAiBipedTest_2(scene, modelMesh, matrix, mannequinDefinition);
		world->AddModel(model);
		//scene->Set2DDisplayRenderFunction(ndAiBipedTest_2::ControlPanel, nullptr, model);
		world->AddJoint(new ndJointFix6dof(model->m_rootBody->GetMatrix(), model->m_rootBody, world->GetSentinelBody()));
		
		delete modelMesh;
	}

	ndBodyDynamic* m_rootBody;
	ndFixSizeArray<ndEffectorInfo, 4> m_effectors;
};

void ndBipedTest_2 (ndDemoEntityManager* const scene)
{
	// build a floor
	BuildFloorBox(scene, ndGetIdentityMatrix());

	ndMatrix origin(ndGetIdentityMatrix());
	origin.m_posit.m_x += 20.0f;
	//AddCapsulesStacks(scene, origin, 10.0f, 0.25f, 0.25f, 0.5f, 10, 10, 7);

	origin.m_posit.m_x -= 20.0f;
	ndAiBipedTest_2::BuildMannequin(scene, origin.m_posit);

	ndQuaternion rot;
	origin.m_posit.m_x -= 5.0f;
	origin.m_posit.m_y = 2.0f;
	scene->SetCameraMatrix(rot, origin.m_posit);
}