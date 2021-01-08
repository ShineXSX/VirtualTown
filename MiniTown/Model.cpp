#include "Model.h"
#include "Tex.h"
#include "DXTrace.h"


using namespace DirectX;

Model::Model()
	: modelParts(), boundingBox(), vertexStride()
{
}

Model::Model(ID3D11Device * device, const ObjLoader & model)
	: modelParts(), boundingBox(), vertexStride()
{
	SetModel(device, model);
}

Model::Model(ID3D11Device * device, const void* vertices, UINT vertexSize, UINT vertexCount,
	const void * indices, UINT indexCount, DXGI_FORMAT indexFormat)
	: modelParts(), boundingBox(), vertexStride()
{
	SetMesh(device, vertices, vertexSize, vertexCount, indices, indexCount, indexFormat);
}

void Model::SetModel(ID3D11Device * device, const ObjLoader & model)
{
	vertexStride = sizeof(VertexPosNormalTex);

	modelParts.resize(model.objProperties.size());

	// 创建包围盒
	BoundingBox::CreateFromPoints(boundingBox, XMLoadFloat3(&model.vMin), XMLoadFloat3(&model.vMax));

	for (size_t i = 0; i < model.objProperties.size(); ++i)
	{
		auto part = model.objProperties[i];

		modelParts[i].vertexCount = (UINT)part.vertices.size();
		// 设置顶点缓冲区描述
		D3D11_BUFFER_DESC vbd;
		ZeroMemory(&vbd, sizeof(vbd));
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = modelParts[i].vertexCount * (UINT)sizeof(VertexPosNormalTex);
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		// 新建顶点缓冲区
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = part.vertices.data();
		HR(device->CreateBuffer(&vbd, &InitData, modelParts[i].vertexBuffer.ReleaseAndGetAddressOf()));

		// 设置索引缓冲区描述
		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		if (modelParts[i].vertexCount > 65535)
		{
			modelParts[i].indexCount = (UINT)part.indices32.size();
			modelParts[i].indexFormat = DXGI_FORMAT_R32_UINT;
			ibd.ByteWidth = modelParts[i].indexCount * (UINT)sizeof(DWORD);
			InitData.pSysMem = part.indices32.data();

		}
		else
		{
			modelParts[i].indexCount = (UINT)part.indices16.size();
			modelParts[i].indexFormat = DXGI_FORMAT_R16_UINT;
			ibd.ByteWidth = modelParts[i].indexCount * (UINT)sizeof(WORD);
			InitData.pSysMem = part.indices16.data();
		}
		// 新建索引缓冲区
		HR(device->CreateBuffer(&ibd, &InitData, modelParts[i].indexBuffer.ReleaseAndGetAddressOf()));

		
		// 创建漫射光对应纹理
		auto& strD = part.texStrDiffuse;
		if (strD.size() > 4)
		{
			if (strD.substr(strD.size() - 3, 3) == L"dds")
			{
				HR(CreateDDSTextureFromFile(device, strD.c_str(), nullptr,
					modelParts[i].texDiffuse.GetAddressOf()));
			}
			else
			{
				HR(CreateWICTextureFromFile(device, strD.c_str(), nullptr,
					modelParts[i].texDiffuse.GetAddressOf()));
			}
		}

		modelParts[i].material = part.material;
	}


}

void Model::SetMesh(ID3D11Device * device, const void * vertices, UINT vertexSize, UINT vertexCount, const void * indices, UINT indexCount, DXGI_FORMAT indexFormat)
{
	vertexStride = vertexSize;

	modelParts.resize(1);

	modelParts[0].vertexCount = vertexCount;
	modelParts[0].indexCount = indexCount;
	modelParts[0].indexFormat = indexFormat;

	modelParts[0].material.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	modelParts[0].material.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	modelParts[0].material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = vertexSize * vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &InitData, modelParts[0].vertexBuffer.ReleaseAndGetAddressOf()));

	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	if (indexFormat == DXGI_FORMAT_R16_UINT)
	{
		ibd.ByteWidth = indexCount * (UINT)sizeof(WORD);
	}
	else
	{
		ibd.ByteWidth = indexCount * (UINT)sizeof(DWORD);
	}
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &InitData, modelParts[0].indexBuffer.ReleaseAndGetAddressOf()));

}





