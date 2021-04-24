#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include"drow_shader.h"
#include<memory>
#include<vector>
#include"constant_buffer.h"
#include"cs_buffer.h"


class TitleParticle
{
public:
	TitleParticle(ID3D11Device* device);
	void Editor();
	void Update(float elapsdTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
	void SetChangeFlag(const bool changeFlag) { mSceneChange = changeFlag; }
private:
	//パーティクルバッファ
	std::unique_ptr<CSBuffer>mParticle;
	std::unique_ptr<CSBuffer>mParticleCount;
	//描画用バッファ
	std::unique_ptr<CSBuffer>mParticleRender;
	std::unique_ptr<CSBuffer>mParticleIndexs[2];
	std::unique_ptr<CSBuffer>mParticleDeleteIndex;
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mSceneChangeCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mRenderSetCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCountShader;
	std::unique_ptr<DrowShader>mShader;
	//srv
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;
	//パーティクルのデータ
	struct Particle
	{
		VECTOR3F position;
		VECTOR3F scale;
		VECTOR3F velocity;
		VECTOR3F angle;
		VECTOR4F color;
		float speed;
		float life;
		float moveAngle;
		float moveAngleMovement;
		float moveAngleLength;
		float maxLife;
		VECTOR3F defVelocity;
		VECTOR3F startPosition;
		VECTOR3F defPosition;

	};
	struct RenderParticle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};
	struct ParticleCount
	{
		UINT aliveParticleCount;
		UINT aliveNewParticleCount;
		UINT deActiveParticleCount;
		UINT dummy;
	};

	//定数バッファ
	struct CbStart
	{
		float startIndex;
		VECTOR3F startPosition;
		float leng;
		VECTOR3F sphereRatio;
		VECTOR4F color;
		float life;
		float moveLen;
		float randSpeed;
		float defSpeed;

	};
	struct CbStart2
	{
		float randMoveLength;
		float defMoveLength;
		float randMoveAngle;
		float randScale;
		VECTOR3F defVelocity;
		float dummy;

	};
	struct Cb
	{
		float elapsdTime;
		VECTOR3F angleMovement;
	};
	std::unique_ptr<ConstantBuffer<CbStart>>mCbStart;
	std::unique_ptr<ConstantBuffer<CbStart2>>mCbStart2;
	std::unique_ptr<ConstantBuffer<Cb>>mCb;
	//エディタデータ
	struct EditorData
	{
		VECTOR3F startPosition;
		float leng;
		VECTOR3F sphereRatio;
		VECTOR4F color;
		float life;
		float moveLen;
		float randSpeed;
		float defSpeed;
		float randMoveLength;
		float defMoveLength;
		float randMoveAngle;
		float randScale;
		VECTOR3F defVelocity;
		VECTOR3F angleMovement;
		UINT textureType;
	};
	EditorData mEditorData;
	//数
	float mStartIdex;
	//生成数
	float particleSize;
	//最大数
	int mMaxParticle;
	float mNewIndex;
	//シーンを切り替えるかどうか
	bool mSceneChange;
	//描画用データ
	UINT mRenderCount;
	int mIndexNum;

};