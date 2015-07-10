Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct PSInput
{
	float4 pos : SV_Position;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
	float4 color = input.color * tex.Sample(samplerState, input.tex).x;
	clip(color.w - 0.004f);
	return color;
}