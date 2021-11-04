Texture2D tex		: register(t0);
Texture2D depthMap	: register(t1);
SamplerState smplr	: register(s0);

struct PSInputType
{
	float4 worldPos : WorldPosition;
	float4 viewPos : ViewPosition;
	float3 worldNormal : WorldNormal;
	float3 normal : Normal;
	float4 pos : SV_Position;
	float2 tex : TexCoord;
	float4 lightViewPos : LightView;
};

cbuffer LightBuffer
{
	float3 ambient;
	float3 sunPos;
	float3 sunColor;
	float3 torchPos;
	float3 torchColor;
	float sunIntensity;
	float torchIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer ObjectBuffer
{
	float3 material;
	float specularIntensity;
	float specularPower;
};

float4 main(PSInputType input) : SV_Target
{
	float bias = 0.0001f;

	float2 projTexCoord;
	projTexCoord.x = input.lightViewPos.x / input.lightViewPos.w / 2.f + 0.5f;
	projTexCoord.y = -input.lightViewPos.y / input.lightViewPos.w / 2.f + 0.5f;

	if (saturate(projTexCoord.x) == projTexCoord.x &&
		saturate(projTexCoord.y) == projTexCoord.y)
	{
		float depthValue = depthMap.Sample(smplr, projTexCoord).r;
		float lightDepthValue = input.lightViewPos.z / input.lightViewPos.w - bias;

		if (lightDepthValue < depthValue)
		{
			return float4(sunColor * sunIntensity, 1.f);
		}
	}

	discard;
	return float4(0.f, 0.f, 0.f, 1.f);
}

