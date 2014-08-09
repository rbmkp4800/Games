cbuffer cbScale : register(b0)
{
	float3x2 transform;
	float2 scale;
}

struct VSInput
{
	float2 pos : POSITION0;
	float4 def : TEXCOORD0;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 def : TEXCOORD0;
};

VSOutput main(VSInput input)
{
	float2 pos = mul(float3(input.pos, 1.0f), transform) * scale + float2(-1.0f, 1.0f);
	VSOutput output = { float4(pos, 0.5f, 1.0f), input.def };
	return output;
}