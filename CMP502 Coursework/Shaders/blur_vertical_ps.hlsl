Texture2D tex;
SamplerState splr;


struct PSInputType
{
	float2 uv : Texcoord;
	float4 pos : SV_Position;
};


float4 main(PSInputType input) : SV_Target
{
	uint width;
	uint height;
	tex.GetDimensions(width, height);
	const float dy = 1.f / height;

	float4 acc = float4(0.f, 0.f, 0.f, 0.f);

	int blur_size = 15;

	for (int y = -blur_size; y <= blur_size; y++)
	{
		float2 tc = input.uv + float2(0.f, dy * y);
		acc += tex.Sample(splr, tc).rgba;
	}

	return acc / (blur_size * 2.f + 1.f);

	//acc += tex.Sample(splr, input.uv + float2(0.f, dy * -5)).rgba * 0.000977f;
	//acc += tex.Sample(splr, input.uv + float2(0.f, dy * -4)).rgba * 0.009766f;
	//acc += tex.Sample(splr, input.uv + float2(0.f, dy * -3)).rgba * 0.043945f;
	//acc += tex.Sample(splr, input.uv + float2(0.f, dy * -2)).rgba * 0.117187f;
	//acc += tex.Sample(splr, input.uv + float2(0.f, dy * -1)).rgba * 0.205078f;
	//acc += tex.Sample(splr, input.uv + float2(0.f, dy *  0)).rgba * 0.246094f;
	//acc += tex.Sample(splr, input.uv + float2(0.f, dy *  1)).rgba * 0.205078f;
	//acc += tex.Sample(splr, input.uv + float2(0.f, dy *  2)).rgba * 0.117187f;
	//acc += tex.Sample(splr, input.uv + float2(0.f, dy *  3)).rgba * 0.043945f;
	//acc += tex.Sample(splr, input.uv + float2(0.f, dy *  4)).rgba * 0.009766f;
	//acc += tex.Sample(splr, input.uv + float2(0.f, dy *  5)).rgba * 0.000977f;

	//return acc;
}

