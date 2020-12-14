#pragma once
#include"vector.h"

class Obj3D
{
public:
	Obj3D():mPosition(0,0,0), mAngle(0,0,0), mScale(1,1,1),mColor(1,1,1,1), mWorld(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), mBeforeWorld(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0){}
	void CalculateTransform()
	{
		DirectX::XMMATRIX W, s, r, t;
		//�X�P�[���s��
		s = DirectX::XMMatrixScaling(mScale.x, mScale.y, mScale.z);
		//��]�s��
		r = DirectX::XMMatrixRotationRollPitchYaw(mAngle.x, mAngle.y, mAngle.z);
		//�ړ��s��
		t = DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
		//���[���h�s��
		W = s * r * t;
		DirectX::XMStoreFloat4x4(&mWorld, W);
	}
	//setter
	void SetPosition(const VECTOR3F&position) { mPosition = position; }
	void SetAngle(const VECTOR3F&angle) { mAngle = angle; }
	void SetScale(const VECTOR3F&scale) { mScale = scale; }
	void SetColor(const VECTOR4F& color) { mColor = color; }
	//getter
	VECTOR3F& GetPosition() { return mPosition; }
	VECTOR3F& GetAngle() { return mAngle; }
	VECTOR3F& GetScale() { return mScale; }
	VECTOR4F& GetColor() { return mColor; }
	FLOAT4X4& GetWorld() { return mWorld; }
	FLOAT4X4& GetBeforeWorld() { return mBeforeWorld; }
protected:
	VECTOR3F mPosition;
	VECTOR3F mAngle;
	VECTOR3F mScale;
	VECTOR4F mColor;
	FLOAT4X4 mWorld;
	FLOAT4X4 mBeforeWorld;
};