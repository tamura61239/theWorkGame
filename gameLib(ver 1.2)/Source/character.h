#pragma once
#include<memory>
#include"obj3d.h"
#include"model_resource.h"
#include"model.h"

class Character :public Obj3D
{
public:
	Character(std::shared_ptr<ModelResource>resouce);
	void CalculateBoonTransform(float elapsd_time);
	Model* GetModel() { return mModel.get(); }
protected:
	std::unique_ptr<Model>mModel;
};