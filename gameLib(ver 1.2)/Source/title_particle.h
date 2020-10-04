#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include"drow_shader.h"
#include<memory>

class TitleParticle
{
public:
	TitleParticle(ID3D11Device* device);
	void ImGuiUpdate();
	void Update(float elapsdTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbStartBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbStart2Buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mNumberBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mNumberUAV;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mSceneChangeCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mRenderSetCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mClearCSShader;
	std::unique_ptr<DrowShader>mShader;
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
	CbStart mCbStart;
	CbStart2 mCbStart2;
	Cb mCb;
	float particleSize;
	float mNewIndex;
	bool mSceneChange;
	void Load();
	void Save();
};