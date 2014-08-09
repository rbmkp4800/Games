Texture2D textures[4];
SamplerState samplerState : register(s0);

struct PSInput
{
	float4 pos : SV_POSITION;
	float4 def : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
	if (input.def.z < 0)
	{
		int textureIndex = (int)(-input.def.z);
		float4 _sample = float4(0, 0, 0, 0);
		if (textureIndex == 0)
			_sample = textures[0].Sample(samplerState, input.def.xy);
		else if (textureIndex == 1)
			_sample = textures[1].Sample(samplerState, input.def.xy);
		else if (textureIndex == 2)
			_sample = textures[2].Sample(samplerState, input.def.xy);
		else if (textureIndex == 3)
			_sample = textures[3].Sample(samplerState, input.def.xy);
		else
		{
			float r = length(frac(input.def.xy) - float2(0.5f, 0.5f));
			if (r < 0.5f && r >= input.def.w)
				return float4(floor(input.def.xy) / 255.0f, frac(-input.def.z), (floor(-input.def.z) - 4.0f) / 255.0f);
			else
				return float4(0, 0, 0, 0);
		}
		_sample.w *= input.def.w;
		return _sample;
	}

	return input.def;
}