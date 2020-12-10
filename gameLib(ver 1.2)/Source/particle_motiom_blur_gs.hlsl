#include"particle_motiom_data_render.hlsli"
#include"before_camera_view_data.hlsli"
#include"rand_function.hlsli"
static const float value = 8.f;


[maxvertexcount(4*3)]
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
    float4 tex[4] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
    float4 nowPosition, beforePosition, vec, p0;
    int i = 0;
    GS_OUT gout;

	[unroll]
    for (i = 0; i < 4; i++)
    {
        gout.position = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[i] * input[0].scale.x, projection);
        gout.texcoord = texcoord[i];
        gout.color = input[0].color;
        output.Append(gout);
    }
    [unroll]
    for (i = 0; i < 4; i++)
    {
        gout.position = mul(mul(float4(input[0].worldPosition - input[0].velocity * value * rand_1_normal(input[0].position.xy, 0.5f), 1), view) + texPosition[i] * input[0].scale.x, projection);
        gout.texcoord = texcoord[i];
        gout.color = input[0].color*0.5f;
        output.Append(gout);
    }
    [unroll]
    for (i = 0; i < 2; i++)
    {
        gout.position = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[i*2+1] * input[0].scale.x, projection);
        gout.texcoord = texcoord[i];
        gout.color = input[0].color;
        output.Append(gout);
        
        gout.position = mul(mul(float4(input[0].worldPosition - input[0].velocity * value * rand_1_normal(input[0].position.xy,0.5f), 1), view) + texPosition[i * 2] * input[0].scale.x, projection);
        gout.texcoord = texcoord[i];
        gout.color = input[0].color*0.5f;
        output.Append(gout);

    }
    output.RestartStrip();

}