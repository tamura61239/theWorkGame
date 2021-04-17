#pragma once
#include"stage_borad.h"

class StageSelect
{
public:
	//コンストラクタ
	StageSelect(ID3D11Device* device,const int maxCount);
	//エディタ
	void Editor();
	//更新
	bool Update(float elapsdTime);
	//ステージ選択
	void Select();
	//描画
	void Render(ID3D11DeviceContext* context);
	//getter
	const int GetSelectNumber() { return mSelectNumber; }
private:
	//ファイル操作
	void Load();
	void Save();
	//ステージ選択中
	bool mSelectSceneFlag;
	//テキストデータ
	std::shared_ptr<TextureData>mStageTexture;
	std::shared_ptr<TextureData>mNumberTexture;
	std::shared_ptr<TextureData>mBackTexture;
	std::vector<std::shared_ptr<TextureData>>mStageImageText;
	//描画用変数
	std::unique_ptr<Sprite>mDrow;
	//選択してるステージ番号
	int mSelectNumber;
	//エディタ変数
	float mInterval;
	struct Local
	{
		std::shared_ptr<LocalData>mData;
		std::string mName;
	};
	std::vector<Local>mLocalDatas;
	//ステージボード
	std::vector<std::unique_ptr<StageBorad>>mStageBoards;
	bool mStageBoardCreateFlag;
	VECTOR2F mBoardSize;
	//選択変更中変数
	int mChangeSelect;
	float mMoveTimer;
};