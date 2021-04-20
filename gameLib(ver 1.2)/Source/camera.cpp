#include "camera.h"
#include<d3d11.h>
#include"misc.h"

/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
Camera::Camera(ID3D11Device*device):mEye(0,0,-200.0f),mFocus(0,0,0),mUp(0,1,0),mFront(0,0,1),mRight(1,0,0),mView(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),mFov(0),mAspect(0),mNearZ(0),mFarZ(0), mWidth(0),mHight(0)
{
	{
		mBeforeFrame = std::make_unique<ConstantBuffer<Cb>>(device);
	}
}
/*****************************************************/
//　　　　　　　　　　view projection行列計算関数
/*****************************************************/

/***********************透視投影****************************/
void Camera::CalculateMatrix()
{
	mBeforeFrame->data.view = mView;
	mBeforeFrame->data.projection = mProjection;
	//ビュー行列作成
	DirectX::XMMATRIX V;
	
	DirectX::XMVECTOR eye, focus, up;
	eye = DirectX::XMVectorSet(mEye.x, mEye.y, mEye.z, 1.0f);//視点
	focus = DirectX::XMVectorSet(mFocus.x, mFocus.y, mFocus.z, 1.0f);//注視点
	up = DirectX::XMVectorSet(mUp.x, mUp.y, mUp.z, 1.0f);//上ベクトル

	V = DirectX::XMMatrixLookAtLH(eye, focus, up);

	DirectX::XMStoreFloat4x4(&mView, V);
	mFront = VECTOR3F(mView._31, mView._32, mView._33);
	//プロジェクション行列作成
	DirectX::XMMATRIX P;

	P = DirectX::XMMatrixPerspectiveFovLH(mFov, mAspect, mNearZ, mFarZ);

	DirectX::XMStoreFloat4x4(&mProjection, P);

}
/***********************平行投影****************************/

void Camera::CalculateParallelMatrix()
{
	//ビュー行列作成
	DirectX::XMMATRIX V;

	DirectX::XMVECTOR eye, focus, up;
	eye = DirectX::XMVectorSet(mEye.x, mEye.y, mEye.z, 1.0f);//視点
	focus = DirectX::XMVectorSet(mFocus.x, mFocus.y, mFocus.z, 1.0f);//注視点
	up = DirectX::XMVectorSet(mUp.x, mUp.y, mUp.z, 1.0f);//上ベクトル

	V = DirectX::XMMatrixLookAtLH(eye, focus, up);

	DirectX::XMStoreFloat4x4(&mView, V);
	//プロジェクション行列作成
	DirectX::XMMATRIX P;

	P = DirectX::XMMatrixOrthographicLH(mWidth, mHight, mNearZ, mFarZ);

	DirectX::XMStoreFloat4x4(&mProjection, P);

}

