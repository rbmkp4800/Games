#include "vsGlobal.hlsli"

struct VSInput
{
	float3 pos : POSITION0;
	float3 norm : NORMAL0;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL0;
};

VSOutput main(VSInput input)
{
	VSOutput output = { mul(transform, float4(input.pos, 1.0f)), input.norm };
	return output;
}