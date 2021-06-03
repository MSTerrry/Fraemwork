#include "ObjLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
ObjLoader::ObjLoader(ID3D11Device* dev, ID3D11DeviceContext* context) :device(dev)
{
	textureLoader = new TextureLoader(dev, context);
};
void ObjLoader::LoadObjModel(const char* fileName, ID3D11Buffer*& vertices, int& elemCount) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::material_t> materials;
	std::vector<tinyobj::shape_t> shapes;

	std::string warn;
	std::string err;

	std::string directory;
	std::string fName(fileName);
	
	const size_t last_slash_idx = fName.rfind('/');
	if (std::string::npos != last_slash_idx)
	{
		directory = fName.substr(0, last_slash_idx);
	}
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName, directory.c_str());
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << "ERR: " << err << std::endl;
	}

	if (!ret) {
		std::cout << "Failed to load/parse .obj.\n" << std::endl;
		return;
	}
	size_t index_offset = 0;
	std::vector<Vector4> verts;
	for (size_t f = 0; f < shapes[0].mesh.num_face_vertices.size(); f++) {
		size_t fv = size_t(shapes[0].mesh.num_face_vertices[f]);
		for (size_t v = 0; v < fv; v++) {
			tinyobj::index_t idx = shapes[0].mesh.indices[index_offset + v];

			tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
			tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
			tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
			verts.push_back(Vector4(vx, vy, vz, 1));
			if (idx.normal_index >= 0) {
				tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
				tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
				tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
				verts.push_back(Vector4(nx, ny, nz, 1));
			}
			else verts.push_back(Vector4(0,0,0,1));

			// Check if `texcoord_index` is zero or positive. negative = no texcoord data
			if (idx.texcoord_index >= 0) {
				tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
				tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
				verts.push_back(Vector4(tx, ty, 0, 1));
			}
			else verts.push_back(Vector4(0, 0, 0, 1));
			elemCount += 3;
		}
	}
	
	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(Vector4) * verts.size();

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = &verts[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	device->CreateBuffer(&vertexBufDesc, &vertexData, &vertices);
}
void ObjLoader::LoadTinyModel(const char* fileName, ID3D11Buffer*& vBuf, ID3D11Buffer*& nBuf, ID3D11Buffer*& tBuf,
	ID3D11Buffer*& strBuf, TinyShape*& outShapes, TinyMaterial*& outMaterials, int& shapesCount) {
	HRESULT res;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::material_t> materials;
	std::vector<tinyobj::shape_t> shapes;

	std::string warn;
	std::string err;

	std::string directory;
	std::string fName(fileName);	

	const size_t last_slash_idx = fName.rfind('/');
	if (std::string::npos != last_slash_idx)
	{
		directory = fName.substr(0, last_slash_idx);
	}
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName, directory.c_str());	
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << "ERR: " << err << std::endl;
	}

	if (!ret) {
		std::cout << "Failed to load/parse .obj.\n" << std::endl;
		return;
	}
	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	constBufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	constBufDesc.CPUAccessFlags = 0;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(float) * (float)attrib.vertices.size();

	D3D11_SUBRESOURCE_DATA resData = {};
	resData.pSysMem = &attrib.vertices[0];
	resData.SysMemPitch = 0;
	resData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&constBufDesc, &resData, &vBuf);

	if(attrib.normals.size()>0){
	constBufDesc.ByteWidth = sizeof(float) * (float)attrib.normals.size();
	resData.pSysMem = &attrib.normals[0];
	res = device->CreateBuffer(&constBufDesc, &resData, &nBuf);
	}

	constBufDesc.ByteWidth = sizeof(float) * (float)attrib.texcoords.size();
	resData.pSysMem = &attrib.texcoords[0];
	res = device->CreateBuffer(&constBufDesc, &resData, &tBuf);

	shapesCount = shapes.size();
	outShapes = new TinyShape[shapes.size()];
	std::vector<tinyobj::index_t> indexes;
	int ind = 0;

	for (auto shape : shapes) {
		outShapes[ind].StartIndex = indexes.size();
		outShapes[ind].Count = shape.mesh.indices.size();
		outShapes[ind].MaterialInd = shape.mesh.material_ids[0];
		indexes.insert(std::end(indexes), std::begin(shape.mesh.indices), std::end(shape.mesh.indices));
		++ind;
	}

	constBufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	constBufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	constBufDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufDesc.StructureByteStride = sizeof(tinyobj::index_t);
	constBufDesc.ByteWidth = sizeof(tinyobj::index_t) * indexes.size();
	resData.pSysMem = &indexes[0];
	res = device->CreateBuffer(&constBufDesc, &resData, &strBuf);
	
	outMaterials = new TinyMaterial[materials.size()];
	for (size_t i = 0; i < materials.size(); i++)
	{
		outMaterials[i].TexName = materials[i].diffuse_texname.c_str();
		if (outMaterials[i].TexName == "") continue;
		auto texFile = (directory + "/" + materials[i].diffuse_texname);
		std::wstring stemp = std::wstring(texFile.begin(), texFile.end());
		res = textureLoader->LoadTextureFromFile(stemp.c_str(), outMaterials[i].DiffuseTexture, outMaterials[i].ResourceView);
	}

}