RWByteAddressBuffer particleCount : register(u1);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint newAliveCount = particleCount.Load(4);
    
    particleCount.Store(0, newAliveCount);
    particleCount.Store(4, 0);
}