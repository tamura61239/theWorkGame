#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include"drow_shader.h"
#include"constant_buffer.h"
#include<memory>
#include<vector>
#include"cs_buffer.h"


class SelectSceneParticle
{
public:
	SelectSceneParticle(ID3D11Device* device);
	void Editor();
	void Update(float elapsdTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
private:
	//バッファ
	std::unique_ptr<CSBuffer>mParticle;
	std::unique_ptr<CSBuffer>mParticleCount;
	std::unique_ptr<CSBuffer>mParticleIndices[2];
	std::unique_ptr<CSBuffer>mParticleDeleteIndex;
	std::unique_ptr<CSBuffer>mParticleRender;
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSEndShader;
	std::unique_ptr<DrowShader>mShader;
	//srv
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;
	//パーティクル用データ
	struct Particle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR3F angleMovement;
		VECTOR4F color;
		VECTOR3F velocity;
		float speed;
		float life;
		float sinAngle;
		float sinAngleMovement;
		float sinLength;
		VECTOR3F centerPosition;
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
	struct CbCreate
	{
		VECTOR3F angleMovement;
		float speed;
		VECTOR4F color;
		float sinLeng;
		VECTOR3F eye;
		float range;
		VECTOR3F scope;
		float life;
		VECTOR3F dummy;
	};
	struct CbUpdate
	{
		VECTOR3F defVelocity;
		float elapsdTime;
		VECTOR3F endPosition;
		float dummy2;
	};
	struct EditorData
	{
		VECTOR3F angleMovement;
		float speed;
		VECTOR4F color;
		float range;
		VECTOR3F scope;
		VECTOR3F defVelocity;
		VECTOR3F endPosition;
		UINT textureType;
		float sinLeng;
		float life;
	};
	EditorData mEditorData;
	std::unique_ptr<ConstantBuffer<CbCreate>> mCbCreate;
	std::unique_ptr<ConstantBuffer<CbUpdate>> mCbUpdate;
	float newIndex;
	int mIndexCount;
	float mMaxParticle;
	UINT mRenderCount;
};