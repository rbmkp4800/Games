struct PSOutput
{
	float3 target : SV_TARGET;
	float depth : SV_DEPTH;
};

PSOutput main()
{
	PSOutput output = { float3(1.0f, 1.0f, 1.0f), 1.0f };
	return output;
}