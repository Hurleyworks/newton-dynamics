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
#include "ndDemoCamera.h"
#include "ndTargaToOpenGl.h"
#include "ndDemoDebrisEntity.h"
#include "ndDemoEntityManager.h"


ndDemoDebrisMesh::ndDemoDebrisMesh(ndDemoDebrisMesh* const srcMeshconst, const dArray<DebrisPoint>& vertexArray)
	:ndDemoMesh("vertexBufferMesh")
{
	m_shader = srcMeshconst->m_shader;
	m_indexCount = 0;
	m_vertexCount = vertexArray.GetCount();
	m_textureLocation = srcMeshconst->m_textureLocation;
	m_transparencyLocation = srcMeshconst->m_transparencyLocation;
	m_normalMatrixLocation = srcMeshconst->m_normalMatrixLocation;
	m_projectMatrixLocation = srcMeshconst->m_projectMatrixLocation;
	m_viewModelMatrixLocation = srcMeshconst->m_viewModelMatrixLocation;
	m_directionalLightDirLocation = srcMeshconst->m_directionalLightDirLocation;

	m_materialAmbientLocation = srcMeshconst->m_materialAmbientLocation;
	m_materialDiffuseLocation = srcMeshconst->m_materialDiffuseLocation;
	m_materialSpecularLocation = srcMeshconst->m_materialSpecularLocation;

	m_textureLocation1 = srcMeshconst->m_textureLocation1;
	memcpy(m_material, srcMeshconst->m_material, sizeof(m_material));

	glGenVertexArrays(1, &m_vertextArrayBuffer);
	glBindVertexArray(m_vertextArrayBuffer);
	
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(DebrisPoint), &vertexArray[0].m_posit.m_x, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(DebrisPoint), (void*)0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebrisPoint), (void*)sizeof(ndMeshVector4));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DebrisPoint), (void*)(sizeof(ndMeshVector4) + sizeof(ndMeshVector)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

ndDemoDebrisMesh::ndDemoDebrisMesh(const char* const name, ndMeshEffect* const meshNode, const ndShaderPrograms& shaderCache, dInt32 offsetBase, dArray<DebrisPoint>& vertexArray)
	:ndDemoMesh(name)
{
	m_name = name;
	m_shader = shaderCache.m_diffuseDebrisEffect;

	// extract the materials index array for mesh
	ndIndexArray* const geometryHandle = meshNode->MaterialGeometryBegin();

	// extract vertex data  from the newton mesh		
	dInt32 indexCount = 0;
	for (dInt32 handle = meshNode->GetFirstMaterial(geometryHandle); handle != -1; handle = meshNode->GetNextMaterial(geometryHandle, handle))
	{
		indexCount += meshNode->GetMaterialIndexCount(geometryHandle, handle);
	}

	dInt32* const indices = dAlloca(dInt32, indexCount);

	dInt32 segmentStart = 0;
	dInt32 materialCount = 0;
	const dArray<ndMeshEffect::dMaterial>& materialArray = meshNode->GetMaterials();
	for (dInt32 handle = meshNode->GetFirstMaterial(geometryHandle); handle != -1; handle = meshNode->GetNextMaterial(geometryHandle, handle))
	{
		dInt32 materialIndex = meshNode->GetMaterialID(geometryHandle, handle);
		const ndMeshEffect::dMaterial& material = materialArray[materialIndex];

		m_material[materialCount].m_ambient = material.m_ambient;
		m_material[materialCount].m_diffuse = material.m_diffuse;
		m_material[materialCount].m_specular = material.m_specular;
		m_material[materialCount].m_opacity = material.m_opacity;
		m_material[materialCount].m_shiness = material.m_shiness;
		strcpy(m_material[materialCount].m_textureName, material.m_textureName);
		m_material[materialCount].m_textureHandle = LoadTexture(material.m_textureName);

		dInt32 subIndexCount = meshNode->GetMaterialIndexCount(geometryHandle, handle);
		meshNode->GetMaterialGetIndexStream(geometryHandle, handle, &indices[segmentStart]);

		dFloat32 blend = materialCount ? 0.0f : 1.0f;
		for (dInt32 i = 0; i < subIndexCount; i++)
		{
			dInt32 index = indices[segmentStart + i] + offsetBase;
			indices[segmentStart + i] = index;
			vertexArray[index].m_posit.m_w = blend;
		}

		materialCount++;
		segmentStart += subIndexCount;
	}
	meshNode->MaterialGeomteryEnd(geometryHandle);


	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glUseProgram(m_shader);
	m_textureLocation = glGetUniformLocation(m_shader, "texture0");
	m_textureLocation1 = glGetUniformLocation(m_shader, "texture1");
	m_transparencyLocation = glGetUniformLocation(m_shader, "transparency");
	m_normalMatrixLocation = glGetUniformLocation(m_shader, "normalMatrix");
	m_projectMatrixLocation = glGetUniformLocation(m_shader, "projectionMatrix");
	m_viewModelMatrixLocation = glGetUniformLocation(m_shader, "viewModelMatrix");
	m_directionalLightDirLocation = glGetUniformLocation(m_shader, "directionalLightDir");
	
	m_materialAmbientLocation = glGetUniformLocation(m_shader, "material_ambient");
	m_materialDiffuseLocation = glGetUniformLocation(m_shader, "material_diffuse");
	m_materialSpecularLocation = glGetUniformLocation(m_shader, "material_specular");
	
	glUseProgram(0);

	m_indexCount = indexCount;
}

void ndDemoDebrisMesh::Render(ndDemoEntityManager* const scene, const dMatrix& modelMatrix)
{
	ndDemoCamera* const camera = scene->GetCamera();

	const dMatrix& viewMatrix = camera->GetViewMatrix();
	dMatrix viewModelMatrix(modelMatrix * viewMatrix);

	glUniformMatrix4fv(m_normalMatrixLocation, 1, false, &viewModelMatrix[0][0]);
	glUniformMatrix4fv(m_viewModelMatrixLocation, 1, false, &viewModelMatrix[0][0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
}

ndDemoDebrisEntity::ndDemoDebrisEntity(ndMeshEffect* const meshNode, dArray<DebrisPoint>& vertexArray, ndDemoDebrisRootEntity* const parent, const ndShaderPrograms& shaderCache)
	:ndDemoEntity(dGetIdentityMatrix(), parent)
{
	dInt32 vertexCount = meshNode->GetPropertiesCount();

	dInt32 const vertexOffsetBase = vertexArray.GetCount();
	vertexArray.SetCount(vertexOffsetBase + vertexCount);
	meshNode->GetVertexChannel(sizeof(DebrisPoint), &vertexArray[vertexOffsetBase].m_posit.m_x);
	meshNode->GetNormalChannel(sizeof(DebrisPoint), &vertexArray[vertexOffsetBase].m_normal.m_x);
	meshNode->GetUV0Channel(sizeof(DebrisPoint), &vertexArray[vertexOffsetBase].m_uv.m_u);

	ndDemoDebrisMesh* const mesh = new ndDemoDebrisMesh("fracture", meshNode, shaderCache, vertexOffsetBase, vertexArray);
	SetMesh(mesh, dGetIdentityMatrix());
	mesh->Release();
}

ndDemoDebrisEntity::ndDemoDebrisEntity(const ndDemoDebrisEntity& copyFrom)
	:ndDemoEntity(copyFrom)
{
}

ndDemoDebrisEntity::~ndDemoDebrisEntity()
{
}

dNodeBaseHierarchy* ndDemoDebrisEntity::CreateClone() const
{
	return new ndDemoDebrisEntity(*this);
}

void ndDemoDebrisEntity::Render(dFloat32 timeStep, ndDemoEntityManager* const scene, const dMatrix& matrix) const
{
	const dMatrix modelMatrix(m_matrix * matrix);

	m_mesh->Render(scene, modelMatrix);
}

ndDemoDebrisRootEntity::ndDemoDebrisRootEntity()
	:ndDemoEntity(dGetIdentityMatrix(), nullptr)
{
}

ndDemoDebrisRootEntity::ndDemoDebrisRootEntity(const ndDemoDebrisRootEntity& copyFrom)
	:ndDemoEntity(copyFrom)
{
}

ndDemoDebrisRootEntity::~ndDemoDebrisRootEntity(void)
{
}

void ndDemoDebrisRootEntity::FinalizeConstruction(const dArray<DebrisPoint>& vertexArray)
{
	ndDemoDebrisMesh* const shaderMesh = (ndDemoDebrisMesh*)GetChild()->GetMesh();
	m_mesh = new ndDemoDebrisMesh(shaderMesh, vertexArray);
}

void ndDemoDebrisRootEntity::Render(dFloat32 timestep, ndDemoEntityManager* const scene, const dMatrix& matrix) const
{
	ndDemoDebrisMesh* const shaderMesh = ((ndDemoDebrisMesh*)m_mesh);
	glUseProgram(shaderMesh->m_shader);
	glBindVertexArray(shaderMesh->m_vertextArrayBuffer);

	ndDemoCamera* const camera = scene->GetCamera();
	const dMatrix& viewMatrix = camera->GetViewMatrix();
	const dMatrix& projectionMatrix = camera->GetProjectionMatrix();
	const dVector directionaLight(viewMatrix.RotateVector(dVector(-1.0f, 1.0f, 0.0f, 0.0f)).Normalize());

	glUniform1i(shaderMesh->m_textureLocation, 0);
	glUniform1i(shaderMesh->m_textureLocation1, 1);
	glUniform1f(shaderMesh->m_transparencyLocation, 1.0f);
	glUniform4fv(shaderMesh->m_directionalLightDirLocation, 1, &directionaLight.m_x);
	glUniformMatrix4fv(shaderMesh->m_projectMatrixLocation, 1, false, &projectionMatrix[0][0]);

	glUniform3fv(shaderMesh->m_materialAmbientLocation, 1, &shaderMesh->m_material[0].m_ambient.m_x);
	glUniform3fv(shaderMesh->m_materialDiffuseLocation, 1, &shaderMesh->m_material[0].m_diffuse.m_x);
	glUniform3fv(shaderMesh->m_materialSpecularLocation, 1, &shaderMesh->m_material[0].m_specular.m_x);

	// these call make the font display wrong
	glActiveTexture(GL_TEXTURE1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, shaderMesh->m_material[1].m_textureHandle);

	glActiveTexture(GL_TEXTURE0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, shaderMesh->m_material[0].m_textureHandle);
	
	const dMatrix nodeMatrix(m_matrix * matrix);
	for (ndDemoEntity* child = GetChild(); child; child = child->GetSibling())
	{
		child->Render(timestep, scene, nodeMatrix);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}
