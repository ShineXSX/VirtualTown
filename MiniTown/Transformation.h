#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <DirectXMath.h>

class Transformation
{
public:
	Transformation() = default;
	Transformation(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& position);
	~Transformation() = default;

	Transformation(const Transformation&) = default;
	Transformation& operator=(const Transformation&) = default;

	Transformation(Transformation&&) = default;
	Transformation& operator=(Transformation&&) = default;

	//得到大小比例
	DirectX::XMFLOAT3 GetScale() const;
	DirectX::XMVECTOR GetScaleXM() const;

	//设定大小比例
	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetScale(float x, float y, float z);

	//得到弧度角
	DirectX::XMFLOAT3 GetAngle() const;
	DirectX::XMVECTOR GetAngleXM() const;

	//设定旋转弧度角
	void SetRotationAngle(const DirectX::XMFLOAT3& eulerAnglesInRadian);
	void SetRotationAngle(float x, float y, float z);

	//获得位置
	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMVECTOR GetPositionXM() const;
	
	//设定位置
	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetPosition(float x, float y, float z);

	//坐标轴使用左手定则

	//x轴(右)
	DirectX::XMFLOAT3 GetRightAxis() const;
	DirectX::XMVECTOR GetRightAxisXM() const;

	//y轴(上)
	DirectX::XMFLOAT3 GetUpAxis() const;
	DirectX::XMVECTOR GetUpAxisXM() const;

	//z轴(纸面内)
	DirectX::XMFLOAT3 GetForwardAxis() const;
	DirectX::XMVECTOR GetForwardAxisXM() const;

	//局部空间变换世界空间的变换矩阵
	DirectX::XMFLOAT4X4 LocalToWorldMartix() const;
	DirectX::XMMATRIX LocalToWorldMartixXM() const;

	//世界空间变换局部空间的变换矩阵
	DirectX::XMFLOAT4X4 WorldToLocalMartix() const;
	DirectX::XMMATRIX WorldToLocalMartixXM() const;

	
	//旋转
	void Rotate(const DirectX::XMFLOAT3& eulerAnglesInRadian);

	//平移
	void Move(const DirectX::XMFLOAT3& direction, float magnitude);

	//镜头专用
	//观察某一点
	void LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up = { 0.0f, 1.0f, 0.0f });
	//沿着某一方向观察
	void LookTo(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& up = { 0.0f, 1.0f, 0.0f });

private:
	//获取旋转角
	DirectX::XMFLOAT3 GetAnglesInMatrix(const DirectX::XMFLOAT4X4& rotationMatrix);

private:
	DirectX::XMFLOAT3 m_Scale = { 1.0f, 1.0f, 1.0f };				// 缩放
	DirectX::XMFLOAT3 m_Angle = {};									// 旋转角
	DirectX::XMFLOAT3 m_Position = {};								// 位置
};

#endif


