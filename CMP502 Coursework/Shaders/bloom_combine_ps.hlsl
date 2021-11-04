Texture2D sceneTex	: register (t0);
Texture2D blurTex1	: register (t1);
Texture2D blurTex2	: register (t2);
Texture2D blurTex3	: register (t3);
SamplerState splr;


struct PSInputType
{
	float2 uv : Texcoord;
	float4 pos : SV_Position;
};


float4 AdjustSaturation(float4 color, float saturation)
{
	float grey = dot(color.rgb, float3(0.30f, 0.59f, 0.11f));
	return lerp(grey, color, saturation);
}


float4 main(PSInputType input) : SV_Target
{
	float4 bloomSaturation = 1.f;
	float4 baseSaturation = 1.f;

	float4 bloomIntensity = 0.7f;
	float4 baseIntensity = 1.f;

	float4 sceneColor = sceneTex.Sample(splr, input.uv);
	float4 blurColor1 = blurTex1.Sample(splr, input.uv);
	float4 blurColor2 = blurTex2.Sample(splr, input.uv);
	float4 blurColor3 = blurTex3.Sample(splr, input.uv);

	return saturate(
		sceneColor * baseIntensity
		+ (blurColor1 + blurColor2 + blurColor3) * bloomIntensity / 3.f);

}