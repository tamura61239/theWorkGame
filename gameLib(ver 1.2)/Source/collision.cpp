#include "collision.h"

bool Collision::IsHitAABB(const VECTOR3F& obj1Min, const VECTOR3F& obj1Max, const VECTOR3F& obj2Min, const VECTOR3F& obj2Max, VECTOR3F* obj1HitDrection)
{
	if (obj1Min.x<obj2Max.x && obj1Max.x>obj2Min.x)
	{
		if (obj1Min.y<obj2Max.y && obj1Max.y>obj2Min.y)
		{
			if (obj1Min.z<obj2Max.z && obj1Max.z>obj2Min.z)
			{
				//VECTOR3F obj2Position = obj2Max - (obj2Max - obj2Min) * 0.5f;
				//VECTOR3F minVec = obj1Min - obj2Position;
				//VECTOR3F maxVec = obj1Max - obj2Position;
				//DirectX::XMVECTOR obj1MinVec = DirectX::XMLoadFloat3(&minVec);
				//DirectX::XMVECTOR obj1MaxVec = DirectX::XMLoadFloat3(&maxVec);
				//DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&VECTOR3F(0,0,1));
				//VECTOR3F cross1,cross2;
				//DirectX::XMStoreFloat3(&cross1, DirectX::XMVector3Cross(vec, DirectX::XMVector3Normalize(obj1MinVec)));
				//DirectX::XMStoreFloat3(&cross2, DirectX::XMVector3Cross(vec, DirectX::XMVector3Normalize(obj1MaxVec)));
				//if (cross1.x > 0 && cross2.x > 0)
				//{
				//	*obj1HitDrection = VECTOR3F(0, 1, 0);
				//}
				//else if (cross1.x < 0 && cross2.x < 0)
				//{
				//	*obj1HitDrection = VECTOR3F(0, -1, 0);
				//}
				//else
				//{
				//	*obj1HitDrection = VECTOR3F(0, 0, 1);
				//}
				
				return true;
			}
		}
	}
	return false;
}
