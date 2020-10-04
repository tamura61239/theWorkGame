RWTexture2D<float4> neighborMaxTexture : register(u0);
Texture2D<float4> velocityTexture : register(t0);
#define M 3
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	/*neighborMaxTexture[DTid.xy] = velocityTexture[DTid.xy];*/
	float4 velocity = (float4)0;
	for (int x = 0; x < M; x++)
	{
		for(int y = 0; y < M; y++)
		{
			float2 uv = DTid.xy + float2(x, y);
			float4 vec = velocityTexture[DTid.xy];
			if (length(velocity.xy) < length(vec.xy))
			{
				velocity = vec;
			}
		}
	}
	neighborMaxTexture[DTid.xy] = velocity;
}