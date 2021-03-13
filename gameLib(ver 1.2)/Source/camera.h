#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include"constant_buffer.h"
#include<memory>

class Camera
{
public:
	Camera(ID3D11Device* device);
	void CalculateMatrix();
	void CalculateParallelMatrix();
	void BeforeActive(ID3D11DeviceContext* context, const int number, const bool vs = false, const bool ps = false, const bool gs = false) { mBeforeFrame->Activate(context, number, vs, ps, gs); }
	void BeforeDactive(ID3D11DeviceContext* context) { mBeforeFrame->DeActivate(context); }
	//setter
	void SetPerspective(float fov, float aspect, float nearZ, float farZ)
	{
		mFov = fov;
		mAspect = aspect;
		mNearZ = nearZ;
		mFarZ = farZ;
	}
	void SetParallelPerspective(float width, float hight, float nearZ, float farZ)
	{
		mWidth = width;
		mHight = hight;
		mNearZ = nearZ;
		mFarZ = farZ;
	}

	void SetEye(const VECTOR3F& eye) { mEye = eye; }
	void SetFocus(const VECTOR3F& focus) { mFocus = focus; }
	void SetUp(const VECTOR3F& up) { mUp = up; }
	//getter
	const VECTOR3F& GetEye() { return mEye; }
	const VECTOR3F& GetFocus() { return mFocus; }
	const VECTOR3F& GetUp() { return mUp; }
	const VECTOR3F& GetFront() { return mFront; }
	const VECTOR3F& GetRight() { return mRight; }
	const FLOAT4X4& GetView() { return mView; }
	const FLOAT4X4& GetProjection() { return mProjection; }
	const float GetFov() { return mFov; }
	const float GetAspect() { return mAspect; }
	const float GetNear() { return mNearZ; }
	const float GetFar() { return mFarZ; }
private:
	struct Cb
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	FLOAT4X4 mView;//ビュー行列
	FLOAT4X4 mProjection;//プロジェクション行列
	VECTOR3F mEye;// 視点 
	VECTOR3F mFocus;//注視点
	VECTOR3F mUp;//上ベクトル
	VECTOR3F mFront;//前ベクトル
	VECTOR3F mRight;//右ベクトル
	float mFov;//画角
	float mWidth;//横幅
	float mHight;//縦幅
	float mAspect;//アスペクト比
	float mNearZ; // 表示最近面までの距離
	float mFarZ;//表紙最遠面までの距離

	std::unique_ptr<ConstantBuffer<Cb>>mBeforeFrame;
};