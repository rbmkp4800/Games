struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
	float c = 1.0f - input.pos.z * input.pos.z * input.pos.z;
	return float4(c, c, c, 1.0f);
}