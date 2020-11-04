#pragma once
#include"stage_manager.h"
#include"stage_borad.h"

class StageSelect
{
public:
	StageSelect(ID3D11Device* device,const int maxCount);
	void ImGuiUpdate();
	void Update(float elapsdTime,StageManager*manager);
	void Select(StageManager* manager);
	void Render(ID3D11DeviceContext* context);
	const bool GetSelectFlag() { return mSelectSceneFlag; }
	void SetSelectFlag(const bool flag) { mSelectSceneFlag = flag; }
private:
	void Load();
	void Save();
	bool mSelectSceneFlag;
	std::shared_ptr<TextureData>mStageTexture;
	std::shared_ptr<TextureData>mNumberTexture;
	std::shared_ptr<TextureData>mBackTexture;
	std::vector<std::shared_ptr<TextureData>>mStageImageText;
	std::unique_ptr<Sprite>mDrow;
	int mSelectNumber;
	float mInterval;
	struct Local
	{
		std::shared_ptr<LocalData>mData;
		std::string mName;
	};
	std::vector<Local>mLocalDatas;
	std::vector<std::unique_ptr<StageBorad>>mStageBoards;
	bool mStageBoardCreateFlag;
	VECTOR2F mBoardSize;
	int mChangeSelect;
	float mMoveTimer;
	//VECTOR2F mSizeParsent;
	//VECTOR2F mMaskLeftUp;
	//VECTOR2F mMaskSize;
	//int mMaxStage;
	//VECTOR2F mPositionParsent;
	//VECTOR4F mTextColor;
};