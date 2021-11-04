struct VSOutputType
{
	float4 worldPos : WorldPosition;
	float4 viewPos : ViewPosition;
	float3 normal : Normal;
	float4 pos : SV_Position;
};

cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

VSOutputType main(float3 pos : Position, float3 n : Normal, float2 tex : TexCoord)
{
	VSOutputType output;
	output.worldPos = mul(float4(pos, 1.f), world);
	output.viewPos = mul(output.worldPos, view);
	output.normal = mul(n, (float3x3)world);
	output.normal = mul(output.normal, (float3x3)view);
	output.pos = mul(output.viewPos, proj);
	return output;
}

