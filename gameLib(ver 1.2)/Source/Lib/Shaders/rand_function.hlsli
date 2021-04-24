float rand_1(in float2 uv, in float a)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * a;
}

float2 rand_2(in float2 uv, in float a) {
	float noiseX = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
	float noiseY = sqrt(1 - noiseX * noiseX);
	return float2(noiseX, noiseY) * a;
}

float rand_1_normal(in float2 uv, in float a)
{
	float2 noise = (frac(sin(dot(normalize(uv), normalize(float2(12.9898, 78.233) * 2.0))) * 43758.5453));
	return abs(noise.x + noise.y) * a;
}

float2 rand_2_normal(in float2 uv, in float a) {
	float noiseX = (frac(sin(dot(normalize(uv), normalize(float2(12.9898, 78.233) * 2.0))) * 43758.5453));
	float noiseY = sqrt(1 - noiseX * noiseX);
	return float2(noiseX, noiseY) * a;
}

