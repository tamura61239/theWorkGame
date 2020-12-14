#include "model_motion_data.hlsli"

[maxvertexcount(3 * 8)]
void main(
	triangle VS_OUT input[3],
	inout TriangleStream<GS_OUT> output
)
{
    GS_OUT gout = (GS_OUT) 0;
    float4 nowPosition, beforePosition;
    int i = 0;
    [unroll]
    for (i = 0; i < 3; i++)
    {
        nowPosition = input[i].position;
        beforePosition = input[i].beforePosition;
        gout.position = nowPosition;
        gout.velocity.xy = nowPosition.xy / nowPosition.w - beforePosition.xy / beforePosition.w;
        gout.velocity.z = gout.position.z / gout.position.w;
        output.Append(gout);
    }
    
    [unroll]
    for (i = 0; i < 3; i++)
    {
        int id0 = i % 3,
        id1 = (i + 1) % 3;
        {
            nowPosition = input[id0].position;
            beforePosition = input[id0].beforePosition;
            gout.position = nowPosition;
            gout.velocity.xy = nowPosition.xy / nowPosition.w - beforePosition.xy / beforePosition.w;
            gout.velocity.z = gout.position.z / gout.position.w;
            output.Append(gout);

            nowPosition = input[id0].position;
            beforePosition = input[id0].beforePosition;
            gout.position = beforePosition;
            gout.velocity.xy = nowPosition.xy / nowPosition.w - beforePosition.xy / beforePosition.w;
            gout.velocity.z = gout.position.z / gout.position.w;
            output.Append(gout);

            nowPosition = input[id1].position;
            beforePosition = input[id1].beforePosition;
            gout.position = nowPosition;
            gout.velocity.xy = nowPosition.xy / nowPosition.w - beforePosition.xy / beforePosition.w;
            gout.velocity.z = gout.position.z / gout.position.w;
            output.Append(gout);

        }
        {
            nowPosition = input[id0].position;
            beforePosition = input[id0].beforePosition;
            gout.position = beforePosition;
            gout.velocity.xy = nowPosition.xy / nowPosition.w - beforePosition.xy / beforePosition.w;
            gout.velocity.z = gout.position.z / gout.position.w;
            output.Append(gout);

            nowPosition = input[id1].position;
            beforePosition = input[id1].beforePosition;
            gout.position = beforePosition;
            gout.velocity.xy = nowPosition.xy / nowPosition.w - beforePosition.xy / beforePosition.w;
            gout.velocity.z = gout.position.z / gout.position.w;
            output.Append(gout);

            nowPosition = input[id1].position;
            beforePosition = input[id1].beforePosition;
            gout.position = nowPosition;
            gout.velocity.xy = nowPosition.xy / nowPosition.w - beforePosition.xy / beforePosition.w;
            gout.velocity.z = gout.position.z / gout.position.w;
            output.Append(gout);

        }
    }
    
    [unroll]
    for (i = 0; i < 3; i++)
    {
        nowPosition = input[i].position;
        beforePosition = input[i].beforePosition;
        gout.position = beforePosition;
        gout.velocity.xy = nowPosition.xy / nowPosition.w - beforePosition.xy / beforePosition.w;
        gout.velocity.z = gout.position.z / gout.position.w;
        output.Append(gout);

    }

    output.RestartStrip();

}