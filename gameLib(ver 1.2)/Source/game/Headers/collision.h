#pragma once
#include"vector.h"

class Collision
{
public:
	//AABB(キューブとキューブ)の当たり判定
	static bool IsHitAABB(const VECTOR3F& obj1Min, const VECTOR3F& obj1Max, const VECTOR3F& obj2Min, const VECTOR3F& obj2Max, VECTOR3F* obj1HitDrection);
};