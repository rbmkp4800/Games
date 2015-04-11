Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct PSInput
{
	float4 pos : SV_Position;
	float3 tex : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET
{
	return tex.Sample(samplerState, input.tex.xy) * input.tex.z;
}