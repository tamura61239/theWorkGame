RWByteAddressBuffer		rwbParticle : register(u0);

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	// パーティクルの通し番号を求める
	uint	index = DTid.z * 100 * 1 + DTid.y * 100 + DTid.x;

	uint bufferIndex = index * 44;
	float4 position = asfloat(rwbParticle.Load4(bufferIndex + 0));
	float3 velocity = asfloat(rwbParticle.Load3(bufferIndex + 16));
	float4 color = asfloat(rwbParticle.Load4(bufferIndex + 28));
	position.xyz += velocity;
	//if (position.x > 10 || position.x < -10)
	//{
	//	if (position.y > 10 || position.y < -10)
	//	{
	//		if (position.z > 10 || position.z < -10)
	//		{
	//			position = float4(0, 0, 0, 1);
	//		}
	//	}
	//}
	if (position.x > 10 || position.x < -10)position = float4(0, 0, 0, 1);
	if (position.y > 10 || position.y < -10)position = float4(0, 0, 0, 1);
	if (position.z > 10 || position.z < -10)position = float4(0, 0, 0, 1);
	rwbParticle.Store4(bufferIndex + 0, asuint(position));
	rwbParticle.Store3(bufferIndex + 16, asuint(velocity));
	rwbParticle.Store4(bufferIndex + 28, asuint(color));
}