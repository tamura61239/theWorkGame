#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>
#include"drow_shader.h"
#include<memory>

class StageSceneParticle
{
public:
	StageSceneParticle(ID3D11Device* device);
	void ImGuiUpdate();
	void Update(ID3D11DeviceContext* context, float elapsdTime);
	void Render(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context,DrowShader*shader);
private:
	void Load();
	void Save();
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	std::vector<std::unique_ptr<DrowShader>>mShader;
	//UAV
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	//バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCreateBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbUpdateBuffer;
	//SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mTextureSRV;
	//
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState;

	struct Particle
	{
		VECTOR3F position;
		VECTOR3F accel;
		VECTOR3F velocity;
		float life;
		VECTOR4F color;
		VECTOR3F scale;
	};
	struct RenderParticle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};
	struct CbCreate
	{
		int startIndex;
		VECTOR3F createCentralPosition;
		float randX;
		VECTOR3F createArea;
		VECTOR4F color;
		float maxLife;
		float scale;
		VECTOR2F colorRatio;
		VECTOR4F color2;
	};
	struct CbUpdate
	{
		float elapsdTime;
		VECTOR3F windDirection;
		float maxSpeed;
		VECTOR3F dummy2;

	};
	struct EditorData
	{
		float randX;
		VECTOR3F createCentralPosition;
		VECTOR3F createArea;
		VECTOR3F windDirection;
		float oneSecondCreateNumber;
		VECTOR4F color;
		float maxLife;
		float scale;
		float maxSpeed;
		VECTOR2F colorRatio;
		VECTOR4F color2;
		int shaderType;
	};
	float mIndexCount;
	float mBeforeIndex;
	int mMaxCount;
	EditorData mEditorData;
};