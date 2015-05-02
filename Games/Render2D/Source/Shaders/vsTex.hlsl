cbuffer cbScale : register(b0)
{
	float3x2 transform;
}

struct VSInput
{
	float2 pos : POSITION;
	float3 tex : TEXCOORD;
};

struct VSOutput
{
	float4 pos : SV_Position;
	float3 tex : TEXCOORD;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.pos = float4(mul(float3(input.pos, 1.0f), transform), 0.5f, 1.0f);
	output.tex = input.tex;
	return output;
}