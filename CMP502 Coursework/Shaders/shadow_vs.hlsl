struct VSOutputType
{
	float4 worldPos : WorldPosition;
	float4 viewPos : ViewPosition;
	float3 worldNormal : WorldNormal;
	float3 normal : Normal;
	float4 pos : SV_Position;
	float2 tex : TexCoord;
	float4 lightViewPos : LightView;
};

cbuffer MatrixBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix proj;
};

cbuffer LightMatrixBuffer : register (b1)
{
	matrix lightView;
	matrix lightProj;
};

VSOutputType main(float3 pos : Position, float3 n : Normal, float2 tex : TexCoord)
{
	VSOutputType output;

	output.worldPos = mul(float4(pos, 1.f), world);
	output.viewPos = mul(output.worldPos, view);
	output.pos = mul(output.viewPos, proj);

	output.normal = mul(n, (float3x3)world);
	output.normal = mul(output.normal, (float3x3)view);
	output.normal = normalize(output.normal);

	output.worldNormal = mul(n, (float3x3)world);
	output.worldNormal = normalize(output.worldNormal);

	output.tex = tex;

	output.lightViewPos = mul(output.worldPos, lightView);
	output.lightViewPos = mul(output.lightViewPos, lightProj);

	return output;
}