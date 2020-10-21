#pragma once
#include"select_text.h"
#include<vector>

class StageBorad
{
public:
	StageBorad():mPosition(0,0),mScale(1,1),mColor(1,1,1,1),mStageNo(0), mInterval(0), mSize(0,0){}
	//テキスト生成
	void CreateText(std::shared_ptr<TextureData>textureData, std::shared_ptr<LocalData>localData,const VECTOR2F&uv);
	//getter
	std::vector<std::shared_ptr<SelectText>>GetTextes() { return mTexts; }
	const int GetStageNo() { return mStageNo; }
	//setter
	void SetStageNo(const int number) { mStageNo = number; }
	void SetSize(const VECTOR2F& size) { mSize = size; }
	void SetInterval(const float interval) { mInterval = interval; }
	//更新
	void Update(int changeSelect,float elapsdTime,int nowStageNo,const VECTOR2F&centerPosition);
	void Render(ID3D11DeviceContext* context, Sprite* sprite);
private:
	std::vector<std::shared_ptr<SelectText>>mTexts;
	VECTOR2F mPosition;
	VECTOR2F mScale;
	VECTOR2F mSize;
	VECTOR4F mColor;
	int mStageNo;
	float mInterval;
};