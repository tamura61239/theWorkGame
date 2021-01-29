#include"run_particle_cs_function.hlsli"
#include"render_particle_cube.hlsli"

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    #if 1
    Particle p = particle[DTid.x];
    float4 particleColor = (float4) 0;
    p.position += p.velocity * elapsdTime;
    particleColor.r = ((p.color >> 24) & 0x000000FF) / 255.0f;
    particleColor.g = ((p.color >> 16) & 0x000000FF) / 255.0f;
    particleColor.b = ((p.color >> 8) & 0x000000FF) / 255.0f;
    particleColor.a = ((p.color >> 0) & 0x000000FF) / 255.0f;
    p.life -= elapsdTime;
    particleColor.a = p.life;
    p.color |= ((uint) (particleColor.r * 255) & 0x000000FF) << 24;
    p.color |= ((uint) (particleColor.g * 255) & 0x000000FF) << 16;
    p.color |= ((uint) (particleColor.b * 255) & 0x000000FF) << 8;
    p.color |= ((uint) (particleColor.a * 255) & 0x000000FF) << 0;
    float positionW = 1.0f;
    if(p.life<=0)
    {
        p.color = 0;
        p.life = 0;
        p.position = 0;
        p.scale = 0;
        p.velocity = 0;
        positionW = 0;
    }
    particle[DTid.x] = p;
    ParticleRender render = (ParticleRender) 0;
    render.position = float4(p.position, positionW);
    render.scale = p.scale;
    render.color = particleColor;
    render.velocity = p.velocity;
    WriteRender(render, DTid.x * 17 * 4);
#else 
    uint aliveCount = particleCount.Load(0);
    if (aliveCount > DTid.x)
    {
        uint index = particleIndex[DTid.x];
        Particle p = particle[index];
        if (p.life > 0)
        {
            float4 particleColor = (float4) 0;
            p.position += p.velocity * elapsdTime;
            particleColor.r = ((p.color >> 24) & 0x000000FF) / 255.0f;
            particleColor.g = ((p.color >> 16) & 0x000000FF) / 255.0f;
            particleColor.b = ((p.color >> 8) & 0x000000FF) / 255.0f;
            particleColor.a = ((p.color >> 0) & 0x000000FF) / 255.0f;
            p.life -= elapsdTime;
            particleColor.a -= elapsdTime * p.lifeAmoust;
            p.color |= ((uint) (particleColor.r * 255) & 0x000000FF) << 24;
            p.color |= ((uint) (particleColor.g * 255) & 0x000000FF) << 16;
            p.color |= ((uint) (particleColor.b * 255) & 0x000000FF) << 8;
            p.color |= ((uint) (particleColor.a * 255) & 0x000000FF) << 0;

            particle[index] = p;
            //生きてる分カウントを増やす
            uint newAliveCount;
            particleCount.InterlockedAdd(4, 1, newAliveCount);
            particleNewIndex[newAliveCount] = index;
            
            ParticleRender render = (ParticleRender) 0;
            render.position = float4(p.position, 1);
            render.scale = p.scale;
            render.color = particleColor;
            render.velocity = p.velocity;
            WriteRender(render, index * 17 * 4);

        }
        else
        {
            //死んでる分のカウントを増やす
            uint deleteCount;
            particleCount.InterlockedAdd(4 * 2, 1, deleteCount);
            deleteIndex[deleteCount] = index;

        }

    }

#endif
}
