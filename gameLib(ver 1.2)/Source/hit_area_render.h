#pragma once
#include"static_mesh.h"
#include"obj3d.h"
#include<vector>
#include"singleton_class.h"
class HitAreaRender:public Singleton<HitAreaRender>
{
public:
	//初期化
	void Initialize(ID3D11Device* device);
	//setter
	void SetObjSize(const int size);
	void SetObjData(const VECTOR3F& position, const VECTOR3F& scale);
	//描画
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	//clear
	void ClearData();
	void ClearCount()
	{
		mCount = 0;
	}
private:
	//描画用変数
	std::unique_ptr<MeshRender>mRender;
	//オブジェクト
	std::vector<std::unique_ptr<Obj3D>>mObjData;
	//メッシュ
	std::unique_ptr<StaticMesh>mMesh;
	//カウント
	int mCount;
};

