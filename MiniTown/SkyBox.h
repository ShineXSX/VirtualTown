#ifndef SKYRENDER_H
#define SKYRENDER_H

#include <vector>
#include <string>
#include "Effects.h"
#include "Camera.h"

class SkyBox
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	SkyBox() = default;
	~SkyBox() = default;
	SkyBox(const SkyBox&) = delete;
	SkyBox& operator=(const SkyBox&) = delete;
	SkyBox(SkyBox&&) = default;
	SkyBox& operator=(SkyBox&&) = default;
	// 需要提供天空盒的六张正方形贴图
	HRESULT InitSkyBox(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& cubemapFilename, float skySphereRadius, bool generateMips = false);
	ID3D11ShaderResourceView* GetTextureCube();
	void Draw(ID3D11DeviceContext* deviceContext, SkyEffect& skyEffect, const Camera& camera);

private:
	HRESULT InitResource(ID3D11Device* device, float skySphereRadius);

private:
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;

	UINT m_IndexCount = 0;

	ComPtr<ID3D11ShaderResourceView> m_pTextureCubeSRV;
};

#endif