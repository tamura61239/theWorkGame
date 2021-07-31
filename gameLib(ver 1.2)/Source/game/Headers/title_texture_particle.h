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
	//�R���X�g���N�^
	TitleTextureParticle(ID3D11Device* device);
	TitleTextureParticle(const TitleTextureParticle&texture){}
	//�G�f�B�^
	void Editor();
	//�p�[�e�B�N���𐶐�����̂Ɏg���e�N�X�`���̓ǂݍ���
	void LoadTexture(ID3D11Device* device,std::wstring name, const VECTOR2F& leftTop, const VECTOR2F& size, const VECTOR2F& uv, const VECTOR2F& uvSize);
	//�X�V
	void Update(float elapsdTime, ID3D11DeviceContext*context);
	//�`��
	void Render(ID3D11DeviceContext* context);
	//setter
	void SetParticleFlag(const bool flag) { mParticleFlag = flag; }
	void SetFullDrowFlag(const bool flag) { mFullCreateFlag = flag; }
	//getter
	const bool GetTextuteFlag() {
		return mTextureFlag;
	}
private:
	//�V�[�����ύX�����Ƃ��̃p�[�e�B�N������
	void SceneChangeUpdate(float elapsdTime, ID3D11DeviceContext* context);
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
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;
	//�p�[�e�B�N���f�[�^
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
	//�o�b�t�@
	std::unique_ptr<CSBuffer>mParticle;
	std::unique_ptr<CSBuffer>mParticleRender;
	//�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ComputeShader>>mCreateCSShader;
	std::unique_ptr<DrowShader>mShader;
	//�萔�o�b�t�@
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
	//�G�f�B�^
	struct EditorData
	{
		float scale;
		float speed;
		float screenSplit;
		UINT textureType;
	};
	EditorData mEditorData;
	//�e�N�X�`���̂𒣂��Ă�|���̃f�[�^
	struct Board
	{
		VECTOR3F position;
		VECTOR3F scale;
	};
	std::vector<Board>boards;
	//�X�V�֘A�t���O
	bool mFullCreateFlag;
	bool mParticleFlag;
	bool mTestFlag;
	//�ő吔
	int mMaxParticle;
	//�����Ă�p�[�e�B�N���̐�
	UINT mMoveParticle;
	//�V�[���J�ڂ̉��o�̂��߂ɐ������ꂽ�p�[�e�B�N���̐�
	UINT mChangeMaxParticle;
	//�p�[�e�B�N�����o���e�N�X�`��(�|��)�̐�
	UINT mMaxTexture;
	//�e�N�X�`��(UI)��`�悷�邩�ǂ���
	bool mTextureFlag;
	//�t�@�C������
	void Load();
	void Save();
};
