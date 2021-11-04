struct VSOutputType
{
	float4 pos : SV_Position;
	float4 depthPos : DepthPosition;
};

cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

cbuffer LightMatrixBuffer
{
	matrix lightView;
	matrix lightProj;
};

VSOutputType main(float3 pos : Position, float3 n : Normal, float2 tex : TexCoord)
{
	VSOutputType output;
	output.pos = mul(float4(pos, 1.f), world);
	output.pos = mul(output.pos, lightView);
	output.pos = mul(output.pos, lightProj);
	output.depthPos = output.pos;
	return output;
}