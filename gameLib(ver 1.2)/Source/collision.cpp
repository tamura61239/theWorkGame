#include "collision.h"
/*****************************************************/
//　　　　　　　　　　当たり判定関数
/*****************************************************/

//AABB(キューブとキューブ)の当たり判定
bool Collision::IsHitAABB(const VECTOR3F& obj1Min, const VECTOR3F& obj1Max, const VECTOR3F& obj2Min, const VECTOR3F& obj2Max, VECTOR3F* obj1HitDrection)
{
	if (obj1Min.x<=obj2Max.x && obj1Max.x>=obj2Min.x)
	{
		if (obj1Min.y<=obj2Max.y && obj1Max.y>=obj2Min.y)
		{
			if (obj1Min.z<=obj2Max.z && obj1Max.z>=obj2Min.z)
			{
				return true;
			}
		}
	}
	return false;
}
