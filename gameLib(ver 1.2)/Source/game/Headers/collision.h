#pragma once
#include"vector.h"

class Collision
{
public:
	//AABB(�L���[�u�ƃL���[�u)�̓����蔻��
	static bool IsHitAABB(const VECTOR3F& obj1Min, const VECTOR3F& obj1Max, const VECTOR3F& obj2Min, const VECTOR3F& obj2Max, VECTOR3F* obj1HitDrection);
};