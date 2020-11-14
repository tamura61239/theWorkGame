#pragma once
#include"static_mesh.h"
#include"obj3d.h"
#include<vector>
#if 0
#include"singleton_macro.h"
class HitAreaRender
{
public:
	DEFINE_SINGLETON(HitAreaRender);
	void Initialize(ID3D11Device* device);
	//setter
	void SetObjSize(const int size);
	void SetObjData(const VECTOR3F& position, const VECTOR3F& scale);
	//•`‰æ
	void Render(ID3D11DeviceContext* context,const FLOAT4X4&view, const FLOAT4X4& projection);
	//clear
	void ClearData();
private:
	HitAreaRender():mCount(0){}
	HitAreaRender(const HitAreaRender&) = delete;
	HitAreaRender& operator =(const HitAreaRender&) = delete;
	std::unique_ptr<MeshRender>mRender;
	std::vector<std::unique_ptr<Obj3D>>mObjData;
	std::unique_ptr<StaticMesh>mMesh;
	int mCount;
};
#else
#include"singleton_class.h"
class HitAreaRender:public Singleton<HitAreaRender>
{
public:
	void Initialize(ID3D11Device* device);
	//setter
	void SetObjSize(const int size);
	void SetObjData(const VECTOR3F& position, const VECTOR3F& scale);
	//•`‰æ
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	//clear
	void ClearData();
	void ClearCount()
	{
		mCount = 0;
	}
private:
	std::unique_ptr<MeshRender>mRender;
	std::vector<std::unique_ptr<Obj3D>>mObjData;
	std::unique_ptr<StaticMesh>mMesh;
	int mCount;
};

#endif