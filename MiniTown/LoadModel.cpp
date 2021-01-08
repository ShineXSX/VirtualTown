#include "LoadModel.h"

using namespace DirectX;

bool ObjLoader::LoadObj(const wchar_t* objFileName)
{
	objProperties.clear();
	vertexCache.clear();

	MtlReader mtlReader;

	std::vector<XMFLOAT3>   positions;
	std::vector<XMFLOAT3>   normals;
	std::vector<XMFLOAT2>   texCoords;

	XMVECTOR vecMin = g_XMInfinity, vecMax = g_XMNegInfinity;

	std::wifstream wfin(objFileName);
	if (!wfin.is_open())
		return false;

	for (;;)
	{
		std::wstring wstr;
		if (!(wfin >> wstr))
			break;

		//跳过注释
		if (wstr[0] == '#')		
		{
			while (!wfin.eof() && wfin.get() != '\n')
				continue;
		}
		//对象名
		else if (wstr == L"o" || wstr == L"g")
		{
			
			objProperties.emplace_back(ObjProperty());
			objProperties.back().material.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
			objProperties.back().material.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
			objProperties.back().material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertexCache.clear();
		}
		//顶点坐标
		else if (wstr == L"v")
		{
			XMFLOAT3 pos;
			wfin >> pos.x >> pos.y >> pos.z;
			pos.z = -pos.z;//反转z坐标
			positions.push_back(pos);
			XMVECTOR vecPos = XMLoadFloat3(&pos);
			vecMax = XMVectorMax(vecMax, vecPos);
			vecMin = XMVectorMin(vecMin, vecPos);
		}
		//顶点文理
		else if (wstr == L"vt")
		{
			float u, v;
			wfin >> u >> v;
			v = 1.0f - v;
			texCoords.emplace_back(XMFLOAT2(u, v));
		}
		//顶点法向量
		else if (wstr == L"vn")
		{
			float x, y, z;
			wfin >> x >> y >> z;
			z = -z;
			normals.emplace_back(XMFLOAT3(x, y, z));
		}
		//指定加载mtl
		else if (wstr == L"mtllib")
		{
			std::wstring mtlFile;
			wfin >> mtlFile;
			size_t beg = 0, ed = mtlFile.size();
			while (iswspace(mtlFile[beg]))
				beg++;
			while (ed > beg&& iswspace(mtlFile[ed - 1]))
				ed--;
			mtlFile = mtlFile.substr(beg, ed - beg);
			std::wstring dir = objFileName;
			size_t pos;
			if ((pos = dir.find_last_of('/')) == std::wstring::npos &&
				(pos = dir.find_last_of('\\')) == std::wstring::npos)
			{
				pos = 0;
			}
			else
			{
				pos += 1;
			}


			mtlReader.LoadMtl((dir.erase(pos) + mtlFile).c_str());
		}
		//指定加载mtl中的某一材质
		else if (wstr == L"usemtl")
		{
			std::wstring mtlName;
			std::getline(wfin, mtlName);
			size_t beg = 0, ed = mtlName.size();
			while (iswspace(mtlName[beg]))
				beg++;
			while (ed > beg&& iswspace(mtlName[ed - 1]))
				ed--;
			mtlName = mtlName.substr(beg, ed - beg);

			objProperties.back().material = mtlReader.materials[mtlName];
			objProperties.back().texStrDiffuse = mtlReader.mapKdStrs[mtlName];
		}
		//三角形面
		else if (wstr == L"f")
		{
			VertexPosNormalTex vertex;
			DWORD vpi[3], vni[3], vti[3];
			wchar_t ignore;
			for (int i = 2; i >= 0; --i)
			{
				wfin >> vpi[i] >> ignore >> vti[i] >> ignore >> vni[i];
			}

			for (int i = 0; i < 3; ++i)
			{
				vertex.pos = positions[vpi[i] - 1];
				vertex.normal = normals[vni[i] - 1];
				vertex.tex = texCoords[vti[i] - 1];
				AddVertex(vertex, vpi[i], vti[i], vni[i]);
			}


			while (iswblank(wfin.peek()))
				wfin.get();
			if (wfin.peek() != '\n')
			{
				return false;
			}

				
		}
	}
	for (auto& part : objProperties)
	{
		if (part.vertices.size() < 65535)
		{
			for (auto& i : part.indices32)
			{
				part.indices16.push_back((WORD)i);
			}
			part.indices32.clear();
		}
	}

	XMStoreFloat3(&vMax, vecMax);
	XMStoreFloat3(&vMin, vecMin);

	return true;
}



void ObjLoader::AddVertex(const VertexPosNormalTex& vertex, DWORD vpi, DWORD vti, DWORD vni)
{
	std::wstring idxStr = std::to_wstring(vpi) + L"/" + std::to_wstring(vti) + L"/" + std::to_wstring(vni);

	auto it = vertexCache.find(idxStr);
	if (it != vertexCache.end())
	{
		objProperties.back().indices32.push_back(it->second);
	}
	else
	{
		objProperties.back().vertices.push_back(vertex);
		DWORD pos = (DWORD)objProperties.back().vertices.size() - 1;
		vertexCache[idxStr] = pos;
		objProperties.back().indices32.push_back(pos);
	}
}



bool MtlReader::LoadMtl(const wchar_t* mtlFileName)
{
	materials.clear();
	mapKdStrs.clear();


	std::wifstream wfin(mtlFileName);
	std::locale china("chs");
	china = wfin.imbue(china);


	if (!wfin.is_open())
		return false;

	std::wstring wstr;
	std::wstring currMtl;
	for (;;)
	{
		if (!(wfin >> wstr))
			break;

		if (wstr[0] == '#')
		{
			while (wfin.get() != '\n')
				continue;
		}
		//新材质
		else if (wstr == L"newmtl")
		{
			std::getline(wfin, currMtl);
			size_t beg = 0, ed = currMtl.size();
			while (iswspace(currMtl[beg]))
				beg++;
			while (ed > beg&& iswspace(currMtl[ed - 1]))
				ed--;
			currMtl = currMtl.substr(beg, ed - beg);
		}
		//环境光
		else if (wstr == L"Ka")
		{
			XMFLOAT4& ambient = materials[currMtl].ambient;
			wfin >> ambient.x >> ambient.y >> ambient.z;
			if (ambient.w == 0.0f)
				ambient.w = 1.0f;
		}
		//漫反射光
		else if (wstr == L"Kd")
		{
			XMFLOAT4& diffuse = materials[currMtl].diffuse;
			wfin >> diffuse.x >> diffuse.y >> diffuse.z;
			if (diffuse.w == 0.0f)
				diffuse.w = 1.0f;
		}
		//镜面光
		else if (wstr == L"Ks")
		{
			XMFLOAT4& specular = materials[currMtl].specular;
			wfin >> specular.x >> specular.y >> specular.z;
		}
		//镜面光
		else if (wstr == L"Ns")
		{
			wfin >> materials[currMtl].specular.w;
		}
		//不透明/透明度
		else if (wstr == L"d" || wstr == L"Tr")
		{

			float alpha;
			wfin >> alpha;
			if (wstr == L"Tr")
				alpha = 1.0f - alpha;
			materials[currMtl].ambient.w = alpha;
			materials[currMtl].diffuse.w = alpha;
		}
		//纹理
		else if (wstr == L"map_Kd")
		{
			std::wstring fileName;
			std::getline(wfin, fileName);
			size_t beg = 0, ed = fileName.size();
			while (iswspace(fileName[beg]))
				beg++;
			while (ed > beg&& iswspace(fileName[ed - 1]))
				ed--;
			fileName = fileName.substr(beg, ed - beg);
			std::wstring dir = mtlFileName;
			size_t pos;
			if ((pos = dir.find_last_of('/')) == std::wstring::npos &&
				(pos = dir.find_last_of('\\')) == std::wstring::npos)
				pos = 0;
			else
				pos += 1;

			mapKdStrs[currMtl] = dir.erase(pos) + fileName;
		}
	}

	return true;
}

