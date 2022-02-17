/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "OGLMesh.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"

using namespace NCL;
using namespace Rendering;
using namespace Maths;

OGLMesh::OGLMesh()
{
	vao = 0;
	subCount = 1;

	for (int i = 0; i < MAX_ATTRIBUTES; ++i)
	{
		attributeBuffers[i] = 0;
	}
	indexBuffer = 0;
}

OGLMesh::OGLMesh(const std::string& filename) : MeshGeometry(filename)
{
	vao = 0;
	subCount = 1;

	for (int i = 0; i < MAX_ATTRIBUTES; ++i)
	{
		attributeBuffers[i] = 0;
	}
	indexBuffer = 0;
}

OGLMesh::~OGLMesh()
{
	glDeleteVertexArrays(1, &vao); //Delete our VAO
	glDeleteBuffers(MAX_ATTRIBUTES, attributeBuffers); //Delete our VBOs
	glDeleteBuffers(1, &indexBuffer); //Delete our indices
}

void CreateVertexBuffer(GLuint& buffer, int byteCount, char* data)
{
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, byteCount, data, GL_STATIC_DRAW);
}

void OGLMesh::BindVertexAttribute(int attribSlot, int buffer, int bindingID, int elementCount, int elementSize,
                                  int elementOffset)
{
	glEnableVertexAttribArray(attribSlot);
	glVertexAttribFormat(attribSlot, elementCount, GL_FLOAT, false, 0);
	glVertexAttribBinding(attribSlot, bindingID);

	glBindVertexBuffer(bindingID, buffer, elementOffset, elementSize);
}

void OGLMesh::UploadToGPU(RendererBase* renderer)
{
	if (!ValidateMeshData())
	{
		return;
	}
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	int numVertices = GetVertexCount();
	int numIndices = GetIndexCount();

	if (!GetPositionData().empty())
	{
		CreateVertexBuffer(attributeBuffers[Positions], numVertices * sizeof(Vector3), (char*)GetPositionData().data());
		BindVertexAttribute(Positions, attributeBuffers[Positions], Positions, 3, sizeof(Vector3), 0);
	}

	if (!GetColourData().empty())
	{
		//buffer colour data
		CreateVertexBuffer(attributeBuffers[Colours], numVertices * sizeof(Vector4), (char*)GetColourData().data());
		BindVertexAttribute(Colours, attributeBuffers[Colours], Colours, 4, sizeof(Vector4), 0);
	}
	if (!GetTextureCoordData().empty())
	{
		//Buffer texture data
		CreateVertexBuffer(attributeBuffers[TextureCoords], numVertices * sizeof(Vector2),
		                   (char*)GetTextureCoordData().data());
		BindVertexAttribute(TextureCoords, attributeBuffers[TextureCoords], TextureCoords, 2, sizeof(Vector2), 0);
	}

	if (!GetNormalData().empty())
	{
		//Buffer normal data
		CreateVertexBuffer(attributeBuffers[Normals], numVertices * sizeof(Vector3), (char*)GetNormalData().data());
		BindVertexAttribute(Normals, attributeBuffers[Normals], Normals, 3, sizeof(Vector3), 0);
	}

	if (!GetTangentData().empty())
	{
		//Buffer tangent data
		CreateVertexBuffer(attributeBuffers[Tangents], numVertices * sizeof(Vector4), (char*)GetTangentData().data());
		BindVertexAttribute(Tangents, attributeBuffers[Tangents], Tangents, 4, sizeof(Vector4), 0);
	}

	if (!GetSkinWeightData().empty())
	{
		//Skeleton weights
		CreateVertexBuffer(attributeBuffers[JointWeights], numVertices * sizeof(Vector4),
		                   (char*)GetSkinWeightData().data());
		BindVertexAttribute(JointWeights, attributeBuffers[JointWeights], JointWeights, 4, sizeof(Vector4), 0);
	}

	if (!GetSkinIndexData().empty())
	{
		//Skeleton joint indices
		CreateVertexBuffer(attributeBuffers[JointIndices], numVertices * sizeof(Vector4),
		                   (char*)GetSkinIndexData().data());
		BindVertexAttribute(JointIndices, attributeBuffers[JointIndices], JointIndices, 4, sizeof(Vector4), 0);
	}

	if (!GetIndexData().empty())
	{
		//buffer index data
		glGenBuffers(1, &attributeBuffers[MAX_ATTRIBUTES]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attributeBuffers[MAX_ATTRIBUTES]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), (int*)GetIndexData().data(), GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

void OGLMesh::UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount)
{
	if (!GetPositionData().empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[Positions]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector3), vertexCount * sizeof(Vector3),
		                (char*)&GetPositionData()[startVertex]);
	}

	if (!GetColourData().empty())
	{
		//buffer colour data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[Colours]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4),
		                (char*)&GetColourData()[startVertex]);
	}
	if (!GetTextureCoordData().empty())
	{
		//Buffer texture data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[TextureCoords]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector2), vertexCount * sizeof(Vector2),
		                (char*)&GetTextureCoordData()[startVertex]);
	}

	if (!GetNormalData().empty())
	{
		//Buffer normal data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[Normals]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector3), vertexCount * sizeof(Vector3),
		                (char*)&GetNormalData()[startVertex]);
	}

	if (!GetTangentData().empty())
	{
		//Buffer tangent data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[Tangents]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4),
		                (char*)&GetTangentData()[startVertex]);
	}

	//if (!GetSkinWeightData().empty()) {	//Skeleton weights
	//	glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::JointWeights]);
	//	glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4), (char*)&GetJoint()[startVertex]);
	//}

	//if (!GetSkinIndexData().empty()) {	//Skeleton joint indices
	//	glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::TextureCoords]);
	//	glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector2), vertexCount * sizeof(Vector2), (char*)&GetTextureCoordData()[startVertex]);
	//}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void OGLMesh::RecalculateNormals()
{
	normals.clear();

	if (indices.size() > 0)
	{
		for (size_t i = 0; i < positions.size(); i++)
		{
			normals.emplace_back(Vector3());
		}

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			Vector3& a = positions[indices[i + 0]];
			Vector3& b = positions[indices[i + 1]];
			Vector3& c = positions[indices[i + 2]];

			Vector3 normal = Vector3::Cross(b - a, c - a);
			normal.Normalise();

			normals[indices[i + 0]] += normal;
			normals[indices[i + 1]] += normal;
			normals[indices[i + 2]] += normal;
		}
		for (size_t i = 0; i < normals.size(); ++i)
		{
			normals[i].Normalise();
		}
	}
	else
	{
	}
}
