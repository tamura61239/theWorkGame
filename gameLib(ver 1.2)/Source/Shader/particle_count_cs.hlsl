#include"particle_count_buffer.hlsli"
/****************************************************************************/
//�@�@�@�p�[�e�B�N���J�E���g���X�V����
/****************************************************************************/

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint newAliveCount = particleCountBuffer.Load(4);
    
    particleCountBuffer.Store(0, newAliveCount);
    particleCountBuffer.Store(4, 0);
}