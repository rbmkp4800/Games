cbuffer cbScale : register(b0)
{
	float3x2 transform;
}

struct VSInput
{
	float2 pos : POSITION;
	float3 tex : TEXCOORD;
	float4 color : COLOR;
};

struct VSOutput
{
	float4 pos : SV_Position;
	float3 tex : TEXCOORD;
	float4 color : COLOR;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.pos = float4(mul(float3(input.pos, 1.0f), transform) / 2.0f + float2(0.5f, 0.5f), 0.5f, 1.0f);
	output.tex = input.tex;
	output.color = input.color;
	return output;
}