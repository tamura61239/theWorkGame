#include "character.h"

Character::Character(std::shared_ptr<ModelResource> resouce):Obj3D()
{
	mModel = std::make_unique<Model>(resouce);
}

void Character::CalculateBoonTransform(float elapsd_time)
{
	mModel->UpdateAnimation(elapsd_time);
	CalculateTransform();
	mModel->CalculateLocalTransform();
	mModel->CalculateWorldTransform(DirectX::XMLoadFloat4x4(&mWorld));
}
