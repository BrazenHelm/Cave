Texture2D tex;
SamplerState splr;


struct PSInputType
{
	float2 uv : Texcoord;
	float4 pos : SV_Position;
};


float4 main(PSInputType input) : SV_Target
{
	// DO NOTHING
	float4 texColor = tex.Sample(splr, input.uv);
	return texColor;	
}

