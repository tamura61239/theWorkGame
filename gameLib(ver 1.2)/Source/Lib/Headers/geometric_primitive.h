#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>
#include<vector>
#include"constant_buffer.h"
#include"drow_shader.h"
#include<memory>

//デフォルトオブジェクトクラス
class GeometricPrimitive
{
public:
	//コンストラクタ
	GeometricPrimitive(ID3D11Device* device) :mIndexNum(0) {};
	//デストラクタ
	virtual ~GeometricPrimitive() {};
	//getter
	Microsoft::WRL::ComPtr<ID3D11Buffer>GetVertexBuffer() { return mVertexBuffer; }
	Microsoft::WRL::ComPtr<ID3D11Buffer>GetIndexBuffer() { return mIndexBuffer; }

	const int& GetIndexNum() { return mIndexNum; }
	//頂点構造体
	struct Vertex
	{
		VECTOR3F position;
		VECTOR3F normal;
	};
protected:
	//バッファ生成
	void CreateBuffer(ID3D11Device* device, std::vector<Vertex>vertics, std::vector<UINT>index);
private:
	//index数
	int mIndexNum;
	//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mVertexBuffer;
	//indexバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mIndexBuffer;
};
//キューブ
class GeometricCube :public GeometricPrimitive
{
public:
	GeometricCube(ID3D11Device* device);
};
//円柱
class GeometricCylinder :public GeometricPrimitive
{
public:
	GeometricCylinder(ID3D11Device* device, u_int slices);
};
//球
class GeometricSphere :public GeometricPrimitive
{
public:
	GeometricSphere(ID3D11Device* device, u_int slices, u_int stacks);//slices:32,stacks:16
};

//描画クラス
class PrimitiveRender
{
public:
	//コンストラクタ
	PrimitiveRender(ID3D11Device* device);
	//描画
	void Begin(ID3D11DeviceContext* context, const VECTOR4F& light, const FLOAT4X4& view, const FLOAT4X4& projection);
	void Render(ID3D11DeviceContext* context, GeometricPrimitive* obj, const FLOAT4X4& world, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void Render(ID3D11DeviceContext* context, DrowShader* shader, GeometricPrimitive* obj, const FLOAT4X4& world, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void End(ID3D11DeviceContext* context);
private:
	//定数バッファ
	struct CbScene
	{
		VECTOR4F lightDirection;
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	struct CbObj
	{
		FLOAT4X4 world;
		VECTOR4F color;
	};
	std::unique_ptr<ConstantBuffer<CbScene>>mCbScene;
	std::unique_ptr<ConstantBuffer<CbObj>>mCbObj;
	//シェーダー
	std::unique_ptr<DrowShader>mShader;
};
