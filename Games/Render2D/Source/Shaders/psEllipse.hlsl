struct PSInput
{
	float4 pos : SV_Position;
	float3 tex : TEXCOORD;
	float4 color : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
	float radius = length(input.tex.xy);
	clip(1.0f - radius);
	clip(radius - input.tex.z);
	return input.color;
}