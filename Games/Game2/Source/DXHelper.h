namespace DXHelper
{
	inline D3D11_TEXTURE2D_DESC D3D11Texture2DDesc(uint32 width, uint32 height, uint32 bingFlags,
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, uint32 cpuAccessFlags = 0)
	{
		D3D11_TEXTURE2D_DESC desc = {0};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.Usage = usage;
		desc.BindFlags = bingFlags;
		desc.CPUAccessFlags = cpuAccessFlags;
		desc.MiscFlags = 0;
		return desc;
	}
	inline D3D11_BUFFER_DESC D3D11BufferDesc(uint32 size, uint32 bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
		uint32 cpuAccessFlags = 0, uint32 miscFlags = 0, uint32 structureByteStride = 0)
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = size;
		desc.Usage = usage;
		desc.BindFlags = bindFlags;
		desc.CPUAccessFlags = cpuAccessFlags;
		desc.MiscFlags = miscFlags;
		desc.StructureByteStride = structureByteStride;
		return desc;
	}
	inline D3D11_SUBRESOURCE_DATA D3D11SubresourceData(void *ptr, uint32 pitch = 0, uint32 slicePitch = 0)
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = ptr;
		data.SysMemPitch = pitch;
		data.SysMemSlicePitch = slicePitch;
		return data;
	}
	inline D3D11_SAMPLER_DESC D3D11SamplerDesc(D3D11_FILTER filter, 
		D3D11_TEXTURE_ADDRESS_MODE addressU = D3D11_TEXTURE_ADDRESS_BORDER,
		D3D11_TEXTURE_ADDRESS_MODE addressV = D3D11_TEXTURE_ADDRESS_BORDER,
		D3D11_TEXTURE_ADDRESS_MODE addressW = D3D11_TEXTURE_ADDRESS_BORDER,
		uint32 maxAnisotropy = 0, float *borderColor = nullptr,
		float mipLODBias = 0.0f, D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_ALWAYS,
		float minLOD = 0.0f, float maxLOD = 3.402823466e+38F)
	{
		D3D11_SAMPLER_DESC desc;
		desc.Filter = filter;
		desc.AddressU = addressU;
		desc.AddressV = addressV;
		desc.AddressW = addressW;
		desc.MipLODBias = mipLODBias;
		desc.MaxAnisotropy = maxAnisotropy;
		desc.ComparisonFunc = comparisonFunc;
		if (borderColor)
			memcpy(desc.BorderColor, borderColor, sizeof(float) * 4);
		else
		{
			desc.BorderColor[0] = 0.0f;
			desc.BorderColor[1] = 0.0f;
			desc.BorderColor[2] = 0.0f;
			desc.BorderColor[3] = 0.0f;
		}
		desc.MinLOD = minLOD;
		desc.MaxLOD = maxLOD;
		return desc;
	}
	inline D3D11_VIEWPORT D3D11ViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f)
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = topLeftX;
		viewport.TopLeftY = topLeftY;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = minDepth;
		viewport.MaxDepth = maxDepth;
		return viewport;
	}
	inline D3D11_BOX D3D11Box(uint32 left, uint32 right, uint32 top = 0, uint32 bottom = 1, uint32 front = 0, uint32 back = 1)
	{
		D3D11_BOX box;
		box.left = left;
		box.top = top;
		box.front = front;
		box.right = right;
		box.bottom = bottom;
		box.back = back;
		return box;
	}
	inline D3D11_BLEND_DESC D3D11BlendDesc(D3D11_BLEND scrBlend, D3D11_BLEND_OP blendOp, D3D11_BLEND dstBlend,
		D3D11_BLEND scrBlendA, D3D11_BLEND_OP blendOpA, D3D11_BLEND dstBlendA)
	{
		D3D11_BLEND_DESC desc = {0};
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		//finalColor.rgb = (srcColor.rgb * SrcBlend) (BlendOp) (destColor.rgb * DestBlend);
		//finalColor.a = (srcColor.a * SrcBlendAlpha) (BlendOpAlpha) (destColor.a * DestBlendAlpha);

		desc.RenderTarget[0].SrcBlend = scrBlend;
		desc.RenderTarget[0].BlendOp = blendOp;
		desc.RenderTarget[0].DestBlend = dstBlend;

		desc.RenderTarget[0].SrcBlendAlpha = scrBlendA;
		desc.RenderTarget[0].BlendOpAlpha = blendOpA;
		desc.RenderTarget[0].DestBlendAlpha = dstBlendA;

		return desc;
	}
	inline DXGI_SWAP_CHAIN_DESC1 DXGISwapChainDesc1(uint16x2 size, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL, uint32 bufferCount = 1, DXGI_SCALING scaling = DXGI_SCALING_STRETCH)
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {0};
		desc.Width = size.x;
		desc.Height = size.y;
		desc.Format = format;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = bufferCount;
		desc.Scaling = scaling;
		desc.SwapEffect = swapEffect;
		desc.Flags = 0;
		return desc;
	}
}