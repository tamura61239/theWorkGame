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
	void SetSceneDrowFlag(const bool flag) { mSceneDrowFlag = flag; }
	void Update(float elapsdTime, ID3D11DeviceContext*context);
	void Render(ID3D11DeviceContext* context);
private:
	//�V�[���Ɏʂ��Ă�Texture�̏����擾���邽�߂̕ϐ�
	std::unique_ptr<FrameBuffer>mArrangementSceneTexture;
	std::unique_ptr<Sprite>mRender;
	//�p�[�e�B�N���ɂ���Texture�̃f�[�^
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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mParticleSRV;
	//�p�[�e�B�N���f�[�^
	struct Particle
	{
		VECTOR3F position;
		VECTOR3F velocity;
		VECTOR4F color;
		float speed;
	};
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	//�`��p�f�[�^
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
	//�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	std::unique_ptr<DrowShader>mShader;
	//�萔
	struct CbUpdate
	{
		float elapsdTime;
		float scale;
		float speed;
		VECTOR2F spiralRatio;

		VECTOR3F dummy;
	};
	struct CbCamera
	{
		FLOAT4X4 inverseVP;
		float ndcZ;
		VECTOR3F dummy;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCameraBuffer;
	//�G�f�B�^
	struct EditorData
	{
		float scale;
		float speed;
		float ndcZ;
		VECTOR2F spiralRatio;

	};
	EditorData mEditorData;
	bool mSceneDrowFlag;
	bool mParticleFlag;
	int mMaxParticle;
	std::unique_ptr<blend_state>blend;
};
