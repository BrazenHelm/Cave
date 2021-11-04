Texture2D tex		: register(t0);
Texture2D depthMap	: register(t1);
SamplerState smplr	: register(s0);

struct PSInputType
{
	float4 worldPos : WorldPosition;
	float4 viewPos : ViewPosition;
	float3 normal : Normal;
	float4 pos : SV_Position;
};

cbuffer LightBuffer
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
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
	const float3 vecToLight = lightPos - (float3)input.viewPos;
	const float distToLight = length(vecToLight);
	const float3 dirToLight = vecToLight / distToLight;

	const float att = 1.f /
		(attConst + attLin * distToLight + attQuad * (distToLight * distToLight));

	const float3 diffuse =
		diffuseColor * diffuseIntensity * att * max(0.f, dot(dirToLight, input.normal));

	const float3 w = input.normal * dot(vecToLight, input.normal);
	const float3 r = w * 2.f - vecToLight;

	const float3 specular =
		diffuseColor * diffuseIntensity * att * specularIntensity
		* pow(max(0.f, dot(normalize(-r), normalize((float3)input.viewPos))), specularPower);

	return float4(saturate(diffuse + ambient + specular) * material, 1.f);
}

