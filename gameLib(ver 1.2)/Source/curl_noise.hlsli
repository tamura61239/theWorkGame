//aのnによる剰余
float mod(float a, float n)
{
	return a - floor(a / n) * n;
}
float2 mod(float2 a, float n)
{
	return a - floor(a / n) * n;
}
float3 mod(float3 a, float n)
{
	return a - floor(a / n) * n;
}
float4 mod(float4 a, float n)
{
	return a - floor(a / n) * n;
}

float4 permute(float4 a)
{
	return mod((a * 34.0 + 1.0) * a, 289);
}
float permute(float a)
{
	return mod((a * 34.0 + 1.0) * a, 289);
}

float4 taylorInvSqrt(float4 r)
{
	return 1.79284291400159f - r * 0.85373472095314f;
}
//rand
float nrand(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}
uint randXorshift(uint rngState)
{
	rngState ^= (rngState << 13);
	rngState ^= (rngState << 17);
	rngState ^= (rngState << 5);
	return rngState;
}

float4 grad4(float j, float4 ip)
{
	float4 ones = float4(1, 1, 1, -1);
	float4 p, s;
	p.xyz = floor(frac(float3(j, j, j) * ip.xyz) * 7) * ip.z - 1;
	p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
	//step(a,x):(x >= a) ? 1 : 0 (xがa以上の場合は1, それ以外は0を返します)
	//lessThan(p,0):要素ごとのx<yの比較を返す
	s.x = lerp(p.x, 0, step(p.x, 0));
	s.y = lerp(p.y, 0, step(p.y, 0));
	s.z = lerp(p.z, 0, step(p.z, 0));
	s.w = lerp(p.w, 0, step(p.w, 0));
	p.xyz = p.xyz + (s.xyz * 2 - 1) * s.www;
	return p;
}
float3 snoise(float3 vec)
{
	float4 v = float4(vec, 1);
	float4 c = float4(0.138196601125011, 0.276393202250021, 0.414589803375032, -0.447213595499958);
	//floor(x):x以下の最大の整数を返す(floorは小数点以下の切り捨てに使える)
	//①vにvとある方向との射影ベクトルを足してそれより少し小さいベクトルを取得
	float4 i = floor(v + dot(v, float4(0.309016994374947451, 0.309016994374947451, 0.309016994374947451, 0.309016994374947451)));
	//
	float4 x0 = v - i + dot(i, c.xxxx);

	float4 i0;
	float3 isX = step(x0.yzw, x0.xxx);
	float3 isYZ = step(x0.zww, x0.yyz);
	i0.x = isX.x + isX.y + isX.z;
	i0.yzw = 1.0f - isX;
	i0.y += isYZ.x + isYZ.y;
	i0.zw += 1.0f - isYZ.xy;
	i0.z += isYZ.z;
	i0.w += 1.0f - isYZ.z;
	//saturate(x):x を [0, 1] の範囲にクランプします。([0, 1] の範囲に押し込む的なもの)
	float4 i3 = saturate(i0);
	float4 i2 = saturate(i0 - 1);
	float4 i1 = saturate(i0 - 2);

	float4 x1 = x0 - i1 + c.xxxx;
	float4 x2 = x0 - i2 + c.yyyy;
	float4 x3 = x0 - i3 + c.zzzz;
	float4 x4 = x0 + c.wwww;

	i = mod(i, 289);
	float j0 = permute(permute(permute(permute(i.w) + i.z) + i.y) + i.x);
	float4 j1 = permute(permute(permute(permute(
		i.w + float4(i1.w, i2.w, i3.w, 1.0f))
		+ i.z + float4(i1.z, i2.z, i3.z, 1.0f))
		+ i.y + float4(i1.y, i2.y, i3.y, 1.0f))
		+ i.x + float4(i1.x, i2.x, i3.x, 1.0f));

	float4 ip = float4(1.0 / 294.0, 1.0 / 49.0, 1.0 / 7.0, 0.0);

	float4 p0 = grad4(j0, ip);
	float4 p1 = grad4(j1.x, ip);
	float4 p2 = grad4(j1.y, ip);
	float4 p3 = grad4(j1.z, ip);
	float4 p4 = grad4(j1.w, ip);

	float4 norm = taylorInvSqrt(float4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	p4 *= taylorInvSqrt(dot(p4, p4));

	float3 values0 = float3(dot(p0, x0), dot(p1, x1), dot(p2, x2));
	float2 values1 = float2(dot(p3, x3), dot(p4, x4));

	float3 m0 = max(0.5f - float3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0);
	float2 m1 = max(0.5f - float2(dot(x3, x3), dot(x4, x4)), 0);

	float3 temp0 = -6 * m0 * m0 * values0;
	float2 temp1 = -6 * m1 * m1 * values1;

	float3 mmm0 = m0 * m0 * m0;
	float2 mmm1 = m1 * m1 * m1;
	//様々なnoiseを混ぜる
	float4 d = temp0.x * x0 + temp0.y * x1 + temp0.z * x2
		+ temp1.x * x3 + temp1.y * x4 + mmm0.x * p1
		+ mmm0.y * p2 + mmm1.x * p3 + mmm1.y * p4;
	return normalize(d.xyz) * d.w;
}

