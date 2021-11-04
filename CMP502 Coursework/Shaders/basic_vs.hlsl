struct VSOutputType
{
	float4 pos : SV_Position;
};

cbuffer MatrixBuffer
{
	matrix world;
	matrix viewProj;
};

VSOutputType main(float3 pos : Position)
{
	VSOutputType output;
	output.pos = mul(float4(pos, 1.f), viewProj);
	return output;
}