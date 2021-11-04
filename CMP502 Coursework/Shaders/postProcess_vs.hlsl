struct VSOutputType
{
	float2 uv : Texcoord;
	float4 pos : SV_Position;
};


VSOutputType main(float2 pos : Position2D)
{
	VSOutputType output;
	output.pos = float4(pos, 0.f, 1.f);
	output.uv = float2((pos.x + 1.f) / 2.f, -(pos.y - 1.f) / 2.f);
	return output;
}
