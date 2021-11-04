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
	const float dx = 1.f / width;

	float4 acc = float4(0.f, 0.f, 0.f, 0.f);

	int blur_size = 15;

	for (int x = -blur_size; x <= blur_size; x++)
	{
		float2 tc = input.uv + float2(dx * x, 0.f);
		acc += tex.Sample(splr, tc).rgba;
	}
	
	return acc / (blur_size * 2.f + 1.f);


	//acc += tex.Sample(splr, input.uv + float2(dx * -5, 0.f)).rgba * 0.000977f;
	//acc += tex.Sample(splr, input.uv + float2(dx * -4, 0.f)).rgba * 0.009766f;
	//acc += tex.Sample(splr, input.uv + float2(dx * -3, 0.f)).rgba * 0.043945f;
	//acc += tex.Sample(splr, input.uv + float2(dx * -2, 0.f)).rgba * 0.117187f;
	//acc += tex.Sample(splr, input.uv + float2(dx * -1, 0.f)).rgba * 0.205078f;
	//acc += tex.Sample(splr, input.uv + float2(dx *  0, 0.f)).rgba * 0.246094f;
	//acc += tex.Sample(splr, input.uv + float2(dx *  1, 0.f)).rgba * 0.205078f;
	//acc += tex.Sample(splr, input.uv + float2(dx *  2, 0.f)).rgba * 0.117187f;
	//acc += tex.Sample(splr, input.uv + float2(dx *  3, 0.f)).rgba * 0.043945f;
	//acc += tex.Sample(splr, input.uv + float2(dx *  4, 0.f)).rgba * 0.009766f;
	//acc += tex.Sample(splr, input.uv + float2(dx *  5, 0.f)).rgba * 0.000977f;

	//return acc;
}