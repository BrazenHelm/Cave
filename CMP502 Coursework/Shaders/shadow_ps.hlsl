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
	float3 pos_s;
	float3 color_s;
	float3 pos_t;
	float3 color_t;
	float intensity_s;
	float intensity_t;
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

	float3 diffuse_s = { 0.f, 0.f, 0.f };
	float3 specular_s = { 0.f, 0.f, 0.f };
	
	float2 projTexCoord;
	projTexCoord.x = input.lightViewPos.x / input.lightViewPos.w / 2.f + 0.5f;
	projTexCoord.y = -input.lightViewPos.y / input.lightViewPos.w / 2.f + 0.5f;

	if (saturate(projTexCoord.x) == projTexCoord.x &&
		saturate(projTexCoord.y) == projTexCoord.y)
	{
		float depthValue = depthMap.Sample(smplr, projTexCoord).r;
		float lightDepthValue = input.lightViewPos.z / input.lightViewPos.w - bias;

		// if lit by sun
		if (lightDepthValue < depthValue)
		{
			const float3 vecToSun = pos_s - (float3)input.viewPos;
			const float3 dirToSun = vecToSun / length(vecToSun);

			// diffuse light due to sun
			diffuse_s =
				color_s * intensity_s * max(0.f, dot(dirToSun, input.normal));

			const float3 ws = input.normal * dot(vecToSun, input.normal);
			const float3 rs = ws * 2.f - vecToSun;

			// specular light due to sun
			specular_s = color_s * intensity_s * specularIntensity
				* pow(max(0.f, dot(normalize(rs), normalize(-(float3)input.viewPos))),
					specularPower);
		}
	}

	const float3 vecToTorch = pos_t - (float3)input.viewPos;
	const float distToTorch = length(vecToTorch);
	const float3 dirToTorch = vecToTorch / distToTorch;

	const float attenuation_t = 1.f /
		(attConst + attLin * distToTorch + attQuad * distToTorch * distToTorch);

	// diffuse light due to torch
	float3 diffuse_t =
		color_t * intensity_t * attenuation_t * max(0.f, dot(dirToTorch, input.normal));

	const float3 wt = input.normal * dot(vecToTorch, input.normal);
	const float3 rt = wt * 2.f - vecToTorch;

	// specular light due to torch
	float3 specular_t = color_t * intensity_t * attenuation_t * specularIntensity
		* pow(max(0.f, dot(normalize(rt), normalize(-(float3)input.viewPos))),
			specularPower);

	float4 texColor = tex.Sample(smplr, input.tex);

	return float4((ambient + diffuse_s + diffuse_t + specular_s + specular_t)
		* material * texColor.xyz, 1.f);
}