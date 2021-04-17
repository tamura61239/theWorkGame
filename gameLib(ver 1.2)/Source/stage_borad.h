#pragma once
#include"select_text.h"
#include<vector>

//ステージのボードクラス
class StageBorad
{
public:
	//コンストラクタ
	StageBorad():mPosition(0,0),mScale(1,1),mColor(1,1,1,1),mStageNo(0), mInterval(0), mSize(0,0){}
	//テキスト生成
	void CreateText(std::shared_ptr<TextureData>textureData, std::shared_ptr<LocalData>localData,const VECTOR2F&uv,const VECTOR4F&color=VECTOR4F(1,1,1,1));
	//getter
	std::vector<std::shared_ptr<SelectText>>GetTextes() { return mTexts; }
	const int GetStageNo() { return mStageNo; }
	//setter
	void SetStageNo(const int number) { mStageNo = number; }
	void SetSize(const VECTOR2F& size) { mSize = size; }
	void SetInterval(const float interval) { mInterval = interval; }
	//更新
	void Update(int changeSelect,float elapsdTime,int nowStageNo,const VECTOR2F&centerPosition);
	//描画
	void Render(ID3D11DeviceContext* context, Sprite* sprite);
private:
	std::vector<std::shared_ptr<SelectText>>mTexts;
	//座標
	VECTOR2F mPosition;
	//スケール
	VECTOR2F mScale;
	//ボードのサイズ
	VECTOR2F mSize;
	//色
	VECTOR4F mColor;
	//ステージ番号
	int mStageNo;
	//ボードの幅
	float mInterval;
};