#include "sprite.hlsli"   
Texture2D diffuse_map : register(t0);
Texture2D velocity_map : register(t1);
SamplerState diffuse_map_sampler_state : register(s0);

float2 rand2(float2 s)
{
    s = float2(dot(s, float2(12.9898f, 78.233f)), dot(s, float2(269.5f, 183.3f)));
    return frac(sin(s) * float2(43758.5453123f, 43758.5453123f));
}

float4 main(VS_OUT pin) : SV_TARGET
{
#if 0
    float2 vec = velocity_map.Sample(diffuse_map_sampler_state, pin.texcoord).xy;
    float4 color = (float4) 0;
    float rand = rand2(vec);
    float total = 0;
    float nFrag = 1.0f / 20.0f;
    for (int i = 0; i < 20; i++)
    {
        float percent = (i + rand) * nFrag;
        float weight = percent - percent * percent;

        float2 uv = pin.texcoord - vec * percent * 3.0f * nFrag;
        total += weight;
        color += diffuse_map.Sample(diffuse_map_sampler_state, uv) * pin.color * weight;
    }
    color.w = 1.0f;
    color.rgb /= total;
    return color;
#else
    //	�s�N�Z���T�C�Y���Z�o
    uint width, height;
    diffuse_map.GetDimensions(width, height);
    const float2 uvPerPix = float2(0.5f / (float) width, 0.5f / (float) height);
    	//	�u���[�����̎Z�o
    const float4 velocityMap = velocity_map.Sample(diffuse_map_sampler_state, pin.texcoord);
    const float2 velocity = velocityMap.xy;
    const float depth = velocityMap.z;

	//	�u���[������ׂ����߂̌W��
    float2 jitter = 1.0f - (rand2(velocityMap.xy) - 0.5f);

	//	���x�x�N�g�������ɂ��ău���[��������
    float4 color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord);
    int loop = 8 / 2;
    for (int ii = -loop; ii <= loop; ++ii)
    {
        if (ii == 0)
            continue;
        float2 uv = pin.texcoord + ii * ((velocity * jitter) / 8);
        color += diffuse_map.Sample(diffuse_map_sampler_state, uv);
    }
    color.xyz /= color.a;

    return color;
#endif
}