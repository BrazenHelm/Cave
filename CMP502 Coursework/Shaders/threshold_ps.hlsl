Texture2D tex;
SamplerState splr;


struct PSInputType
{
	float2 uv : Texcoord;
	float4 pos : SV_Position;
};


float4 main(PSInputType input) : SV_Target
{
	float threshold = 0.55f;

	float4 color = tex.Sample(splr, input.uv).rgba;
	
	if (color.b < threshold)
	{
		color = float4(0.f, 0.f, 0.f, 1.f);
	}

	return color;
}

