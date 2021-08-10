//回転行列計算関数
float3x3 MakeRotation(float3 axis, float angle)
{
	//回転行列を作るためのパラメーターを計算する
	float cs = cos(angle);
	float sn = sin(angle);
	float oneMinusCos = 1.0 - cs;
	float3 squared = axis * axis;
	float xym = axis.x * axis.y * oneMinusCos;
	float xzm = axis.x * axis.z * oneMinusCos;
	float yzm = axis.y * axis.z * oneMinusCos;
	float3 xyzSin = axis * sn;
	//回転行列を作る
	return float3x3(
		squared.x * oneMinusCos + cs, xym - xyzSin.z, xzm + xyzSin.y,
		xym + xyzSin.z, squared.y * oneMinusCos + cs, yzm - xyzSin.x,
		xzm - xyzSin.y, yzm + xyzSin.x, squared.z * oneMinusCos + cs
		);
}