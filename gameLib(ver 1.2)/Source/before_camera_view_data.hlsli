cbuffer CbBeforeCamera : register(b5)
{
    row_major float4x4 beforeView;
    row_major float4x4 beforeProjection;
}