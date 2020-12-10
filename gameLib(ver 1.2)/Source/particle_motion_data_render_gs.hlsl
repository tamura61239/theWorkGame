#include"particle_motiom_data_render.hlsli"
#include"before_camera_view_data.hlsli"
#define BLUER_TYPE 0
static const float value = 3.f;
[maxvertexcount(4 * 9)]
void main(
	point VS_OUT input[1],
	inout TriangleStream<GS_OUT> output
)
{

    float4 texPosition[4] =
    {
        { -0.5f, 0.5f, 0, 0 },
        { 0.5f, 0.5f, 0, 0 },
        { -0.5f, -0.5f, 0, 0 },
        { 0.5f, -0.5f, 0, 0 },
    };
    float2 texcoord[4] =
    {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
    };
    float4 nowPosition, beforePosition, vec, p0;
    GS_OUT gout = (GS_OUT) 0;
#if BLUER_TYPE
	//[unroll]
 //   for (int i = 0; i < 4; i++)
 //   {
 //       nowPosition = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[i] * input[0].scale.x, projection);
 //       beforePosition = mul(mul(float4(input[0].worldPosition - input[0].velocity, 1), beforeView) + texPosition[i] * input[0].scale.x, beforeProjection);
 //       gout.position = nowPosition;
 //       gout.velocity = nowPosition.xyz - beforePosition.xyz;
 //       gout.texcoord = texcoord[i];
 //       gout.color = input[0].color;
 //       output.Append(gout);

 //   }
#else
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        nowPosition = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[i] * input[0].scale.x, projection);
        beforePosition = mul(mul(float4(input[0].worldPosition - input[0].velocity, 1), beforeView) + texPosition[i] * input[0].scale.x, beforeProjection);
        gout.position = nowPosition;
        vec = (beforePosition - nowPosition) * 0.5f * value;
        p0 = nowPosition + vec;
        gout.texcoord = texcoord[i];
        gout.color.xy = nowPosition.xy / nowPosition.w - p0.xy / p0.w;
        gout.color.z = gout.position.z / gout.position.w;
        output.Append(gout);
    }

    [unroll]
    for (int j = 0; j < 4; j++)
    {
        int id0 = (j + 0) % 4,
			id1 = (j + 1) % 4;
        {
            nowPosition = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[id0] * input[0].scale.x, projection);
            beforePosition = mul(mul(float4(input[0].worldPosition - input[0].velocity, 1), beforeView) + texPosition[id0] * input[0].scale.x, beforeProjection);
            vec = (beforePosition - nowPosition) * 0.5f * value;
            p0 = nowPosition + vec;
            gout.position = nowPosition;
            gout.texcoord = texcoord[id0];
            gout.color.xy = nowPosition.xy / nowPosition.w - p0.xy / p0.w;
            gout.color.z = gout.position.z / gout.position.w;
            output.Append(gout);
            
            gout.position = p0;
            gout.texcoord = texcoord[id0];
            gout.color.xy = nowPosition.xy / nowPosition.w - p0.xy / p0.w;
            gout.color.z = gout.position.z / gout.position.w;
            output.Append(gout);

            nowPosition = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[id1] * input[0].scale.x, projection);
            beforePosition = mul(mul(float4(input[0].worldPosition - input[0].velocity, 1), beforeView) + texPosition[id1] * input[0].scale.x, beforeProjection);
            vec = (beforePosition - nowPosition) * 0.5f * value;
            p0 = nowPosition + vec;
            gout.position = nowPosition;
            gout.texcoord = texcoord[id1];
            gout.color.xy = nowPosition.xy / nowPosition.w - p0.xy / p0.w;
            gout.color.z = gout.position.z / gout.position.w;
            output.Append(gout);

        }
        {
            nowPosition = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[id0] * input[0].scale.x, projection);
            beforePosition = mul(mul(float4(input[0].worldPosition - input[0].velocity, 1), beforeView) + texPosition[id0] * input[0].scale.x, beforeProjection);
            vec = (beforePosition - nowPosition) * 0.5f * value;
            p0 = nowPosition + vec;
            gout.position = p0;
            gout.texcoord = texcoord[id0];
            gout.color.xy = nowPosition.xy / nowPosition.w - p0.xy / p0.w;
            gout.color.z = gout.position.z / gout.position.w;
            output.Append(gout);

            nowPosition = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[id1] * input[0].scale.x, projection);
            beforePosition = mul(mul(float4(input[0].worldPosition - input[0].velocity, 1), beforeView) + texPosition[id1] * input[0].scale.x, beforeProjection);
            vec = (beforePosition - nowPosition) * 0.5f * value;
            p0 = nowPosition + vec;
            gout.position = p0;
            gout.texcoord = texcoord[id1];
            gout.color.xy = nowPosition.xy / nowPosition.w - p0.xy / p0.w;
            gout.color.z = gout.position.z / gout.position.w;
            output.Append(gout);

            nowPosition = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[id1] * input[0].scale.x, projection);
            beforePosition = mul(mul(float4(input[0].worldPosition - input[0].velocity, 1), beforeView) + texPosition[id1] * input[0].scale.x, beforeProjection);
            vec = (beforePosition - nowPosition) * 0.5f * value;
            p0 = nowPosition + vec;
            gout.position = nowPosition;
            gout.texcoord = texcoord[id1];
            gout.color.xy = nowPosition.xy / nowPosition.w - p0.xy / p0.w;
            gout.color.z = gout.position.z / gout.position.w;
            output.Append(gout);

        }

    }
    [unroll]
    for (int k = 0; k < 4; k++)
    {
        nowPosition = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[k] * input[0].scale.x, projection);

        beforePosition = mul(mul(float4(input[0].worldPosition - input[0].velocity, 1), beforeView) + texPosition[k] * input[0].scale.x, beforeProjection);
        vec = (beforePosition - nowPosition) * 0.5f * 0.5f;
        p0 = nowPosition + vec;
        gout.position = p0;
        gout.texcoord = texcoord[k];
        gout.color.xy = nowPosition.xy / nowPosition.w - p0.xy / p0.w;
        gout.color.z = gout.position.z / gout.position.w;
        output.Append(gout);

    }
#endif
    output.RestartStrip();

}