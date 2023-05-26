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
#include "ndDemoMesh.h"
#include "ndFbxLoader.h"
#include "ndDemoSkinMesh.h"
#include "ndPhysicsUtils.h"
#include "ndPhysicsWorld.h"
#include "ndDemoEntityManager.h"
#include "ndAnimationSequence.h"

using namespace ofbx;

#define D_ANIM_BASE_FREQ ndFloat32 (30.0f)

class ndFbxImportMeshEffectNodeStackData
{
	public:
	ndFbxImportMeshEffectNodeStackData()
	{
	}

	ndFbxImportMeshEffectNodeStackData(const ofbx::Object* const fbxNode, ndMeshEffectNode* const parentNode)
		:m_fbxNode(fbxNode)
		,m_parentNode(parentNode)
	{
	}

	const ofbx::Object* m_fbxNode;
	ndMeshEffectNode* m_parentNode;
};

class ndFbxMeshEffectNodeGlobalNodeMap : public ndTree<ndMeshEffectNode*, const ofbx::Object*>
{
};

class ndFbxAnimationTrack
{
	public:
	class dCurve : public ndMeshEffectNode::ndCurve
	{
		public:
		dCurve()
			:ndMeshEffectNode::ndCurve()
		{
		}
	};

	ndFbxAnimationTrack()
	{
	}

	~ndFbxAnimationTrack()
	{
	}

	void AddKeyframe(ndFloat32 time, const ndMatrix& matrix)
	{
		ndVector scale;
		ndVector euler1;
		ndMatrix transform;
		ndMatrix eigenScaleAxis;
		matrix.PolarDecomposition(transform, scale, eigenScaleAxis);
		ndVector euler0(transform.CalcPitchYawRoll(euler1));

		AddScale(time, scale.m_x, scale.m_y, scale.m_z);
		AddPosition(time, matrix.m_posit.m_x, matrix.m_posit.m_y, matrix.m_posit.m_z);
		AddRotation(time, euler0.m_x, euler0.m_y, euler0.m_z);
	}

	void AddScale(ndFloat32 time, ndFloat32 x, ndFloat32 y, ndFloat32 z)
	{
		ndMeshEffectNode::ndCurveValue& value = m_scale.Append()->GetInfo();
		value.m_x = x;
		value.m_y = y;
		value.m_z = z;
		value.m_time = time;
	}

	void AddPosition(ndFloat32 time, ndFloat32 x, ndFloat32 y, ndFloat32 z)
	{
		ndMeshEffectNode::ndCurveValue& value = m_position.Append()->GetInfo();
		value.m_x = x;
		value.m_y = y;
		value.m_z = z;
		value.m_time = time;
	}

	void AddRotation(ndFloat32 time, ndFloat32 x, ndFloat32 y, ndFloat32 z)
	{
		ndMeshEffectNode::ndCurveValue& value = m_rotation.Append()->GetInfo();
		value.m_x = x;
		value.m_y = y;
		value.m_z = z;
		value.m_time = time;
	}

	dCurve m_scale;
	dCurve m_position;
	dCurve m_rotation;
};

ndFbxLoader::ndFbxLoader()
{
}

ndFbxLoader::~ndFbxLoader()
{
}


ndMatrix ndFbxLoader::GetCoordinateSystemMatrix(ofbx::IScene* const fbxScene)
{
	const ofbx::GlobalSettings* const globalSettings = fbxScene->getGlobalSettings();

	ndMatrix convertMatrix(ndGetIdentityMatrix());

	ndFloat32 scaleFactor = globalSettings->UnitScaleFactor;
	convertMatrix[0][0] = ndFloat32(scaleFactor / 100.0f);
	convertMatrix[1][1] = ndFloat32(scaleFactor / 100.0f);
	convertMatrix[2][2] = ndFloat32(scaleFactor / 100.0f);

	ndMatrix axisMatrix(ndGetZeroMatrix());
	//axisMatrix.m_up[globalSettings->UpAxis] = ndFloat32(globalSettings->UpAxisSign);
	//axisMatrix.m_front[globalSettings->FrontAxis] = ndFloat32(globalSettings->FrontAxisSign);
	//axisMatrix.m_right = axisMatrix.m_front.CrossProduct(axisMatrix.m_up);
	//axisMatrix = axisMatrix.Transpose();

	switch (globalSettings->UpAxis)
	{
		case UpVector_AxisX:
			ndAssert(0);
			break;

		case UpVector_AxisY:
			//axisMatrix = ndPitchMatrix(-90.0f * ndDegreeToRad) * ndYawMatrix(90.0f * ndDegreeToRad);
			axisMatrix[0][2] = ndFloat32(-1.0f);
			axisMatrix[1][0] = ndFloat32(-1.0f);
			axisMatrix[2][1] = ndFloat32(1.0f);
			axisMatrix[3][3] = ndFloat32(1.0f);
			break;

		case UpVector_AxisZ:
			//axisMatrix.m_up[globalSettings->UpAxis] = ndFloat32(globalSettings->UpAxisSign);
			//axisMatrix.m_front[globalSettings->FrontAxis] = ndFloat32(globalSettings->FrontAxisSign);
			//axisMatrix.m_right = axisMatrix.m_front.CrossProduct(axisMatrix.m_up);
			//axisMatrix = axisMatrix.Transpose();

			axisMatrix[0][2] = ndFloat32(-1.0f);
			axisMatrix[1][0] = ndFloat32(-1.0f);
			axisMatrix[2][1] = ndFloat32(1.0f);
			axisMatrix[3][3] = ndFloat32(1.0f);
			break;
	}
	
	convertMatrix = axisMatrix * convertMatrix;

	return convertMatrix;
}

ndInt32 ndFbxLoader::GetChildrenNodes(const ofbx::Object* const node, ofbx::Object** buffer)
{
	ndInt32 count = 0;
	ndInt32 index = 0;
	while (ofbx::Object* child = node->resolveObjectLink(index))
	{
		if (child->isNode())
		{
			buffer[count] = child;
			count++;
			ndAssert(count < 1024);
		}
		index++;
	}
	return count;
}

ndMatrix ndFbxLoader::ofbxMatrix2dMatrix(const ofbx::Matrix& fbxMatrix)
{
	ndMatrix matrix;
	for (ndInt32 i = 0; i < 4; ++i)
	{
		for (ndInt32 j = 0; j < 4; ++j)
		{
			matrix[i][j] = ndFloat32 (fbxMatrix.m[i * 4 + j]);
		}
	}
	return matrix;
}

void ndFbxLoader::ImportMaterials(const ofbx::Mesh* const fbxMesh, ndMeshEffect* const mesh)
{
	ndArray<ndMeshEffect::ndMaterial>& materialArray = mesh->GetMaterials();
	
	ndInt32 materialCount = fbxMesh->getMaterialCount();
	if (materialCount == 0)
	{
		ndMeshEffect::ndMaterial material;
		const ofbx::Geometry* const geom = fbxMesh->getGeometry();
		Color color(geom->getRgbDisplayColor());
		material.m_diffuse = ndVector(color.r, color.g, color.b, 1.0f);
		material.m_ambient = ndVector(color.r, color.g, color.b, 1.0f);
		material.m_specular = ndVector(color.r, color.g, color.b, 1.0f);

		materialArray.PushBack(material);
	}
	else
	{
		for (ndInt32 i = 0; i < materialCount; ++i)
		{
			ndMeshEffect::ndMaterial material;
			const ofbx::Material* const fbxMaterial = fbxMesh->getMaterial(i);
			ndAssert(fbxMaterial);

			ofbx::Color color = fbxMaterial->getDiffuseColor();
			material.m_diffuse = ndVector(color.r, color.g, color.b, 1.0f);
			
			color = fbxMaterial->getAmbientColor();
			material.m_ambient = ndVector(color.r, color.g, color.b, 1.0f);
			
			color = fbxMaterial->getSpecularColor();
			material.m_specular = ndVector(color.r, color.g, color.b, 1.0f);
			
			material.m_opacity = ndFloat32(fbxMaterial->getOpacityFactor());
			material.m_shiness = ndFloat32(fbxMaterial->getShininess());
			
			const ofbx::Texture* const texture = fbxMaterial->getTexture(ofbx::Texture::DIFFUSE);
			if (texture)
			{
				char textName[1024];
				ofbx::DataView dataView = texture->getRelativeFileName();
				dataView.toString(textName);
				char* namePtr = strrchr(textName, '\\');
				if (!namePtr)
				{
					namePtr = strrchr(textName, '/');
				}
				if (namePtr)
				{
					namePtr++;
				}
				else
				{
					namePtr = textName;
				}
				strncpy(material.m_textureName, namePtr, sizeof(material.m_textureName));
			}
 			else
			{
				strcpy(material.m_textureName, "default.tga");
			}
			materialArray.PushBack(material);
		}
	}
}

ndMatrix ndFbxLoader::GetKeyframe(ndMeshEffectNode::ndCurveValue& scale, ndMeshEffectNode::ndCurveValue& position, ndMeshEffectNode::ndCurveValue& rotation)
{
	ndMatrix scaleMatrix(ndGetIdentityMatrix());
	scaleMatrix[0][0] = scale.m_x;
	scaleMatrix[1][1] = scale.m_y;
	scaleMatrix[2][2] = scale.m_z;
	ndMatrix matrix(scaleMatrix * ndPitchMatrix(rotation.m_x) * ndYawMatrix(rotation.m_y) * ndRollMatrix(rotation.m_z));
	matrix.m_posit = ndVector(position.m_x, position.m_y, position.m_z, 1.0f);
	return matrix;
}

void ndFbxLoader::FreezeScale(ndMeshEffectNode* const entity)
{
	ndInt32 stack = 1;
	ndMatrix parentMatrix[1024];
	ndMeshEffectNode* entBuffer[1024];
	
	entBuffer[0] = entity;
	parentMatrix[0] = ndGetIdentityMatrix();

	while (stack)
	{
		stack--;
		ndMatrix scaleMatrix(parentMatrix[stack]);
		ndMeshEffectNode* const ent = entBuffer[stack];

		ndVector scale;
		ndMatrix stretchAxis;
		ndMatrix transformMatrix;
		
		ndMatrix matrix(ent->m_matrix * scaleMatrix);
		matrix.PolarDecomposition(transformMatrix, scale, stretchAxis);
		ent->m_matrix = transformMatrix;
		scaleMatrix = ndMatrix(ndGetIdentityMatrix(), scale, stretchAxis);

		ndSharedPtr<ndMeshEffect> mesh = ent->GetMesh();
		if (*mesh)
		{
			matrix = ent->m_meshMatrix * scaleMatrix;
			matrix.PolarDecomposition(transformMatrix, scale, stretchAxis);
			ent->m_meshMatrix = transformMatrix;
			ndMatrix meshMatrix(ndGetIdentityMatrix(), scale, stretchAxis);
			mesh->ApplyTransform(meshMatrix);
		}

		if (ent->GetScaleCurve().GetCount())
		{
			ndMeshEffectNode::ndCurve::ndNode* scaleNode = ent->GetScaleCurve().GetFirst();
			ndMeshEffectNode::ndCurve::ndNode* positNode = ent->GetPositCurve().GetFirst();
			ndMeshEffectNode::ndCurve::ndNode* rotationNode = ent->GetRotationCurve().GetFirst();

			ndMatrix parentAnimScale (parentMatrix[stack]);
			for (ndInt32 i = 0; i < ent->GetScaleCurve().GetCount(); ++i)
			{
				ndMeshEffectNode::ndCurveValue& scaleValue = scaleNode->GetInfo();
				ndMeshEffectNode::ndCurveValue& positValue = positNode->GetInfo();
				ndMeshEffectNode::ndCurveValue& rotationValue = rotationNode->GetInfo();

				ndVector animScale;
				ndMatrix animTransformMatrix;
				ndMatrix keyframe(GetKeyframe(scaleValue, positValue, rotationValue) * parentAnimScale);
				keyframe.PolarDecomposition(animTransformMatrix, animScale, stretchAxis);

				ndVector euler0;
				ndVector euler(animTransformMatrix.CalcPitchYawRoll(euler0));

				rotationValue.m_x = euler.m_x;
				rotationValue.m_y = euler.m_y;
				rotationValue.m_z = euler.m_z;

				positValue.m_x = animTransformMatrix.m_posit.m_x;
				positValue.m_y = animTransformMatrix.m_posit.m_y;
				positValue.m_z = animTransformMatrix.m_posit.m_z;

				//scaleValue.m_x = animScale.m_x;
				//scaleValue.m_y = animScale.m_y;
				//scaleValue.m_z = animScale.m_z;

				scaleValue.m_x = 1.0f;
				scaleValue.m_y = 1.0f;
				scaleValue.m_z = 1.0f;

				scaleNode = scaleNode->GetNext();
				positNode = positNode->GetNext();
				rotationNode = rotationNode->GetNext();
			}
		}

		for (ndMeshEffectNode* child = ent->GetFirstChild(); child; child = child->GetNext())
		{
			entBuffer[stack] = child;
			parentMatrix[stack] = scaleMatrix;
			stack++;
		}
	}
}

void ndFbxLoader::ApplyTransform(ndMeshEffectNode* const entity, const ndMatrix& transform)
{
	entity->ApplyTransform(transform);
	//ndInt32 stack = 1;
	//ndMeshEffectNode* entBuffer[1024];
	//entBuffer[0] = entity;
	//ndMatrix invTransform(transform.Inverse4x4());
	//while (stack)
	//{
	//	stack--;
	//	ndMeshEffectNode* const ent = entBuffer[stack];
	//
	//	ndMatrix entMatrix(invTransform * ent->m_matrix * transform);
	//	ent->m_matrix = entMatrix;
	//
	//	ndSharedPtr<ndMeshEffect> mesh = ent->GetMesh();
	//	if (*mesh)
	//	{
	//		ndMatrix meshMatrix(invTransform * ent->m_meshMatrix * transform);
	//		ent->m_meshMatrix = meshMatrix;
	//		mesh->ApplyTransform(transform);
	//	}
	//
	//	if (ent->GetScaleCurve().GetCount())
	//	{
	//		ndMeshEffectNode::ndCurve::ndNode* positNode = ent->GetPositCurve().GetFirst();
	//		ndMeshEffectNode::ndCurve::ndNode* rotationNode = ent->GetRotationCurve().GetFirst();
	//
	//		ndMeshEffectNode::ndCurveValue scaleValue;
	//		scaleValue.m_x = 1.0f;
	//		scaleValue.m_y = 1.0f;
	//		scaleValue.m_z = 1.0f;
	//		for (ndInt32 i = 0; i < ent->GetScaleCurve().GetCount(); ++i)
	//		{
	//			ndMeshEffectNode::ndCurveValue& positValue = positNode->GetInfo();
	//			ndMeshEffectNode::ndCurveValue& rotationValue = rotationNode->GetInfo();
	//
	//			ndVector animScale;
	//			ndMatrix stretchAxis;
	//			ndMatrix animTransformMatrix;
	//			ndMatrix keyframe(invTransform * GetKeyframe(scaleValue, positValue, rotationValue) * transform);
	//			keyframe.PolarDecomposition(animTransformMatrix, animScale, stretchAxis);
	//
	//			ndVector euler0;
	//			ndVector euler(animTransformMatrix.CalcPitchYawRoll(euler0));
	//
	//			rotationValue.m_x = euler.m_x;
	//			rotationValue.m_y = euler.m_y;
	//			rotationValue.m_z = euler.m_z;
	//
	//			positValue.m_x = animTransformMatrix.m_posit.m_x;
	//			positValue.m_y = animTransformMatrix.m_posit.m_y;
	//			positValue.m_z = animTransformMatrix.m_posit.m_z;
	//
	//			positNode = positNode->GetNext();
	//			rotationNode = rotationNode->GetNext();
	//		}
	//	}
	//
	//	for (ndMeshEffectNode* child = ent->GetFirstChild(); child; child = child->GetNext())
	//	{
	//		entBuffer[stack] = child;
	//		stack++;
	//	}
	//}
}

void ndFbxLoader::AlignToWorld(ndMeshEffectNode* const entity)
{
	ndMeshEffectNode* entBuffer[1024];

	ndInt32 stack = 0;
	ndMatrix rotation(entity->m_matrix);
	ndVector posit(rotation.m_posit);
	rotation.m_posit = ndVector::m_wOne;

	ndMatrix invRotation(rotation.OrthoInverse());
	for (ndMeshEffectNode* child = entity->GetFirstChild(); child; child = child->GetNext())
	{
		entBuffer[stack] = child;
		stack++;
	}

	entity->m_matrix = ndGetIdentityMatrix();
	entity->m_matrix.m_posit = posit;
	if (entity->GetScaleCurve().GetCount())
	{
		ndAssert(0);
	}

	while (stack)
	{
		stack--;
		ndMeshEffectNode* const ent = entBuffer[stack];

		ndMatrix entMatrix(invRotation * ent->m_matrix * rotation);
		ent->m_matrix = entMatrix;

		ndSharedPtr<ndMeshEffect> mesh = ent->GetMesh();
		if (*mesh)
		{
			ndMatrix meshMatrix(invRotation * ent->m_meshMatrix * rotation);
			ent->m_meshMatrix = meshMatrix;
			mesh->ApplyTransform(rotation);
		}

		if (ent->GetScaleCurve().GetCount())
		{
			ndMeshEffectNode::ndCurve::ndNode* positNode = ent->GetPositCurve().GetFirst();
			ndMeshEffectNode::ndCurve::ndNode* rotationNode = ent->GetRotationCurve().GetFirst();

			ndMeshEffectNode::ndCurveValue scaleValue;
			scaleValue.m_x = 1.0f;
			scaleValue.m_y = 1.0f;
			scaleValue.m_z = 1.0f;
			for (ndInt32 i = 0; i < ent->GetScaleCurve().GetCount(); ++i)
			{
				ndMeshEffectNode::ndCurveValue& positValue = positNode->GetInfo();
				ndMeshEffectNode::ndCurveValue& rotationValue = rotationNode->GetInfo();

				ndVector animScale;
				ndMatrix stretchAxis;
				ndMatrix animTransformMatrix;
				ndMatrix keyframe(invRotation * GetKeyframe(scaleValue, positValue, rotationValue) * rotation);
				keyframe.PolarDecomposition(animTransformMatrix, animScale, stretchAxis);

				ndVector euler0;
				ndVector euler(animTransformMatrix.CalcPitchYawRoll(euler0));

				rotationValue.m_x = euler.m_x;
				rotationValue.m_y = euler.m_y;
				rotationValue.m_z = euler.m_z;

				positValue.m_x = animTransformMatrix.m_posit.m_x;
				positValue.m_y = animTransformMatrix.m_posit.m_y;
				positValue.m_z = animTransformMatrix.m_posit.m_z;

				positNode = positNode->GetNext();
				rotationNode = rotationNode->GetNext();
			}
		}

		for (ndMeshEffectNode* child = ent->GetFirstChild(); child; child = child->GetNext())
		{
			entBuffer[stack] = child;
			stack++;
		}
	}
}

void ndFbxLoader::ApplyAllTransforms(ndMeshEffectNode* const meshEffectNode, const ndMatrix& unitMatrix, const ndMatrix& upAxis)
{
	FreezeScale(meshEffectNode);
	ApplyTransform(meshEffectNode, unitMatrix);
	meshEffectNode->m_matrix = meshEffectNode->m_matrix * upAxis;
	if (meshEffectNode->GetScaleCurve().GetCount())
	{
		ndAssert(0);
	}
	AlignToWorld(meshEffectNode);
	OnPostLoad(meshEffectNode);
}

void ndFbxLoader::ImportMeshNode(ofbx::Object* const fbxNode, ndFbxMeshEffectNodeGlobalNodeMap& nodeMap)
{
	const ofbx::Mesh* const fbxMesh = (ofbx::Mesh*)fbxNode;

	ndAssert(nodeMap.Find(fbxNode));
	ndMeshEffectNode* const entity = nodeMap.Find(fbxNode)->GetInfo();
	ndSharedPtr<ndMeshEffect> mesh(new ndMeshEffect());
	
	ndMatrix pivotMatrix(ofbxMatrix2dMatrix(fbxMesh->getGeometricMatrix()));
	entity->m_meshMatrix = pivotMatrix;
	
	const ofbx::Geometry* const geom = fbxMesh->getGeometry();
	const ofbx::Vec3* const vertices = geom->getVertices();
	ndInt32 indexCount = geom->getIndexCount();
	
	ndArray<ndInt32> indexArray;
	indexArray.SetCount(indexCount);
	memcpy(&indexArray[0], geom->getFaceIndices(), indexCount * sizeof(ndInt32));
	
	ndInt32 faceCount = 0;
	for (ndInt32 i = 0; i < indexCount; ++i)
	{
		if (indexArray[i] < 0)
		{
			faceCount++;
		}
	}
	
	ndArray<ndInt32> faceIndexArray;
	ndArray<ndInt32> faceMaterialArray;

	faceIndexArray.SetCount(faceCount);
	faceMaterialArray.SetCount(faceCount);
	
	ImportMaterials(fbxMesh, *mesh);
	
	ndInt32 count = 0;
	ndInt32 faceIndex = 0;
	const ndArray<ndMeshEffect::ndMaterial>& materialArray = mesh->GetMaterials();
	ndInt32 materialId = (materialArray.GetCount() <= 1) ? 0 : -1;
	for (ndInt32 i = 0; i < indexCount; ++i)
	{
		count++;
		if (indexArray[i] < 0)
		{
			indexArray[i] = -indexArray[i] - 1;
			faceIndexArray[faceIndex] = count;
			if (materialId == 0)
			{
				faceMaterialArray[faceIndex] = materialId;
			}
			else
			{
				ndInt32 fbxMatIndex = geom->getMaterials()[faceIndex];
				faceMaterialArray[faceIndex] = fbxMatIndex;
			}
			count = 0;
			faceIndex++;
		}
	}
	
	ndMeshEffect::dMeshVertexFormat format;
	format.m_vertex.m_data = &vertices[0].x;
	format.m_vertex.m_indexList = &indexArray[0];
	format.m_vertex.m_strideInBytes = sizeof(ofbx::Vec3);
	
	format.m_faceCount = faceCount;
	format.m_faceIndexCount = &faceIndexArray[0];
	format.m_faceMaterial = &faceMaterialArray[0];
	
	ndArray<ndVector> normalArray;
	if (geom->getNormals())
	{
		//normalArray.Resize(indexCount);
		normalArray.SetCount(indexCount);
		const ofbx::Vec3* const normals = geom->getNormals();
		for (ndInt32 i = 0; i < indexCount; ++i)
		{
			ofbx::Vec3 n = normals[i];
			normalArray[i] = ndVector(ndFloat32(n.x), ndFloat32(n.y), ndFloat32(n.z), ndFloat32(0.0f));
		}
	
		format.m_normal.m_data = &normalArray[0].m_x;
		format.m_normal.m_indexList = &indexArray[0];
		format.m_normal.m_strideInBytes = sizeof(ndVector);
	}
	
	ndArray<ndVector> uvArray;
	if (geom->getUVs())
	{
		//uvArray.Resize(indexCount);
		uvArray.SetCount(indexCount);
		const ofbx::Vec2* const uv = geom->getUVs();
		for (ndInt32 i = 0; i < indexCount; ++i)
		{
			ofbx::Vec2 n = uv[i];
			uvArray[i] = ndVector(ndFloat32(n.x), ndFloat32(n.y), ndFloat32(0.0f), ndFloat32(0.0f));
		}
		format.m_uv0.m_data = &uvArray[0].m_x;
		format.m_uv0.m_indexList = &indexArray[0];
		format.m_uv0.m_strideInBytes = sizeof(ndVector);
	}
	
	// import skin if there is any
	if (geom->getSkin())
	{
		const ofbx::Skin* const skin = geom->getSkin();
		ndInt32 clusterCount = skin->getClusterCount();
	
		ndTree <const ofbx::Cluster*, const Object*> clusterBoneMap;
		for (ndInt32 i = 0; i < clusterCount; ++i)
		{
			const ofbx::Cluster* const cluster = skin->getCluster(i);
			const ofbx::Object* const link = cluster->getLink();
			clusterBoneMap.Insert(cluster, link);
		}
	
		for (ndInt32 i = 0; i < clusterCount; ++i)
		{
			const ofbx::Cluster* const fbxCluster = skin->getCluster(i);
			const ofbx::Object* const fbxBone = fbxCluster->getLink();
			if (nodeMap.Find(fbxBone))
			{
				ndMeshEffect::dVertexCluster* const cluster = mesh->CreateCluster(fbxBone->name);
	
				ndAssert(fbxCluster->getIndicesCount() == fbxCluster->getWeightsCount());
				ndInt32 clusterIndexCount = fbxCluster->getIndicesCount();
				const ndInt32* const indices = fbxCluster->getIndices();
				const double* const weights = fbxCluster->getWeights();
				for (ndInt32 j = 0; j < clusterIndexCount; ++j)
				{
					cluster->m_vertexIndex.PushBack(indices[j]);
					cluster->m_vertexWeigh.PushBack(ndFloat32(weights[j]));
				}
			}
		}
	}
	
	mesh->BuildFromIndexList(&format);
	entity->SetMesh(mesh);
	//mesh->RepairTJoints();
}

ndMeshEffectNode* ndFbxLoader::LoadMeshEffectNodeHierarchy(ofbx::IScene* const fbxScene, ndFbxMeshEffectNodeGlobalNodeMap& nodeMap)
{
	ndInt32 stack = 0;
	ndFixSizeArray<ofbx::Object*, 1024> buffer;
	ndFixSizeArray <ndFbxImportMeshEffectNodeStackData, 1024> nodeStack;

	buffer.SetCount(1024);
	nodeStack.SetCount(1024);

	const ofbx::Object* const rootNode = fbxScene->getRoot();
	ndAssert(rootNode);
	stack = GetChildrenNodes(rootNode, &buffer[0]);
	
	ndMeshEffectNode* rootEntity = nullptr;
	if (stack > 1)
	{
		rootEntity = new ndMeshEffectNode(nullptr);
		rootEntity->SetName("dommyRoot");
	}
	
	for (ndInt32 i = 0; i < stack; ++i)
	{
		ofbx::Object* const child = buffer[stack - i - 1];
		nodeStack[i] = ndFbxImportMeshEffectNodeStackData(child, rootEntity);
	}
	
	while (stack)
	{
		stack--;
		ndFbxImportMeshEffectNodeStackData data(nodeStack[stack]);
	
		ndMeshEffectNode* const node = new ndMeshEffectNode(data.m_parentNode);
		if (!rootEntity)
		{
			rootEntity = node;
		}
	
		const ndMatrix localMatrix(ofbxMatrix2dMatrix(data.m_fbxNode->getLocalTransform()));
	
		node->SetName(data.m_fbxNode->name);
		node->m_matrix = localMatrix;
	
		nodeMap.Insert(node, data.m_fbxNode);
		const ndInt32 count = GetChildrenNodes(data.m_fbxNode, &buffer[0]);
		for (ndInt32 i = 0; i < count; ++i)
		{
			ofbx::Object* const child = buffer[count - i - 1];
			nodeStack[stack] = ndFbxImportMeshEffectNodeStackData(child, node);
			stack++;
			ndAssert(stack < ndInt32(sizeof(nodeStack) / sizeof(nodeStack[0])));
		}
	}

	return rootEntity;
}

ndMeshEffectNode* ndFbxLoader::FbxToMeshEffectNode(ofbx::IScene* const fbxScene)
{
	ndFbxMeshEffectNodeGlobalNodeMap nodeMap;
	ndMeshEffectNode* const entity = LoadMeshEffectNodeHierarchy(fbxScene, nodeMap);

	ndFbxMeshEffectNodeGlobalNodeMap::Iterator iter(nodeMap);
	for (iter.Begin(); iter; iter++)
	{
		ofbx::Object* const fbxNode = (ofbx::Object*)iter.GetKey();
		ofbx::Object::Type type = fbxNode->getType();
		switch (type)
		{
			case ofbx::Object::Type::MESH:
			{
				ImportMeshNode(fbxNode, nodeMap);
				break;
			}
	
			case ofbx::Object::Type::NULL_NODE:
			{
				break;
			}
	
			case ofbx::Object::Type::LIMB_NODE:
			{
				break;
			}
	
			//case FbxNodeAttribute::eLine:
			//{
			//	ImportLineShape(fbxScene, ngdScene, fbxNode, node, meshCache, materialCache, textureCache, usedMaterials);
			//	break;
			//}
			//case FbxNodeAttribute::eNurbsCurve:
			//{
			//	ImportNurbCurveShape(fbxScene, ngdScene, fbxNode, node, meshCache, materialCache, textureCache, usedMaterials);
			//	break;
			//}
			//case FbxNodeAttribute::eMarker:
			//case FbxNodeAttribute::eNurbs:
			//case FbxNodeAttribute::ePatch:
			//case FbxNodeAttribute::eCamera:
			//case FbxNodeAttribute::eCameraStereo:
			//case FbxNodeAttribute::eCameraSwitcher:
			//case FbxNodeAttribute::eLight:
			//case FbxNodeAttribute::eOpticalReference:
			//case FbxNodeAttribute::eOpticalMarker:
			//case FbxNodeAttribute::eTrimNurbsSurface:
			//case FbxNodeAttribute::eBoundary:
			//case FbxNodeAttribute::eNurbsSurface:
			//case FbxNodeAttribute::eShape:
			//case FbxNodeAttribute::eLODGroup:
			//case FbxNodeAttribute::eSubDiv:
			//case FbxNodeAttribute::eCachedEffect:
			//case FbxNodeAttribute::eUnknown:
			default:
				ndAssert(0);
				break;
		}
	}

	return entity;
}

void ndFbxLoader::LoadAnimationCurve(ndTree <ndFbxAnimationTrack, ndString>& tracks, const ofbx::IScene* const, const ofbx::Object* const bone, const ofbx::AnimationLayer* const animLayer, ndFloat32 timestep, int framesCount)
{
	const ofbx::AnimationCurveNode* const scaleNode = animLayer->getCurveNode(*bone, "Lcl Scaling");
	const ofbx::AnimationCurveNode* const rotationNode = animLayer->getCurveNode(*bone, "Lcl Rotation");
	const ofbx::AnimationCurveNode* const translationNode = animLayer->getCurveNode(*bone, "Lcl Translation");
	if (scaleNode || rotationNode || translationNode)
	{
		ndFbxAnimationTrack& track = tracks.Insert(bone->name)->GetInfo();

		Vec3 scale;
		Vec3 rotation;
		Vec3 translation;

		ndFloat32 timeAcc = 0.0f;
		//ndFloat32 timestep = m_timestep;

		ndVector scale1;
		ndVector euler1;
		ndMatrix transform;
		ndMatrix eigenScaleAxis;
		ndMatrix boneMatrix(ofbxMatrix2dMatrix(bone->getLocalTransform()));
		boneMatrix.PolarDecomposition(transform, scale1, eigenScaleAxis);
		ndVector euler0(transform.CalcPitchYawRoll(euler1));
		euler0 = euler0.Scale(180.0f / ndPi);
		//for (ndInt32 i = 0; i < m_framesCount; ++i)
		for (ndInt32 i = 0; i < framesCount; ++i)
		{
			scale.x = scale1.m_x;
			scale.y = scale1.m_y;
			scale.z = scale1.m_z;
			rotation.x = euler0.m_x;
			rotation.y = euler0.m_y;
			rotation.z = euler0.m_z;
			translation.x = transform.m_posit.m_x;
			translation.y = transform.m_posit.m_y;
			translation.z = transform.m_posit.m_z;

			if (scaleNode)
			{
				scale = scaleNode->getNodeLocalTransform(timeAcc);
			}
			if (rotationNode)
			{
				rotation = rotationNode->getNodeLocalTransform(timeAcc);
			}
			if (translationNode)
			{
				translation = translationNode->getNodeLocalTransform(timeAcc);
			}
			ndMatrix matrix(ofbxMatrix2dMatrix(bone->evalLocal(translation, rotation, scale)));
			track.AddKeyframe(timeAcc, matrix);

			timeAcc += timestep;
		}
	}
}

void ndFbxLoader::LoadAnimationLayer(ndTree <ndFbxAnimationTrack, ndString>& tracks, const ofbx::IScene* const fbxScene, const ofbx::AnimationLayer* const animLayer)
{
	ndInt32 stack = 0;
	ofbx::Object* stackPool[1024];

	const ofbx::Object* const rootNode = fbxScene->getRoot();
	ndAssert(rootNode);
	stack = GetChildrenNodes(rootNode, stackPool);

	const ofbx::TakeInfo* const animationInfo = fbxScene->getTakeInfo(0);
	//animation.m_length = ndFloat32(animationInfo->local_time_to - animationInfo->local_time_from);

	ndFloat32 period = ndFloat32(animationInfo->local_time_to - animationInfo->local_time_from);
	ndFloat32 framesFloat = period * D_ANIM_BASE_FREQ;

	ndInt32 frames = ndInt32(ndFloor(framesFloat));
	ndAssert(frames > 0);
	ndFloat32 timestep = period / (ndFloat32)frames;

	//m_length = period;
	//m_timestep = timestep;
	//m_framesCount = frames;

	while (stack)
	{
		stack--;
		ofbx::Object* const bone = stackPool[stack];
		LoadAnimationCurve(tracks, fbxScene, bone, animLayer, timestep, frames);

		stack += GetChildrenNodes(bone, &stackPool[stack]);
		ndAssert(stack < ndInt32(sizeof(stackPool) / sizeof(stackPool[0]) - 64));
	}
}

//dFbxAnimation(const ofbx::IScene* const fbxScene, ndMeshEffectNode* const model)
void ndFbxLoader::LoadAnimation(const ofbx::IScene* const fbxScene, ndMeshEffectNode* const model)
//	:ndTree <ndFbxAnimationTrack, ndString>()
//	, m_length(0.0f)
//	, m_timestep(0.0f)
//	, m_framesCount(0)
{
	ndInt32 animationCount = fbxScene->getAnimationStackCount();
	// only load one animation per file
	animationCount = 1;

	ndTree <ndFbxAnimationTrack, ndString> tracks;
	for (ndInt32 i = 0; i < animationCount; ++i)
	{
		const ofbx::AnimationStack* const animStack = fbxScene->getAnimationStack(i);

		ndInt32 layerCount = 0;
		while (const ofbx::AnimationLayer* const animLayer = animStack->getLayer(layerCount))
		{
			LoadAnimationLayer(tracks, fbxScene, animLayer);
			layerCount++;
			// only one layer per file 
			break;
		}
	}

	ndInt32 stack = 1;
	ndMeshEffectNode* entBuffer[1024];
	entBuffer[0] = model;
	while (stack)
	{
		stack--;
		ndMeshEffectNode* const ent = entBuffer[stack];
		//ndNode* const node = Find(ent->GetName());
		ndTree <ndFbxAnimationTrack, ndString>::ndNode* node = tracks.Find(ent->GetName());
		if (node)
		{
			ndFbxAnimationTrack* const track = &node->GetInfo();

			ndMeshEffectNode::ndCurve& scale = ent->GetScaleCurve();
			scale.RemoveAll();
			for (ndMeshEffectNode::ndCurve::ndNode* srcNode = track->m_scale.GetFirst(); srcNode; srcNode = srcNode->GetNext())
			{
				scale.Append(srcNode->GetInfo());
			}

			ndMeshEffectNode::ndCurve& posit = ent->GetPositCurve();
			posit.RemoveAll();
			for (ndMeshEffectNode::ndCurve::ndNode* srcNode = track->m_position.GetFirst(); srcNode; srcNode = srcNode->GetNext())
			{
				posit.Append(srcNode->GetInfo());
			}

			ndMeshEffectNode::ndCurve& rotation = ent->GetRotationCurve();
			rotation.RemoveAll();
			for (ndMeshEffectNode::ndCurve::ndNode* srcNode = track->m_rotation.GetFirst(); srcNode; srcNode = srcNode->GetNext())
			{
				rotation.Append(srcNode->GetInfo());
			}
		}

		for (ndMeshEffectNode* child = ent->GetFirstChild(); child; child = child->GetNext())
		{
			entBuffer[stack] = child;
			stack++;
		}
	}
}

void ndFbxLoader::OptimizeCurve(ndMeshEffectNode::ndCurve& curve)
{
	const ndFloat32 tol = 5.0e-5f;
	const ndFloat32 tol2 = tol * tol;

	auto Interpolate = [](ndFloat32 x0, ndFloat32 t0, ndFloat32 x1, ndFloat32 t1, ndFloat32 t)
	{
		return x0 + (x1 - x0) * (t - t0) / (t1 - t0);
	};

	for (ndMeshEffectNode::ndCurve::ndNode* node0 = curve.GetFirst(); node0->GetNext(); node0 = node0->GetNext())
	{
		const ndMeshEffectNode::ndCurveValue& value0 = node0->GetInfo();
		for (ndMeshEffectNode::ndCurve::ndNode* node1 = node0->GetNext()->GetNext(); node1; node1 = node1->GetNext())
		{
			const ndMeshEffectNode::ndCurveValue& value1 = node1->GetPrev()->GetInfo();
			const ndMeshEffectNode::ndCurveValue& value2 = node1->GetInfo();
			ndVector p1(value1.m_x, value1.m_y, value1.m_z, ndFloat32(0.0f));
			ndVector p2(value2.m_x, value2.m_y, value2.m_z, ndFloat32(0.0f));

			ndFloat32 dist_x = value1.m_x - Interpolate(value0.m_x, value0.m_time, value2.m_x, value2.m_time, value1.m_time);
			ndFloat32 dist_y = value1.m_y - Interpolate(value0.m_y, value0.m_time, value2.m_y, value2.m_time, value1.m_time);
			ndFloat32 dist_z = value1.m_z - Interpolate(value0.m_z, value0.m_time, value2.m_z, value2.m_time, value1.m_time);

			ndVector err(dist_x, dist_y, dist_z, 0.0f);
			ndFloat32 mag2 = err.DotProduct(err).GetScalar();
			if (mag2 > tol2)
			{
				break;
			}
			curve.Remove(node1->GetPrev());
		}
	}

	if (curve.GetCount() == 2)
	{
		const ndMeshEffectNode::ndCurveValue& value0 = curve.GetFirst()->GetInfo();
		const ndMeshEffectNode::ndCurveValue& value1 = curve.GetFirst()->GetNext()->GetInfo();

		ndFloat32 dist_x = value1.m_x - value0.m_x;
		ndFloat32 dist_y = value1.m_y - value0.m_y;
		ndFloat32 dist_z = value1.m_z - value0.m_z;

		ndVector err(dist_x, dist_y, dist_z, 0.0f);
		ndFloat32 mag2 = err.DotProduct(err).GetScalar();
		if (mag2 < tol2)
		{
			curve.Remove(curve.GetFirst()->GetNext());
		}
	}
}

void ndFbxLoader::OptimizeRotationCurve(ndMeshEffectNode::ndCurve& curve)
{
	auto AngleAlias = [](ndFloat32 angleA, ndFloat32 angleB)
	{
		ndFloat32 s1 = ndSin(angleB);
		ndFloat32 c1 = ndCos(angleB);
		ndFloat32 s0 = ndSin(angleA);
		ndFloat32 c0 = ndCos(angleA);

		ndFloat32 s = s1 * c0 - s0 * c1;
		ndFloat32 c = c1 * c0 + s0 * s1;
		ndFloat32 delta = ndAtan2(s, c);
		return delta;
	};

	ndMeshEffectNode::ndCurveValue eulerRef(curve.GetFirst()->GetInfo());
	for (ndMeshEffectNode::ndCurve::ndNode* node = curve.GetFirst()->GetNext(); node->GetNext(); node = node->GetNext())
	{
		ndMeshEffectNode::ndCurveValue value(node->GetInfo());
		ndFloat32 angleError = AngleAlias(value.m_z, eulerRef.m_z);
		if (ndAbs(angleError) > ndPi * ndFloat32(0.5f))
		{
			ndAssert(0);
		}
		eulerRef = value;
	}

	OptimizeCurve(curve);
}


ndAnimationSequence* ndFbxLoader::CreateSequence(ndMeshEffectNode* const model, const char* const name)
{
	ndAnimationSequence* const sequence = new ndAnimationSequence;
	sequence->SetName(name);
	//sequence->m_period = m_length;

	ndInt32 stack = 1;
	ndMeshEffectNode* entBuffer[1024];
	entBuffer[0] = model;
	while (stack)
	{
		stack--;
		ndMeshEffectNode* const ent = entBuffer[stack];

		if (ent->GetScaleCurve().GetCount())
		{
			ndAnimationKeyFramesTrack* const track = sequence->AddTrack();
			track->m_name = ent->GetName();
			//dTrace(("name: %s\n", track->m_name.GetStr()));

			ndMeshEffectNode::ndCurve& positCurve = ent->GetPositCurve();
			OptimizeCurve(positCurve);
			for (ndMeshEffectNode::ndCurve::ndNode* srcNode = positCurve.GetFirst(); srcNode; srcNode = srcNode->GetNext())
			{
				ndMeshEffectNode::ndCurveValue& keyFrame = srcNode->GetInfo();
				track->m_position.m_param.PushBack(keyFrame.m_time);
				track->m_position.PushBack(ndVector(keyFrame.m_x, keyFrame.m_y, keyFrame.m_z, ndFloat32(1.0f)));
			}

			ndQuaternion rotation;
			ndMeshEffectNode::ndCurve& rotationCurve = ent->GetRotationCurve();
			OptimizeRotationCurve(rotationCurve);
			for (ndMeshEffectNode::ndCurve::ndNode* srcNode = rotationCurve.GetFirst(); srcNode; srcNode = srcNode->GetNext())
			{
				ndMeshEffectNode::ndCurveValue& keyFrame = srcNode->GetInfo();

				const ndMatrix transform(ndPitchMatrix(keyFrame.m_x) * ndYawMatrix(keyFrame.m_y) * ndRollMatrix(keyFrame.m_z));
				ndQuaternion quat(transform);
				ndAssert(quat.DotProduct(quat).GetScalar() > 0.999f);
				ndAssert(quat.DotProduct(quat).GetScalar() < 1.001f);

				if (track->m_rotation.GetCount())
				{
					ndFloat32 dot = quat.DotProduct(rotation).GetScalar();
					if (dot < ndFloat32(0.0f))
					{
						quat = quat.Scale(ndFloat32(-1.0f));
					}
				}

				track->m_rotation.PushBack(quat);
				track->m_rotation.m_param.PushBack(keyFrame.m_time);

				rotation = quat;
			}
		}

		for (ndMeshEffectNode* child = ent->GetFirstChild(); child; child = child->GetNext())
		{
			entBuffer[stack] = child;
			stack++;
		}
	}

	return sequence;
}

ndMeshEffectNode* ndFbxLoader::LoadFbxMesh(const char* const fileName, bool loadAnimation)
{
	char outPathName[1024];
	dGetWorkingFileName(fileName, outPathName);

	FILE* const fp = fopen(outPathName, "rb");
	if (!fp)
	{
		ndAssert(0);
		return nullptr;
	}

	size_t readBytes = 0;
	readBytes++;
	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	ndArray<ofbx::u8> content;
	content.SetCount(file_size);
	readBytes = fread(&content[0], 1, size_t(file_size), fp);
	ndSharedPtr<ofbx::IScene> fbxScene(ofbx::load(&content[0], file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE));

	const ndMatrix convertMatrix(GetCoordinateSystemMatrix(*fbxScene));
	ndMeshEffectNode* const meshEffectNode = FbxToMeshEffectNode(*fbxScene);
	if (loadAnimation)
	{
		//dFbxAnimation animation (*fbxScene, meshEffectNode);
		LoadAnimation(*fbxScene, meshEffectNode);
	}

	ndMatrix upAxis((fbxScene->getGlobalSettings()->UpAxis != UpVector_AxisY) ? ndGetIdentityMatrix() : ndRollMatrix(-90.0f * ndDegreeToRad));
	ApplyAllTransforms(meshEffectNode, convertMatrix, upAxis);
	return meshEffectNode;
}

ndMeshEffectNode* ndFbxLoader::LoadMesh(const char* const fileName)
{
	return LoadFbxMesh(fileName);
}

ndAnimationSequence* ndFbxLoader::LoadAnimation(const char* const fileName)
{
	ndSharedPtr<ndMeshEffectNode> entity(LoadFbxMesh(fileName, true));
	ndAnimationSequence* const sequence = CreateSequence(*entity, fileName);
	return sequence;
}