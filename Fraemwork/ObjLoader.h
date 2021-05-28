#pragma once
#include "tiny_obj_loader.h"
#include <iostream>
#include <d3d11.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;
class ObjLoader {
	ID3D11Device* device = nullptr;
public:
	void LoadTinyModel(const char* fileName, ID3D11Buffer* vBuf, ID3D11Buffer* nBuf, ID3D11Buffer* tBuf,
		ID3D11Buffer* strBuf, std::vector<tinyobj::material_t> materials, std::vector<tinyobj::shape_t> shapes, UINT elemCount) {
		tinyobj::attrib_t attrib;
		std::string warn;
		std::string err;
		std::vector<Vector4> vertices;
		std::vector<Vector3> normals;
		std::vector<Vector2> texCoord;
		std::vector<int> vertPosIndex;
		std::vector<int> vertNormIndex;
		std::vector<int> vertTCIndex;


		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName, NULL, true);

		if (!warn.empty()) {
			std::cout << "WARN: " << warn << std::endl;
		}

		if (!err.empty()) {
			std::cerr << "ERR: " << err << std::endl;
		}

		if (!ret) {
			printf("Failed to load/parse .obj.\n");
			return;
		}
		for (size_t s = 0; s < shapes.size(); s++) {
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
				
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					float vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					float vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					float vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
					vertices.push_back(Vector4((vx, vy, vz, 1)));
					// Check if `normal_index` is zero or positive. negative = no normal data
					if (idx.normal_index >= 0) {
						float nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
						float ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
						float nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
						normals.push_back(Vector3(nx, ny, nz));
					}

					// Check if `texcoord_index` is zero or positive. negative = no texcoord data
					if (idx.texcoord_index >= 0) {
						float tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
						float ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
						texCoord.push_back(Vector2(tx, ty));
					}
					
					// Optional: vertex colors
					// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
				}
				index_offset += fv;
				shapes[s].mesh.material_ids[f];
			}


			/*D3D11_BUFFER_DESC vertexBufDesc = {};
			vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufDesc.CPUAccessFlags = 0;
			vertexBufDesc.MiscFlags = 0;
			vertexBufDesc.StructureByteStride = 0;
			vertexBufDesc.ByteWidth = sizeof(Vector4) * 10;

			D3D11_SUBRESOURCE_DATA vertexData = {};
			vertexData.pSysMem = points;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			device->CreateBuffer(&vertexBufDesc, &vertexData, &vBuf);*/

		}
	}
};