#ifndef REALIZE_H
#define REALIZE_H

#include "Model.h"
#include "Transformation.h"

class Town_Ins
{
public:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;


	Town_Ins() = default;
	~Town_Ins() = default;

	Town_Ins(const Town_Ins&) = default;
	Town_Ins& operator=(const Town_Ins&) = default;

	Town_Ins(Town_Ins&&) = default;
	Town_Ins& operator=(Town_Ins&&) = default;

	// 获取物体变换
	Transformation& GetTransformation();
	const Transformation& GetTransformation() const;

	//
	// 获取包围盒
	//

	DirectX::BoundingBox GetLocalBoundingBox() const;
	DirectX::BoundingBox GetBoundingBox() const;
	DirectX::BoundingOrientedBox GetBoundingOrientedBox() const;

	//
	// 设置实例缓冲区
	//

	// 获取缓冲区可容纳实例的数目
	size_t GetCapacity() const;
	// 重新设置实例缓冲区可容纳实例的数目
	void ResizeBuffer(ID3D11Device * device, size_t count);
	// 获取实例缓冲区

	//
	// 设置模型
	//

	void SetModel(Model&& model);
	void SetModel(const Model& model);

	//
	// 绘制
	//

	// 绘制对象
	void Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect);
	// 绘制实例
	void DrawInstanced(ID3D11DeviceContext* deviceContext, BasicEffect& effect, const std::vector<Transformation>& data);
private:
	Model m_Model = {};											    // 模型
	Transformation m_Transformation = {};							// 物体变换

	ComPtr<ID3D11Buffer> m_pInstancedBuffer = nullptr;				// 实例缓冲区
	size_t m_Capacity = 0;										    // 缓冲区容量
};




#endif
