#include "character.h"
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

Character::Character(std::shared_ptr<ModelResource> resouce):Obj3D()
{
	mModel = std::make_unique<Model>(resouce);
}
/*****************************************************/
//　　　　　　　　　　行列計算関数
/*****************************************************/

void Character::CalculateBoonTransform(float elapsd_time)
{
	mModel->UpdateAnimation(elapsd_time);
	CalculateTransform();
	mModel->CalculateLocalTransform();
	mModel->CalculateWorldTransform(DirectX::XMLoadFloat4x4(&mWorld));
}
