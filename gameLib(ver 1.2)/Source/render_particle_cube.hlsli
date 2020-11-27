struct ParticleRender
{
	float4 position;
	float3 angle;
	float4 color;
	float3 velocity;
	float3 scale;
};
RWByteAddressBuffer renderBuffer:register(u2);

void WriteRender(in ParticleRender render, in uint bufferIndex)
{
	renderBuffer.Store4(bufferIndex, asuint(render.position));
	renderBuffer.Store3(bufferIndex + 4 * 4, asuint(render.angle));
	renderBuffer.Store4(bufferIndex + 7 * 4, asuint(render.color));
	renderBuffer.Store3(bufferIndex + 11 * 4, asuint(render.velocity));
	renderBuffer.Store3(bufferIndex + 14 * 4, asuint(render.scale));
}

void ReadRender(inout ParticleRender render, in uint bufferIndex)
{
	render.position = asfloat(renderBuffer.Load4(bufferIndex));
	render.angle = asfloat(renderBuffer.Load3(bufferIndex + 4 * 4));
	render.color = asfloat(renderBuffer.Load4(bufferIndex + 7 * 4));
	render.velocity = asfloat(renderBuffer.Load3(bufferIndex + 11 * 4));
	render.scale = asfloat(renderBuffer.Load3(bufferIndex + 14 * 4));
}