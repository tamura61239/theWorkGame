#include"particle_count_buffer.hlsli"
/****************************************************************************/
//　　　パーティクルカウントを初期化する
/****************************************************************************/

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint count = particleCountBuffer.Load(0);
    uint deleteCount = particleCountBuffer.Load(8) + count;
    particleCountBuffer.Store(0, 0);
    particleCountBuffer.Store(8, deleteCount);
}