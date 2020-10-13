#pragma once
#include"ui.h"
#include<vector>
#include"title_ui_move.h"

class UIManager
{
public:
	static UIManager&GetInctance()
	{
		static UIManager uiManager;
		return uiManager;
	}
	//������
	void TitleInitialize(ID3D11Device* device);
	void GameInitialize(ID3D11Device* device);
	//�N���A
	void Clear();
	void ClearUI();
	//�X�V
	void ImGuiUpdate();
	void Update(float elapsdTime);
	void Render(ID3D11DeviceContext* context);
	//getter
	const bool GetMoveChangeFlag() { return mTitleMove->GetMoveChangeFlag(); }
private:
	UIManager(){}
	std::vector<std::shared_ptr<UI>>mUIs;
	std::vector<std::string>mNames;
	std::unique_ptr<TitleUIMove>mTitleMove;
	int mUINumber;
	std::string mSceneName;
	bool mMoveStartFlag;
	void Load(const char* scene);
	void Save(const char* scene);
};