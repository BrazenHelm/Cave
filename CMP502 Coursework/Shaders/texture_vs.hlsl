struct VSOutputType
{
	float2 tex : TexCoord;
	float4 pos : SV_Position;
};

cbuffer MatrixBuffer
{
	matrix world;
	matrix viewProj;
};

VSOutputType main(float3 pos : Position, float2 tex : TexCoord)
{
	VSOutputType output;
	output.pos = mul(float4(pos, 1.f), viewProj);
	output.tex = tex;
	return output;
}