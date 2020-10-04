#include"select_scene_particle.hlsli"
#include"render_particle_cube.hlsli"
#include"curl_noise.hlsli"
#include"matrix_calculation.hlsli"

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint index = DTid.x + DTid.y + DTid.z;
	uint bufferIndex = index * MAX_INDEX;
	Particle p = (Particle)0;
	//ì«Ç›çûÇ›
	ReadParticle(p, bufferIndex);
	ParticleRender render;

	//çXêV

	p.angle += p.angleMovement * elapsdTime;

	//p.velocity = defVelocity * p.speed;
	//p.centerPosition.xyz += p.velocity * elapsdTime;
	//p.sinAngle += p.sinAngleMovement * elapsdTime;

	//p.position.xyz = p.centerPosition + sin(p.sinAngle) * p.upVec * p.sinLength;
	p.sinLength += p.sinLength * elapsdTime * 0.25f;
	p.sinAngle += p.sinAngleMovement * elapsdTime;
	float3 vec = normalize(endPosition - p.centerPosition);
	float3 vec2 = normalize(defVelocity);
	float3 vec3 = normalize(vec + vec2);

	float3 right = float3(-1, 0, 0);

	float3 up = cross(vec3, right);

	right = cross(vec3, up);

	//p.velocity = right * sin(p.sinAngle)*5;
	p.velocity = normalize(vec3 * p.speed + right * sin(p.sinAngle) * p.sinLength) * p.speed * 2.5f;

	p.centerPosition += vec3 * p.speed * 2.5f * elapsdTime;

	p.position.xyz += p.velocity * elapsdTime;

	p.life -= elapsdTime;
	if (p.life <= 0)
	{
		p.color = float4(0, 0, 0, 0);
		p.life = 0;
		p.position.w = 0;
	}
	render.scale = float3(1, 1, 1) * 0.5f;

	//èëÇ´çûÇ›
	WriteParticle(p, bufferIndex);

	bufferIndex = index * 17 * 4;
	render.position = p.position;
	render.angle = p.angle;
	render.color = p.color;
	render.velocity = p.velocity;
	WriteRender(render, bufferIndex);

}