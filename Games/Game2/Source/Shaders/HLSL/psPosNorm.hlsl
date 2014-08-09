struct PSInput
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL0;
};

float4 main(PSInput input) : SV_TARGET
{
	float3 v = normalize(float3(1.0f, 1.0f, 0.0f));
	
	float c = dot(input.norm, v);
	return float4(c, c, c, 0.5f);
}