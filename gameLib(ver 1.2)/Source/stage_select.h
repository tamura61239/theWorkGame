#pragma once
#include<d3d11.h>
#include"vector.h"
#include<memory>
#include<vector>
#include"sprite.h"
#include"stage_manager.h"

class StageSelect
{
public:
	StageSelect(ID3D11Device* device);
	void ImGuiUpdate();
	void Update(float elapsdTime,StageManager*manager);
	void Select(StageManager* manager);
	void Move(float elapsdtime);
	void Mask();
	void Render(ID3D11DeviceContext* context);
	const bool GetSelectFlag() { return mSelectSceneFlag; }
	void SetSelectFlag(const bool flag) { mSelectSceneFlag = flag; }
private:
	void Load();
	void Save();
	bool mSelectSceneFlag;
	struct Texture
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
		D3D11_TEXTURE2D_DESC mDesc;
		VECTOR2F mDrowSize;
	};
	Texture mSelect;
	Texture mNumber;
	std::unique_ptr<Sprite>mDrow;
	int mSelectNumber;
	VECTOR2F mSizeParsent;
	VECTOR2F mMaskLeftUp;
	VECTOR2F mMaskSize;
	int mMaxStage;
	VECTOR2F mPositionParsent;
	VECTOR4F mTextColor;
};