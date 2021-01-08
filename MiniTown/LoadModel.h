
#ifndef LOADMODEL_H
#define LOADMODEL_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <string>
#include <algorithm>
#include <locale>
#include "Vertex.h"
#include "LightHelper.h"


class MtlReader;

class ObjLoader
{
public:
	struct ObjProperty
	{
		ObjProperty() : material() {}
		~ObjProperty() = default;

		Material material;							//材质
		std::vector<VertexPosNormalTex> vertices;	//顶点集合
		std::vector<WORD> indices16;				//顶点数不超过65535时使用
		std::vector<DWORD> indices32;				//顶点数超过65535时使用
		std::wstring texStrDiffuse;					//漫反射纹理
	};

	ObjLoader() : vMin(), vMax() {}
	~ObjLoader() = default;

	bool LoadObj(const wchar_t* objFileName);
public:
	std::vector<ObjProperty> objProperties;
	DirectX::XMFLOAT3 vMin, vMax;					
private:
	void AddVertex(const VertexPosNormalTex& vertex, DWORD vpi, DWORD vti, DWORD vni);

	// 缓存有v/vt/vn字符串信息
	std::unordered_map<std::wstring, DWORD> vertexCache;
};

class MtlReader
{
public:
	bool LoadMtl(const wchar_t* mtlFileName);


public:
	std::map<std::wstring, Material> materials;
	std::map<std::wstring, std::wstring> mapKdStrs;
};


#endif

