#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include"drow_shader.h"
#include<memory>
#include<vector>
#include<string>
#include"framebuffer.h"
#include"sprite.h"
#include"blend_state.h"

class TitleTextureParticle
{
public:
	TitleTextureParticle(ID3D11Device* device); 
	void ImGuiUpdate();
	void LoadTexture(ID3D11Device* device,std::wstring name, const VECTOR2F& leftTop, const VECTOR2F& size, const VECTOR2F& uv, const VECTOR2F& uvSize);
	void SetParticleFlag(const bool flag) { mParticleFlag = flag; }
	void SetFullDrowFlag(const bool flag) { mFullCreateFlag = flag; }
	void Update(float elapsdTime, ID3D11DeviceContext*context);
	void Render(ID3D11DeviceContext* context);
	const bool GetTextuteFlag() {
		return mTextureFlag;
	}
private:
	void SceneChangeUpdate(float elapsdTime, ID3D11DeviceContext* context);
	void Create1(float elapsdTime, ID3D11DeviceContext* context);
	//パーティクルにするTextureのデータ
	struct TextureData
	{
		VECTOR2F mLeftTop;
		VECTOR2F mSize;
		VECTOR2F mUVLeftTop;
		VECTOR2F mUVSize;
		std::wstring mTextureName;
	};
	struct Texture
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
		TextureData data;
	};
	std::vector<Texture>mTextures;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;
	//パーティクルデータ
	struct Particle
	{
		VECTOR3F position;
		VECTOR3F velocity;
		VECTOR4F color;
		float speed;
	};
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	//描画用データ
	struct RenderParticle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};
	//Buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	//UAV
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ComputeShader>>mCreateCSShader;

	std::unique_ptr<DrowShader>mShader;
	//定数
	struct CbUpdate
	{
		float elapsdTime;
		float scale;
		float speed;
		VECTOR2F spiralRatio;

		VECTOR3F dummy;
	};
	struct CbCreate
	{
		VECTOR2F leftTop;
		VECTOR2F uvSize;
		FLOAT4X4 world;
		float screenSplit;
		int startIndex;

		VECTOR2F dummy;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCreateBuffer;
	struct Board
	{
		VECTOR3F position;
		VECTOR3F scale;
	};
	//エディタ
	struct EditorData
	{
		float scale;
		float speed;
		float screenSplit;
		UINT textureType;
	};
	std::vector<Board>boards;
	EditorData mEditorData;
	bool mFullCreateFlag;
	bool mParticleFlag;
	bool mTestFlag;
	int mMaxParticle;
	UINT mMoveParticle;
	float mSceneParticleIndex;
	UINT mChangeMaxParticle;
	float mTimer;
	UINT mMaxTexture;
	float mBeforeTime;
	float mBeforeParsent;
	bool mTextureFlag;
	std::unique_ptr<BlendState>blend;
	void Load();
	void Save();
};
