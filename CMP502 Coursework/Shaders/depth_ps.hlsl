struct PSInputType
{
	float4 pos : SV_Position;
	float4 depthPos : DepthPosition;
};

float4 main(PSInputType input) : SV_Target
{
	float depthValue = input.depthPos.z / input.depthPos.w;
	return float4(depthValue, depthValue, depthValue, 1.f);
}

