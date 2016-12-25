/* Copyright (c) <2003-2016> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/


#include <toolbox_stdafx.h>
#include "SkyBox.h"
#include "TargaToOpenGl.h"
#include "DemoMesh.h"
#include "DemoEntityManager.h"
#include "DemoCamera.h"
#include "PhysicsUtils.h"
#include "DebugDisplay.h"

#if NEWTON_MAJOR_VERSION == 3
class SimpleSoftBodyEntity: public DemoEntity
{
	public:
	SimpleSoftBodyEntity(DemoEntityManager* const scene, const dVector& location)
		:DemoEntity(dGetIdentityMatrix(), NULL)
		,m_body(NULL)
		,m_indexMap(NULL)
	{
		dMatrix matrix (dGetIdentityMatrix());

		matrix.m_posit.m_x = location.m_x;
		matrix.m_posit.m_y = location.m_y;
		matrix.m_posit.m_z = location.m_z;
		ResetMatrix(*scene, matrix);

		// add an new entity to the world
		scene->Append(this);
	}

	~SimpleSoftBodyEntity()
	{
		if (m_indexMap) {
			delete[] m_indexMap; 
		}
	}


	void AddTetra (NewtonMesh* const tetrahedra, int i0, int i1, int i2, int i3, const dVector* const tetra, int layer)
	{
		NewtonMeshBeginFace(tetrahedra);
			NewtonMeshAddPoint(tetrahedra, tetra[i0].m_x, tetra[i0].m_y, tetra[i0].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
			NewtonMeshAddPoint(tetrahedra, tetra[i1].m_x, tetra[i1].m_y, tetra[i1].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
			NewtonMeshAddPoint(tetrahedra, tetra[i2].m_x, tetra[i2].m_y, tetra[i2].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
		NewtonMeshEndFace(tetrahedra);

		NewtonMeshBeginFace(tetrahedra);
			NewtonMeshAddPoint(tetrahedra, tetra[i3].m_x, tetra[i3].m_y, tetra[i3].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
			NewtonMeshAddPoint(tetrahedra, tetra[i0].m_x, tetra[i0].m_y, tetra[i0].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
			NewtonMeshAddPoint(tetrahedra, tetra[i2].m_x, tetra[i2].m_y, tetra[i2].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
		NewtonMeshEndFace(tetrahedra);

		NewtonMeshBeginFace(tetrahedra);
			NewtonMeshAddPoint(tetrahedra, tetra[i3].m_x, tetra[i3].m_y, tetra[i3].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
			NewtonMeshAddPoint(tetrahedra, tetra[i1].m_x, tetra[i1].m_y, tetra[i1].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
			NewtonMeshAddPoint(tetrahedra, tetra[i0].m_x, tetra[i0].m_y, tetra[i0].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
		NewtonMeshEndFace(tetrahedra);

		NewtonMeshBeginFace(tetrahedra);
			NewtonMeshAddPoint(tetrahedra, tetra[i3].m_x, tetra[i3].m_y, tetra[i3].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
			NewtonMeshAddPoint(tetrahedra, tetra[i2].m_x, tetra[i2].m_y, tetra[i2].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
			NewtonMeshAddPoint(tetrahedra, tetra[i1].m_x, tetra[i1].m_y, tetra[i1].m_z);
			NewtonMeshAddLayer(tetrahedra, layer);
		NewtonMeshEndFace(tetrahedra);
	}

	void BuildTetraSolidBlock(NewtonMesh* const tetrahedra, int x, int y, int z, float width, float height, float depth)
	{
		dVector points[1024];

		//make the point array;
		int index = 0;
		for (int i = 0; i <= x; ++i) {
			for (int j = 0; j <= y; ++j) {
				for (int k = 0; k <= z; ++k) {
					points[index] = dVector(width*i, height*k, depth*j);
// temp hack until I fox the com bug
points[index] -= dVector(width * 0.5f, height * 0.5f, depth * 0.5f, 0.0f);
					index++;
					dAssert (index < sizeof (points) / sizeof (points[0]));
				}
			}
		}

		// create the simple for cube one a a time but interleave them so that the neighbors cubes shared faces.  
		int layer = 0;
		for (int i = 0; i < x; ++i) {
			for (int j = 0; j < y; ++j) {
				for (int k = 0; k < z; ++k) {
					int p0 = (i * (y + 1) + j) * (z + 1) + k;
					int p1 = p0 + 1;
					int p3 = ((i + 1) * (y + 1) + j) * (z + 1) + k;
					int p2 = p3 + 1;
					int p7 = ((i + 1) * (y + 1) + (j + 1)) * (z + 1) + k;
					int p6 = p7 + 1;
					int p4 = (i * (y + 1) + (j + 1)) * (z + 1) + k;
					int p5 = p4 + 1;

					if ((i + j + k) & 1) {
						AddTetra (tetrahedra, p1, p2, p3, p6, points, layer + 0);
						AddTetra (tetrahedra, p3, p6, p7, p4, points, layer + 1);
						AddTetra (tetrahedra, p1, p4, p5, p6, points, layer + 2);
						AddTetra (tetrahedra, p1, p3, p0, p4, points, layer + 3);
						AddTetra (tetrahedra, p1, p6, p3, p4, points, layer + 4);
					} else {
						AddTetra (tetrahedra, p2, p0, p1, p5, points, layer + 0);
						AddTetra (tetrahedra, p2, p7, p3, p0, points, layer + 1);
						AddTetra (tetrahedra, p2, p5, p6, p7, points, layer + 2);
						AddTetra (tetrahedra, p0, p7, p4, p5, points, layer + 3);
						AddTetra (tetrahedra, p2, p0, p5, p7, points, layer + 4);
					}
					layer += 5;
				}
			}
		}
	}

	void BuildMesh (NewtonMesh* const tetrahedraMesh, NewtonCollision* const deformableCollision)
	{
		DemoMesh* const mesh1 = new DemoMesh(tetrahedraMesh);
		SetMesh(mesh1, dGetIdentityMatrix());
		mesh1->ResetOptimization();

		int pointCount = NewtonMeshGetPointCount(tetrahedraMesh);
		const int* const indexMap = NewtonMeshGetIndexToVertexMap(tetrahedraMesh);
		m_indexMap = new int[pointCount];
		const int* const solidIndexList = NewtonDeformableMeshGetIndexToVertexMap(deformableCollision);
		for (int i = 0; i < pointCount; i ++) {
			int j = indexMap[i];
			m_indexMap[i] = solidIndexList[j];
		}
		mesh1->Release();
	}

	void BuildRegularTetraHedra (DemoEntityManager* const scene, int materialID)
	{
		dFloat mass = 5.0f;
		NewtonWorld* const world = scene->GetNewton();

		dVector tetra[] = { dVector(-1.0f, 0.0f, -0.71f, 0.0f),
							dVector(1.0f, 0.0f, -0.71f, 0.0f),
							dVector(0.0f, -1.0f, 0.71f, 0.0f),
							dVector(0.0f, 1.0f, 0.71f, 0.0f) };

		NewtonMesh* const tetrahedra = NewtonMeshCreate(scene->GetNewton());
		NewtonMeshBeginBuild(tetrahedra);
			AddTetra (tetrahedra, 0, 1, 2, 3, tetra, 0);
		NewtonMeshEndBuild(tetrahedra);

		int material = LoadTexture("smilli.tga");
		NewtonMeshApplyBoxMapping (tetrahedra, material, material, material);
		NewtonMeshCalculateVertexNormals (tetrahedra, 60.0f * 3.1416f / 180.0f);

		// make a deformable collision mesh
		NewtonCollision* const deformableCollision = NewtonCreateDeformableSolid(world, tetrahedra, materialID);

		BuildMesh (tetrahedra, deformableCollision);

		//create a rigid body with a deformable mesh
		m_body = CreateRigidBody (scene, mass, deformableCollision);

		// do not forget to destroy this objects, else you get bad memory leaks.
		NewtonMeshDestroy (tetrahedra);
		NewtonDestroyCollision(deformableCollision);
	}


	void BuildTetraHedraCube(DemoEntityManager* const scene, int materialID)
	{
		dFloat mass = 5.0f;
		NewtonWorld* const world = scene->GetNewton();

		NewtonMesh* const tetraCube = NewtonMeshCreate(scene->GetNewton());
		NewtonMeshBeginBuild(tetraCube);
			BuildTetraSolidBlock(tetraCube, 2, 2, 15, 0.5f, 0.5f, 0.5f);
		NewtonMeshEndBuild(tetraCube);

		int material = LoadTexture("smilli.tga");
		NewtonMeshApplyBoxMapping(tetraCube, material, material, material);
		NewtonMeshCalculateVertexNormals(tetraCube, 60.0f * 3.1416f / 180.0f);

		// make a deformable collision mesh
		NewtonCollision* const deformableCollision = NewtonCreateDeformableSolid(world, tetraCube, materialID);

		BuildMesh (tetraCube, deformableCollision);

		//create a rigid body with a deformable mesh
		m_body = CreateRigidBody(scene, mass, deformableCollision);

		// do not forget to destroy this objects, else you get bad memory leaks.
		NewtonDestroyCollision(deformableCollision);
		NewtonMeshDestroy(tetraCube);
	}

	NewtonMesh* CreateFlatClothPatch(DemoEntityManager* const scene, int size_x, int size_z)
	{
		size_x += 1;
		size_z += 1;
		dAssert(size_x < 128);
		dAssert(size_z < 128);
		dFloat dimension = 0.25f;
		dVector points[128][128];

		dFloat y = 0.0f;
		int enumerator = 0;
		for (int i = 0; i < size_z; i++) {
			dFloat z = (i - size_z / 2) * dimension;
			for (int j = 0; j < size_x; j++) {
				dFloat x = (j - size_x / 2) * dimension;
				points[i][j] = dVector(x, y, z, dFloat(enumerator));
				enumerator++;
			}
		}

		NewtonMesh* const clothPatch = NewtonMeshCreate(scene->GetNewton());
		NewtonMeshBeginBuild(clothPatch);
		for (int i = 0; i < size_z - 1; i++) {
			//if (i & 1) {
			if (1) {
				for (int j = 0; j < size_x - 1; j++) {
					NewtonMeshBeginFace(clothPatch);
						NewtonMeshAddPoint(clothPatch, points[i + 0][j + 0].m_x, points[i + 0][j + 0].m_y, points[i + 0][j + 0].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);

						NewtonMeshAddPoint(clothPatch, points[i + 0][j + 1].m_x, points[i + 0][j + 1].m_y, points[i + 0][j + 1].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);

						NewtonMeshAddPoint(clothPatch, points[i + 1][j + 1].m_x, points[i + 1][j + 1].m_y, points[i + 1][j + 1].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);
					NewtonMeshEndFace(clothPatch);

					NewtonMeshBeginFace(clothPatch);
						NewtonMeshAddPoint(clothPatch, points[i + 0][j + 0].m_x, points[i + 0][j + 0].m_y, points[i + 0][j + 0].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);

						NewtonMeshAddPoint(clothPatch, points[i + 1][j + 1].m_x, points[i + 1][j + 1].m_y, points[i + 1][j + 1].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);

						NewtonMeshAddPoint(clothPatch, points[i + 1][j + 0].m_x, points[i + 1][j + 0].m_y, points[i + 1][j + 0].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);
					NewtonMeshEndFace(clothPatch);
				}
			} else {
				for (int j = 0; j < size_x - 1; j++) {
					NewtonMeshBeginFace(clothPatch);
						NewtonMeshAddPoint(clothPatch, points[i + 0][j + 0].m_x, points[i + 0][j + 0].m_y, points[i + 0][j + 0].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);

						NewtonMeshAddPoint(clothPatch, points[i + 0][j + 1].m_x, points[i + 0][j + 1].m_y, points[i + 0][j + 1].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);

						NewtonMeshAddPoint(clothPatch, points[i + 1][j + 0].m_x, points[i + 1][j + 0].m_y, points[i + 1][j + 0].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);
					NewtonMeshEndFace(clothPatch);

					NewtonMeshBeginFace(clothPatch);
						NewtonMeshAddPoint(clothPatch, points[i + 0][j + 1].m_x, points[i + 0][j + 1].m_y, points[i + 0][j + 1].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);

						NewtonMeshAddPoint(clothPatch, points[i + 1][j + 1].m_x, points[i + 1][j + 1].m_y, points[i + 1][j + 1].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);

						NewtonMeshAddPoint(clothPatch, points[i + 1][j + 0].m_x, points[i + 1][j + 0].m_y, points[i + 1][j + 0].m_z);
						NewtonMeshAddMaterial(clothPatch, 0);
					NewtonMeshEndFace(clothPatch);
				}
			}
		}
		NewtonMeshEndBuild(clothPatch);

		int material = LoadTexture("persianRug.tga");
		NewtonMeshApplyBoxMapping(clothPatch, material, material, material);
		return clothPatch;
	}


	void BuildClothPatch (DemoEntityManager* const scene, int size_x, int size_z)
	{
		NewtonWorld* const world = scene->GetNewton();

		NewtonMesh* const clothPatch = CreateFlatClothPatch(scene, size_x, size_z);
		NewtonCollision* const deformableCollision = NewtonCreateClothPatch(world, clothPatch, 0);

		dFloat mass = 8.0f;
		CreateRigidBody(scene, mass, deformableCollision);

		DemoMesh* const mesh = new DemoMesh(clothPatch);
		SetMesh(mesh, dGetIdentityMatrix());

		// do not forget to destroy this objects, else you get bad memory leaks.
		mesh->Release();
		NewtonMeshDestroy(clothPatch);
		NewtonDestroyCollision(deformableCollision);
	}


	NewtonBody* CreateRigidBody(DemoEntityManager* const scene, dFloat mass, NewtonCollision* const deformableCollision)
	{
		//create the rigid body
		NewtonWorld* const world = scene->GetNewton();
		dMatrix matrix(GetCurrentMatrix());

		//matrix.m_posit.m_y = FindFloor (world, matrix.m_posit.m_x, matrix.m_posit.m_z) + 4.0f;
		SetMatrix(*scene, dQuaternion(), matrix.m_posit);
		SetMatrix(*scene, dQuaternion(), matrix.m_posit);
		NewtonBody* const deformableBody = NewtonCreateDynamicBody(world, deformableCollision, &matrix[0][0]);

		// set the mass matrix
		NewtonBodySetMassProperties(deformableBody, mass, deformableCollision);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData(deformableBody, this);

		// assign the wood id
		//	NewtonBodySetMaterialGroupID (deformableBody, materialId);

		// set a destructor for this rigid body
		NewtonBodySetDestructorCallback(deformableBody, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback(deformableBody, DemoEntity::TransformCallback);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback(deformableBody, PhysicsApplyGravityForce);

		return deformableBody;
	}

	virtual void Render(dFloat timeStep, DemoEntityManager* const scene) const
	{
		NewtonCollision* const deformableCollision = NewtonBodyGetCollision(m_body);
		dAssert ((NewtonCollisionGetType(deformableCollision) == SERIALIZE_ID_CLOTH_PATCH) ||
				 (NewtonCollisionGetType(deformableCollision) == SERIALIZE_ID_DEFORMABLE_SOLID));

		DemoMesh* const mesh = (DemoMesh*) GetMesh();
		const dFloat* const particles = NewtonDeformableMeshGetParticleArray(deformableCollision);
		int stride = NewtonDeformableMeshGetParticleStrideInBytes(deformableCollision) / sizeof (dFloat);

		for (int i = 0; i < mesh->m_vertexCount; i ++) {
			int index = m_indexMap[i] * stride;
			mesh->m_vertex[i* 3 + 0] = particles[index + 0];
			mesh->m_vertex[i* 3 + 1] = particles[index + 1];
			mesh->m_vertex[i* 3 + 2] = particles[index + 2];
		}

//		glDisable(GL_CULL_FACE);
		DemoEntity::Render(timeStep, scene);
//		glEnable(GL_CULL_FACE);
	}
	
	NewtonBody* m_body;
	int* m_indexMap;
};

#endif




void ClothPatch(DemoEntityManager* const scene)
{
	// load the skybox
	scene->CreateSkyBox();

	// load the scene from a ngd file format
	CreateLevelMesh(scene, "flatPlane.ngd", 1);
	//CreateLevelMesh (scene, "playground.ngd", 1);

	//	dVector location (8.0f, 0.0f, -10.0f, 0.0f) ;
	dVector location(0.0f, 5.0f, 0.0f, 0.0f);

	SimpleSoftBodyEntity* const entity = new SimpleSoftBodyEntity(scene, location);
	//entity->BuildClothPatch(scene, 50, 50);
	entity->BuildClothPatch(scene, 1, 1);

	dQuaternion rot;
	dVector origin(location.m_x - 10.0f, 2.0f, location.m_z, 0.0f);
	scene->SetCameraMatrix(rot, origin);
}


void SoftBodies(DemoEntityManager* const scene)
{
	// load the sky box
	scene->CreateSkyBox();

//dMatrix xxx3(dGetIdentityMatrix());
//NewtonCollision* xxx0 = NewtonCreateBox(scene->GetNewton(), 1, 1, 1, 0, NULL);
//NewtonMesh* xxx1 = NewtonMeshCreateFromCollision(xxx0);
//
char name[2048];
//dGetWorkingFileName ("prism.off", name);
dGetWorkingFileName ("box.off", name);
NewtonMesh* const xxx1 = NewtonMeshLoadOFF(scene->GetNewton(), name);
NewtonMesh* xxx2 = NewtonMeshCreateConstrainedTetrahedralization(xxx1);


	// load the scene from a ngd file format
	CreateLevelMesh(scene, "flatPlane.ngd", 1);
	//CreateLevelMesh (scene, "playground.ngd", 1);

	dVector location(0.0f, 6.0f, 0.0f, 0.0f);

	SimpleSoftBodyEntity* const entity = new SimpleSoftBodyEntity(scene, location);
	entity->BuildRegularTetraHedra(scene, 0);
//	entity->BuildTetraHedraCube (scene, 0);

	dQuaternion rot;
	dVector origin(location.m_x - 10.0f, 2.0f, location.m_z, 0.0f);
	scene->SetCameraMatrix(rot, origin);
}
