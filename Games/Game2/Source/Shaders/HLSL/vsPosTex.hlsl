#include "vsGlobal.hlsli"

struct VSInput
{
	float3 pos : POSITION0;
	float2 tex : TEXCOORD0;
};
struct VSOutput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

VSOutput main(VSInput input)
{
	VSOutput output = { mul(transform, float4(input.pos, 1.0f)), input.tex };
	return output;
}