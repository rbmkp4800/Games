cbuffer cbScale : register(b0)
{
	float3x2 transform;
}

struct VSInput
{
	float2 pos : POSITION;
	float2 tex : TEXCOORD0;
	float2 radius : TEXCOORD1;
	float4 outerColor : COLOR0;
	float4 innerColor : COLOR1;
};

struct VSOutput
{
	float4 pos : SV_Position;
	float2 tex : TEXCOORD0;
	float2 radius : TEXCOORD1;
	float4 outerColor : COLOR0;
	float4 innerColor : COLOR1;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.pos = float4(mul(float3(input.pos, 1.0f), transform), 0.5f, 1.0f);
	output.tex = input.tex;
	output.radius = input.radius;
	output.outerColor = input.outerColor;
	output.innerColor = input.innerColor;
	return output;
}