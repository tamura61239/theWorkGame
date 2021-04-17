#pragma once
#include"stage_borad.h"

class StageSelect
{
public:
	//�R���X�g���N�^
	StageSelect(ID3D11Device* device,const int maxCount);
	//�G�f�B�^
	void Editor();
	//�X�V
	bool Update(float elapsdTime);
	//�X�e�[�W�I��
	void Select();
	//�`��
	void Render(ID3D11DeviceContext* context);
	//getter
	const int GetSelectNumber() { return mSelectNumber; }
private:
	//�t�@�C������
	void Load();
	void Save();
	//�X�e�[�W�I��
	bool mSelectSceneFlag;
	//�e�L�X�g�f�[�^
	std::shared_ptr<TextureData>mStageTexture;
	std::shared_ptr<TextureData>mNumberTexture;
	std::shared_ptr<TextureData>mBackTexture;
	std::vector<std::shared_ptr<TextureData>>mStageImageText;
	//�`��p�ϐ�
	std::unique_ptr<Sprite>mDrow;
	//�I�����Ă�X�e�[�W�ԍ�
	int mSelectNumber;
	//�G�f�B�^�ϐ�
	float mInterval;
	struct Local
	{
		std::shared_ptr<LocalData>mData;
		std::string mName;
	};
	std::vector<Local>mLocalDatas;
	//�X�e�[�W�{�[�h
	std::vector<std::unique_ptr<StageBorad>>mStageBoards;
	bool mStageBoardCreateFlag;
	VECTOR2F mBoardSize;
	//�I��ύX���ϐ�
	int mChangeSelect;
	float mMoveTimer;
};