#include"run_particle_cs_function.hlsli"
#include"render_particle_cube.hlsli"

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
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
}