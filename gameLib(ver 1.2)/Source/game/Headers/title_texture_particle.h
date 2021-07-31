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
#include"cs_buffer.h"
#include"constant_buffer.h"

class TitleTextureParticle
{
public:
	//コンストラクタ
	TitleTextureParticle(ID3D11Device* device);
	TitleTextureParticle(const TitleTextureParticle&texture){}
	//エディタ
	void Editor();
	//パーティクルを生成するのに使うテクスチャの読み込み
	void LoadTexture(ID3D11Device* device,std::wstring name, const VECTOR2F& leftTop, const VECTOR2F& size, const VECTOR2F& uv, const VECTOR2F& uvSize);
	//更新
	void Update(float elapsdTime, ID3D11DeviceContext*context);
	//描画
	void Render(ID3D11DeviceContext* context);
	//setter
	void SetParticleFlag(const bool flag) { mParticleFlag = flag; }
	void SetFullDrowFlag(const bool flag) { mFullCreateFlag = flag; }
	//getter
	const bool GetTextuteFlag() {
		return mTextureFlag;
	}
private:
	//シーンが変更されるときのパーティクル生成
	void SceneChangeUpdate(float elapsdTime, ID3D11DeviceContext* context);
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
	struct RenderParticle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};
	//バッファ
	std::unique_ptr<CSBuffer>mParticle;
	std::unique_ptr<CSBuffer>mParticleRender;
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ComputeShader>>mCreateCSShader;
	std::unique_ptr<DrowShader>mShader;
	//定数バッファ
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
	std::unique_ptr<ConstantBuffer<CbCreate>>mCbCreate;
	std::unique_ptr<ConstantBuffer<CbUpdate>>mCbUpdate;
	//エディタ
	struct EditorData
	{
		float scale;
		float speed;
		float screenSplit;
		UINT textureType;
	};
	EditorData mEditorData;
	//テクスチャのを張ってる板ポリのデータ
	struct Board
	{
		VECTOR3F position;
		VECTOR3F scale;
	};
	std::vector<Board>boards;
	//更新関連フラグ
	bool mFullCreateFlag;
	bool mParticleFlag;
	bool mTestFlag;
	//最大数
	int mMaxParticle;
	//動いてるパーティクルの数
	UINT mMoveParticle;
	//シーン遷移の演出のために生成されたパーティクルの数
	UINT mChangeMaxParticle;
	//パーティクルを出すテクスチャ(板ポリ)の数
	UINT mMaxTexture;
	//テクスチャ(UI)を描画するかどうか
	bool mTextureFlag;
	//ファイル操作
	void Load();
	void Save();
};
