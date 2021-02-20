#pragma once
#include"drow_shader.h"
#include<memory>
#include"vector.h"
#include<vector>
class FireworksParticle
{
public:
	FireworksParticle(ID3D11Device* device);
	void ImGuiUpdate();
	void Update(float elapsdTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context,DrowShader*shader);
	void CreateEmitor(const int ranking)
	{
		mCreateFlag = true;
		mEmitorTimer = 0;
		mEmitors.resize(mStartEmitorData.size());
		mCreateCount.resize(mStartEmitorData.size());
		memset(&mCreateCount[0], 0, sizeof(int) * mCreateCount.size());
		mIndex = 0;
		mState = 0;
		mNowPlayRanking = ranking;
		SetStartList((5 - mNowPlayRanking) * mEditorData.mOneRankEmitorCount+ mEditorData.mOneRankEmitorCount/2);
	}
	void ClearEmitor()
	{
		mCreateFlag = false;
		mEmitorTimer = 0;
		mEmitors.clear();
		mCreateCount.clear();
		mIndex = 0;
		mState = 0;
	}
private:
	/****************************変数*******************************/
	//シェーダー
	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreate1Shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreate2Shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	//描画用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCreateBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCreate2Buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbUpdateBuffer;

	//UAV
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	//SRV
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;

	struct EmitorData
	{
		EmitorData() :position(0, 0, 0), velocity(0, 0, 0), type(0), maxLife(0), speed(0), emitorStartTime(0) {}

		VECTOR3F position;
		VECTOR3F velocity;
		float maxLife;
		float speed;
		float emitorStartTime;
		int type;
	};
	struct Emitor
	{
		Emitor() :position(0, 0, 0), velocity(0, 0, 0), type(-1), life(0) {}
		VECTOR3F position;
		VECTOR3F velocity;
		int type;
		float life;
	};
	struct Particle
	{
		VECTOR3F position;
		VECTOR3F velocity;
		VECTOR3F accel;
		float scale;
		float life;
		float lifeAmoust;
		float endTime;
		float endTimer;
		VECTOR4F startColor;
		VECTOR4F endColor;
	};
	struct RenderParticle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};
	struct FireworksData
	{
		float maxCount;
		float maxLife;
		float speed;
		float scale;
		float parsent;
		float endTimer;
		float gravity;

		float dummy;

		VECTOR4F color;
		VECTOR4F endColor;
	};
	struct CreateData
	{
		VECTOR3F position;
		float dummy2;
		VECTOR3F velocity;
		float dummy3;
		FireworksData firework;
	};

	struct CbCreate
	{
		CreateData createData[30];
		int startIndex;
		VECTOR3F dummy2;

	};
	struct CbUpdate
	{
		float elapsdime;
		VECTOR3F dummy;
	};
	struct EditorData
	{
		float mMaxEmitorTime;
		float mStartMaxTime;
		int mOneRankEmitorCount;
		UINT textureType;
	};
	/************editorParameter*************/
	std::vector<EmitorData>mEmitorData;
	std::vector<EmitorData>mStartEmitorData;
	std::vector<FireworksData>mFireworkDatas;
	EditorData mEditorData;
	//セーブしない変数
	bool mParameterEditFlag;
	bool mCreateFlag;
	int mDefRanking;
	int mDefStartState;

	//updataParameter
	int mMaxParticle;
	float mEmitorTimer;
	int mIndex;
	int mNowPlayRanking;
	int mState;
	int mFireworksCount;
	std::vector<Emitor>mEmitors;
	std::vector<int>mCreateCount;
	std::vector<int>mStartCreateNumberList;
	/****************************関数********************************/
	void Load();
	void Save();
	void StartFireworksEmitorUpdate(float elapsdTime, CbCreate* smokeConstant, CbCreate* fireworksConstant, int& emitorCount, int& fireworksCount);
	void LoopFireworksEmitorUpdate(float elapsdTime, CbCreate* smokeConstant, CbCreate* fireworksConstant, int& emitorCount, int& fireworksCount);
	void SetStartList(const int size);

};