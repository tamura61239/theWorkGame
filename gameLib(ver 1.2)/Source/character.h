#pragma once
#include<memory>
#include"obj3d.h"
#include"model_resource.h"
#include"model.h"
//アニメーションするオブジェクトのクラス
class Character :public Obj3D
{
public:
	//コンストラクタ
	Character(std::shared_ptr<ModelResource>resouce);
	//ワールド行列やボーン行列の計算をする
	void CalculateBoonTransform(float elapsd_time);
	//getter
	Model* GetModel() { return mModel.get(); }
protected:
	//アニメーションするモデル
	std::unique_ptr<Model>mModel;
};