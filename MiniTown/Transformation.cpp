#include "Transformation.h"

using namespace DirectX;

Transformation::Transformation(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& r_angle, const DirectX::XMFLOAT3& position)
	: m_Scale(scale), m_Angle(r_angle), m_Position(position)
{
}

//---------------------------------------------
//三种基本变换
//---------------------------------------------

XMFLOAT3 Transformation::GetScale() const
{
	return m_Scale;
}

DirectX::XMVECTOR Transformation::GetScaleXM() const
{
	return XMLoadFloat3(&m_Scale);
}

void Transformation::SetScale(const XMFLOAT3& scale)
{
	m_Scale = scale;
}

void Transformation::SetScale(float x, float y, float z)
{
	m_Scale = XMFLOAT3(x, y, z);
}

XMFLOAT3 Transformation::GetAngle() const
{
	return m_Angle;
}

DirectX::XMVECTOR Transformation::GetAngleXM() const
{
	return XMLoadFloat3(&m_Angle);
}

void Transformation::SetRotationAngle(const XMFLOAT3& eulerAnglesInRadian)
{
	m_Angle = eulerAnglesInRadian;
}

void Transformation::SetRotationAngle(float x, float y, float z)
{
	m_Angle = XMFLOAT3(x, y, z);
}

XMFLOAT3 Transformation::GetPosition() const
{
	return m_Position;
}

DirectX::XMVECTOR Transformation::GetPositionXM() const
{
	return XMLoadFloat3(&m_Position);
}

void Transformation::SetPosition(const XMFLOAT3& position)
{
	m_Position = position;
}

void Transformation::SetPosition(float x, float y, float z)
{
	m_Position = XMFLOAT3(x, y, z);
}




XMFLOAT3 Transformation::GetRightAxis() const
{
	XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Angle));
	XMFLOAT3 right;
	XMStoreFloat3(&right, R.r[0]);
	return right;
}

DirectX::XMVECTOR Transformation::GetRightAxisXM() const
{
	XMFLOAT3 right = GetRightAxis();
	return XMLoadFloat3(&right);
}

XMFLOAT3 Transformation::GetUpAxis() const
{
	XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Angle));
	XMFLOAT3 up;
	XMStoreFloat3(&up, R.r[1]);
	return up;
}

DirectX::XMVECTOR Transformation::GetUpAxisXM() const
{
	XMFLOAT3 up = GetUpAxis();
	return XMLoadFloat3(&up);
}

XMFLOAT3 Transformation::GetForwardAxis() const
{
	XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Angle));
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, R.r[2]);
	return forward;
}

DirectX::XMVECTOR Transformation::GetForwardAxisXM() const
{
	XMFLOAT3 forward = GetForwardAxis();
	return XMLoadFloat3(&forward);
}

XMFLOAT4X4 Transformation::LocalToWorldMartix() const
{
	XMFLOAT4X4 res;
	XMStoreFloat4x4(&res, LocalToWorldMartixXM());
	return res;
}

XMMATRIX Transformation::LocalToWorldMartixXM() const
{
	XMVECTOR scaleVec = XMLoadFloat3(&m_Scale);
	XMVECTOR rotationVec = XMLoadFloat3(&m_Angle);
	XMVECTOR positionVec = XMLoadFloat3(&m_Position);
	XMMATRIX World = XMMatrixScalingFromVector(scaleVec) * XMMatrixRotationRollPitchYawFromVector(rotationVec) * XMMatrixTranslationFromVector(positionVec);
	return World;
}

XMFLOAT4X4 Transformation::WorldToLocalMartix() const
{
	XMFLOAT4X4 res;
	XMStoreFloat4x4(&res, WorldToLocalMartixXM());
	return res;
}

XMMATRIX Transformation::WorldToLocalMartixXM() const
{
	XMMATRIX InvWorld = XMMatrixInverse(nullptr, LocalToWorldMartixXM());
	return InvWorld;
}


void Transformation::Rotate(const XMFLOAT3& eulerAnglesInRadian)
{
	XMVECTOR newRotationVec = XMVectorAdd(XMLoadFloat3(&m_Angle), XMLoadFloat3(&eulerAnglesInRadian));
	XMStoreFloat3(&m_Angle, newRotationVec);
}

void Transformation::Move(const XMFLOAT3& direction, float magnitude)
{
	XMVECTOR directionVec = XMVector3Normalize(XMLoadFloat3(&direction));
	XMVECTOR newPosition = XMVectorMultiplyAdd(XMVectorReplicate(magnitude), directionVec, XMLoadFloat3(&m_Position));
	XMStoreFloat3(&m_Position, newPosition);
}

void Transformation::LookAt(const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMMATRIX View = XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&target), XMLoadFloat3(&up));
	XMMATRIX InvView = XMMatrixInverse(nullptr, View);
	XMFLOAT4X4 rotMatrix;
	XMStoreFloat4x4(&rotMatrix, InvView);
	m_Angle = GetAnglesInMatrix(rotMatrix);
}

void Transformation::LookTo(const XMFLOAT3& direction, const XMFLOAT3& up)
{
	XMMATRIX View = XMMatrixLookToLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&direction), XMLoadFloat3(&up));
	XMMATRIX InvView = XMMatrixInverse(nullptr, View);
	XMFLOAT4X4 rotMatrix;
	XMStoreFloat4x4(&rotMatrix, InvView);
	m_Angle = GetAnglesInMatrix(rotMatrix);
}

XMFLOAT3 Transformation::GetAnglesInMatrix(const XMFLOAT4X4& rotationMatrix)
{
	float c = sqrtf(1.0f - rotationMatrix(2, 1) * rotationMatrix(2, 1));
	if (isnan(c))
		c = 0.0f;
	XMFLOAT3 r_angle;
	r_angle.z = atan2f(rotationMatrix(0, 1), rotationMatrix(1, 1));
	r_angle.x = atan2f(-rotationMatrix(2, 1), c);
	r_angle.y = atan2f(rotationMatrix(2, 0), rotationMatrix(2, 2));
	return r_angle;
}
