#include"select_scene_particle.hlsli"
#include"rand_function.hlsli"
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//uint index = startIndex + DTid.x;
	//uint bufferIndex = index * MAX_INDEX;

	//Particle p = (Particle)0;
	//p.velocity = (float3)0;
	//float2 bR = float2(bufferIndex % 591, bufferIndex % 256);
	//float2 r = float2(index % 591, index % 256);
	//float x = (rand_1_normal(float2(bufferIndex % 591, bufferIndex % 256), 0.34f) + rand_1_normal(float2(index % 591, index % 256),0.34f)) * scope.x;
	//float y = (rand_1_normal(float2(bufferIndex % 777, bufferIndex % 444), 0.34f) + rand_1_normal(float2(index % 777, index % 444),0.34f)) * scope.y;
	//float2 xy = float2(x, y);
	//float z = rand_1_normal(bR, 0.5f) + rand_1_normal(r, 0.5f) * scope.z;
	//xy = xy*2.f - scope.xy;
	//z = z * 2.f - scope.z;
	//p.position.xy = xy * range;
	//p.position.z = z * range;
	//p.position.w = 1.0f; 
	//p.angle = (float3)0;
	//p.angleMovement = angleMovement * rand_1_normal(float2((bufferIndex + index) % 652, (bufferIndex - index) % 311), 1);
	//p.speed = speed + (rand_1_normal(float2((bufferIndex + index) % 652, (bufferIndex - index) % 311), 1) * 0.5f * speed);
	//p.life = 8;
	//p.color = color * (rand_1_normal(float2((bufferIndex + index) % 546, (bufferIndex - index) % 825), 1));
	//p.centerPosition = p.position.xyz;
	//p.sinAngle = (rand_1_normal(float2(bufferIndex % 712, bufferIndex % 583), 0.2f) - 1) * 3.14f;
	//p.sinAngleMovement = (rand_1_normal(float2(bufferIndex % 645, bufferIndex % 377), 0.2f) - 1) * 3.14f * 0.75f;


	//p.sinLength = 10 + (rand_1_normal(float2(bufferIndex % 213, bufferIndex % 491), 1) - 1);
	//WriteParticle(p, bufferIndex);
    uint deleteCount;
    particleCountBuffer.InterlockedAdd(8, -1, deleteCount);
	
    uint newParticleIndex = deleteIndexBuffer[deleteCount - 1];
    Particle particle = (Particle) 0;
	
    float2 bR = float2(newParticleIndex % 591, newParticleIndex % 256);
    float2 r = float2(deleteCount % 591, deleteCount % 256);
    float x = (rand_1_normal(float2(newParticleIndex * 54 % 591, deleteCount * 54 % 256), 0.34f) + rand_1_normal(float2(deleteCount % 591, deleteCount % 256), 0.34f)) * scope.x;
    float y = (rand_1_normal(float2(deleteCount * 54 % 777, newParticleIndex * 54 % 444), 0.34f) + rand_1_normal(float2(deleteCount % 777, deleteCount % 444), 0.34f)) * scope.y;
    float2 xy = float2(x, y);
    float z = rand_1_normal(bR, 0.5f) + rand_1_normal(r, 0.5f) * scope.z;
    xy = xy * 2.f - scope.xy;
    z = z * 2.f - scope.z;
	
    particle.position.xy = xy * range;
    particle.position.z = z * range;
    particle.position.w = 1.0f;
    particle.angle = (float3) 0;
    particle.angleMovement = angleMovement * rand_1_normal(float2((newParticleIndex + deleteCount) % 652, (newParticleIndex - deleteCount) % 311), 1);
    particle.speed = speed + (rand_1_normal(float2((newParticleIndex + deleteCount) % 652, (newParticleIndex - deleteCount) % 311), 1) * 0.5f * speed);
    particle.life = 8;
    particle.color = color * (rand_1_normal(float2((newParticleIndex + deleteCount) % 546, (newParticleIndex - deleteCount) % 825), 1));
    particle.centerPosition = particle.position.xyz;
    particle.sinAngle = (rand_1_normal(float2(newParticleIndex % 712, newParticleIndex % 583), 0.2f) - 1) * 3.14f;
    particle.sinAngleMovement = (rand_1_normal(float2(newParticleIndex % 645, newParticleIndex % 377), 0.2f) - 1) * 3.14f * 0.75f;
    particle.sinLength = 10 + (rand_1_normal(float2(newParticleIndex % 213, newParticleIndex % 491), 1) - 1);
    particleBuffer[newParticleIndex] = particle;
	
    uint particleCont;
    particleCountBuffer.InterlockedAdd(0, 1, particleCont);
    indexBuffer.Store(particleCont * 4, newParticleIndex);
	
	
	
	
	

}