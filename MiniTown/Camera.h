
#ifndef CAMERA_H
#define CAMERA_H

#include <d3d11_1.h>
#include <DirectXMath.h>
#include "Transformation.h"


class Camera
{
public:
	Camera() = default;
	virtual ~Camera() = 0;

	//镜头位置
	DirectX::XMVECTOR GetPositionXM() const;
	DirectX::XMFLOAT3 GetPosition() const;

	//-----------------------------------
	//镜头旋转
	//-----------------------------------

	//读取角度
	float GetAngleX() const;
	float GetAngleY() const;

	
	//获取坐标向量
	DirectX::XMFLOAT3 GetRightAxis() const;
	DirectX::XMVECTOR GetRightAxisXM() const;
	
	DirectX::XMFLOAT3 GetUpAxis() const;
	DirectX::XMVECTOR GetUpAxisXM() const;
	
	DirectX::XMFLOAT3 GetLookAxis() const;
	DirectX::XMVECTOR GetLookAxisXM() const;
	

	//获取矩阵
	DirectX::XMMATRIX GetViewXM() const;
	DirectX::XMMATRIX GetProjXM() const;
	DirectX::XMMATRIX GetViewProjXM() const;

	// 获取视口
	D3D11_VIEWPORT GetViewPort() const;


	// 设置视锥体
	void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

	// 设置视口
	void SetViewPort(const D3D11_VIEWPORT& viewPort);
	void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

protected:

	// 摄像机的变换
	Transformation m_Transformation = {};
	
	// 视锥体属性
	float m_NearZ = 0.0f;
	float m_FarZ = 0.0f;
	float m_Aspect = 0.0f;
	float m_FovY = 0.0f;

	// 当前视口
	D3D11_VIEWPORT m_ViewPort = {};

};

class CamControl : public Camera
{
public:
	CamControl() = default;
	~CamControl() override;

	//镜头位置
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& pos);
	//镜头方向
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target,const DirectX::XMFLOAT3& up);
	void LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up);
	//平移镜头
	void Move(float d);
	//直线行动镜头
	void Straight(float d);
	// 前进(朝前向移动)
	void MoveForward(float d);
	//向上旋转镜头(负值向下)
	void UpSpot(float rad);
	//向右旋转镜头(负值向左)
	void RightSpot(float rad);
};


#endif
