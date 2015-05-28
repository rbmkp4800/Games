struct PSInput
{
	float4 pos : SV_Position;
	float2 tex : TEXCOORD0;
	float2 radius : TEXCOORD1;	// x - outer, y - inner
	float4 outerColor : COLOR0;
	float4 innerColor : COLOR1;
};

float4 main(PSInput input) : SV_TARGET
{
	float distance = length(input.tex.xy);
	clip(input.radius.x - distance);
	clip(distance - input.radius.y <= 0.0f ? -1.0f : 1.0f);
	float4 color = lerp(input.innerColor, input.outerColor,
		(distance - input.radius.y) / (input.radius.x - input.radius.y));
	clip(color.w - 0.004f);
	return color;
}