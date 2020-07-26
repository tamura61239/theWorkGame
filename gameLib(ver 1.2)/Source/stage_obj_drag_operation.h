#pragma once
#include"stage_obj.h"

class StageObjDragOperation
{
public:
	StageObjDragOperation(ID3D11Device* device);
	void DragStart(const VECTOR3F& farMouse,const VECTOR3F&min,const VECTOR3F&max);
	int Update(std::shared_ptr<StageObj>obj, const VECTOR3F& nearMouse, const VECTOR3F& farMouse, int number);
	int isHitArrow(const VECTOR3F& nearMouse, const VECTOR3F& farMouse);
	void Move(std::shared_ptr<StageObj>obj, const VECTOR3F& farMouse,int arrowNumber);
	void SetArrow(std::shared_ptr<StageObj>obj);
	void Render(ID3D11DeviceContext*context,MeshRender* render);
private:
	VECTOR3F mBeforeFar;
	std::shared_ptr<StaticObj>mArrowObjs[3];
	VECTOR3F mArrowSize;
	VECTOR3F mDragObjSize;
	VECTOR3F mLocalPosition[3];
	VECTOR3F mLocalScale[3];
};