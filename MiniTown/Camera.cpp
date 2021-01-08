#include "Camera.h"
using namespace DirectX;

Camera::~Camera()
{
}

XMFLOAT3 Camera::GetPosition() const
{
	return m_Transformation.GetPosition();
}

XMVECTOR Camera::GetPositionXM() const
{
	return m_Transformation.GetPositionXM();
}



float Camera::GetAngleX() const
{
	return m_Transformation.GetAngle().x;
}

float Camera::GetAngleY() const
{
	return m_Transformation.GetAngle().y;
}

XMFLOAT3 Camera::GetRightAxis() const
{
	return m_Transformation.GetRightAxis();
}

XMVECTOR Camera::GetRightAxisXM() const
{
	return m_Transformation.GetRightAxisXM();
}

XMFLOAT3 Camera::GetUpAxis() const
{
	return m_Transformation.GetUpAxis();
}

XMVECTOR Camera::GetUpAxisXM() const
{
	return m_Transformation.GetUpAxisXM();
}

XMFLOAT3 Camera::GetLookAxis() const
{
	return m_Transformation.GetForwardAxis();
}

XMVECTOR Camera::GetLookAxisXM() const
{
	return m_Transformation.GetForwardAxisXM();
}



XMMATRIX Camera::GetViewXM() const
{
	return m_Transformation.WorldToLocalMartixXM();
}

XMMATRIX Camera::GetProjXM() const
{
	return XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
}

XMMATRIX Camera::GetViewProjXM() const
{
	return GetViewXM() * GetProjXM();
}

D3D11_VIEWPORT Camera::GetViewPort() const
{
	return m_ViewPort;
}

void Camera::SetFrustum(float fovY, float aspect, float nearZ, float farZ)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = nearZ;
	m_FarZ = farZ;
}

void Camera::SetViewPort(const D3D11_VIEWPORT & viewPort)
{
	m_ViewPort = viewPort;
}

void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	m_ViewPort.TopLeftX = topLeftX;
	m_ViewPort.TopLeftY = topLeftY;
	m_ViewPort.Width = width;
	m_ViewPort.Height = height;
	m_ViewPort.MinDepth = minDepth;
	m_ViewPort.MaxDepth = maxDepth;
}



CamControl::~CamControl()
{
}

void CamControl::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

void CamControl::SetPosition(const XMFLOAT3& pos)
{
	m_Transformation.SetPosition(pos);
}

void CamControl::LookAt(const XMFLOAT3 & pos, const XMFLOAT3 & target,const XMFLOAT3 & up)
{
	m_Transformation.SetPosition(pos);
	m_Transformation.LookAt(target, up);
}

void CamControl::LookTo(const XMFLOAT3 & pos, const XMFLOAT3 & to, const XMFLOAT3 & up)
{
	m_Transformation.SetPosition(pos);
	m_Transformation.LookTo(to, up);
}

void CamControl::Move(float d)
{
	m_Transformation.Move(m_Transformation.GetRightAxis(), d);
}

void CamControl::Straight(float d)
{
	XMVECTOR rightVec = m_Transformation.GetRightAxisXM();
	XMVECTOR frontVec = XMVector3Normalize(XMVector3Cross(rightVec, g_XMIdentityR1));
	XMFLOAT3 front;
	XMStoreFloat3(&front, frontVec);
	m_Transformation.Move(front, d);
}

void CamControl::MoveForward(float d)
{
	m_Transformation.Move(m_Transformation.GetForwardAxis(), d);
}

void CamControl::UpSpot(float rad)
{
	XMFLOAT3 rotation = m_Transformation.GetAngle();
	rotation.x += rad;
	if (rotation.x > XM_PI * 1/2)
		rotation.x = XM_PI * 1/2;
	else if (rotation.x < -XM_PI * 1/2)
		rotation.x = -XM_PI * 1/2;			//限制仰角俯角

	m_Transformation.SetRotationAngle(rotation);
}

void CamControl::RightSpot(float rad)
{
	XMFLOAT3 rotation = m_Transformation.GetAngle();
	rotation.y = XMScalarModAngle(rotation.y + rad);
	m_Transformation.SetRotationAngle(rotation);
}




