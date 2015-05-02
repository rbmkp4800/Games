cbuffer cbScale : register(b0)
{
	float3x2 transform;
}

struct VSInput
{
	float2 pos : POSITION;
	float4 color : COLOR;
};

struct VSOutput
{
	float4 pos : SV_Position;
	float4 color : COLOR;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.pos = float4(mul(float3(input.pos, 1.0f), transform), 0.5f, 1.0f);
	output.color = input.color;
	return output;
}