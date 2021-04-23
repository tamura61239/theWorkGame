#include"particle_motiom_data_render.hlsli"
#include"before_camera_view_data.hlsli"
#include"rand_function.hlsli"
static const float value = 0.5f;
/*****************************************************************/
//　　　取得した情報を元にテクスチャの4つの頂点を決める
/*****************************************************************/


[maxvertexcount(4)]
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
    float4 textPosition[8] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
    int i = 0;
    GS_OUT gout;

	[unroll]
    for (i = 0; i < 4; i++)
    {
        textPosition[i] = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[i] * input[0].scale.x, projection);
        textPosition[i + 4] = mul(mul(float4(input[0].worldPosition - input[0].velocity * value * rand_1_normal(input[0].position.xy, 0.5f), 1), view) + texPosition[i] * input[0].scale.x, projection);

    }
    float4 leftTop,leftDown, rightTop, rightDown;
    
    leftTop = lerp(textPosition[0], textPosition[4], step(textPosition[4].x, textPosition[0].x));
    leftDown = lerp(textPosition[2], textPosition[6], step(textPosition[2].x, textPosition[6].x));
    
    rightTop = lerp(textPosition[5], textPosition[1], step(textPosition[5].x, textPosition[1].x));
    rightDown = lerp(textPosition[7], textPosition[3], step(textPosition[7].x, textPosition[3].x));
    
    gout.position = leftTop;
    gout.texcoord = texcoord[0];
    gout.color = input[0].color;
    output.Append(gout);
    
    gout.position = rightTop;
    gout.texcoord = texcoord[1];
    gout.color = input[0].color;
    output.Append(gout);

    gout.position = leftDown;
    gout.texcoord = texcoord[2];
    gout.color = input[0].color;
    output.Append(gout);

    gout.position = rightDown;
    gout.texcoord = texcoord[3];
    gout.color = input[0].color;
    output.Append(gout);

    //[unroll]
    //for (i = 0; i < 2; i++)
    //{
    //    gout.position = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[i*2] * input[0].scale.x, projection);
    //    gout.texcoord = texcoord[i];
    //    gout.color = input[0].color;
    //    output.Append(gout);
        
    //    gout.position = mul(mul(float4(input[0].worldPosition - input[0].velocity * value * rand_1_normal(input[0].position.xy,0.5f), 1), view) + texPosition[i * 2+1] * input[0].scale.x, projection);
    //    gout.texcoord = texcoord[i];
    //    gout.color = input[0].color*0.5f;
    //    output.Append(gout);

    //}
    output.RestartStrip();

}