struct PSInput
{
	float4 pos : SV_Position;
	float4 color : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
	clip(input.color.w - 0.004f);
	return input.color;
}