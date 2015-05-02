template <typename ComInterface>
inline void SafeComInterfaceRelease(ComInterface& object)
{
	if (object)
	{
		object->Release();
		object = nullptr;
	}
}

inline D3D11_TEXTURE2D_DESC D3D11Texture2DDesc(UINT width, UINT height, UINT bingFlags,
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT miscFlags = 0, UINT mipLevels = 1,
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuAccessFlags = 0)
{
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = mipLevels;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = usage;
	desc.BindFlags = bingFlags;
	desc.CPUAccessFlags = cpuAccessFlags;
	desc.MiscFlags = miscFlags;
	return desc;
}
inline D3D11_BUFFER_DESC D3D11BufferDesc(UINT size, UINT bindFlags, UINT structureByteStride = 0,
	UINT miscFlags = 0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuAccessFlags = 0)
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
inline D3D11_SUBRESOURCE_DATA D3D11SubresourceData(void *ptr, UINT pitch = 0, UINT slicePitch = 0)
{
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = ptr;
	data.SysMemPitch = pitch;
	data.SysMemSlicePitch = slicePitch;
	return data;
}
inline D3D11_SAMPLER_DESC D3D11SamplerDesc(D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE addressU = D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_MODE addressV = D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_MODE addressW = D3D11_TEXTURE_ADDRESS_CLAMP,
	UINT maxAnisotropy = 0, float *borderColor = nullptr,
	float mipLODBias = 0.0f, D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_ALWAYS,
	float minLOD = 0.0f, float maxLOD = 3.402823466e+38f)
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
inline D3D11_BOX D3D11Box(UINT left, UINT right, UINT top = 0, UINT bottom = 1, UINT front = 0, UINT back = 1)
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
	D3D11_BLEND_DESC desc = { 0 };
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
inline D3D11_RASTERIZER_DESC D3D11RasterizerDesc(D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK,
	bool frontCounterClockwise = false, UINT depthBias = 0, float depthBiasClamp = 0.0f, float slopeScaledDepthBias = 0.0f,
	bool depthClipEnable = true, bool scissorEnable = false, bool multisampleEnable = false, bool antialiasedLineEnable = false)
{
	D3D11_RASTERIZER_DESC desc;
	desc.FillMode = fillMode;
	desc.CullMode = cullMode;
	desc.FrontCounterClockwise = frontCounterClockwise;
	desc.DepthBias = depthBias;
	desc.DepthBiasClamp = depthBiasClamp;
	desc.SlopeScaledDepthBias = slopeScaledDepthBias;
	desc.DepthClipEnable = depthClipEnable;
	desc.ScissorEnable = scissorEnable;
	desc.MultisampleEnable = multisampleEnable;
	desc.AntialiasedLineEnable = antialiasedLineEnable;
	return desc;
}
inline D3D11_SHADER_RESOURCE_VIEW_DESC D3D11ShaderResourceViewDescTexture2D(DXGI_FORMAT format, UINT mostDetailedMip = 0, UINT mipLevels = -1)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.Format = format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MostDetailedMip = mostDetailedMip;
	desc.Texture2D.MipLevels = mipLevels;
	return desc;
}
inline D3D11_SHADER_RESOURCE_VIEW_DESC D3D11ShaderResourceViewDescBuffer(DXGI_FORMAT format, UINT firstElement, UINT numElements)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.Format = format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = firstElement;
	desc.Buffer.NumElements = numElements;
	return desc;
}
inline D3D11_UNORDERED_ACCESS_VIEW_DESC D3D11UnorderedAccessViewDescTexture2D(DXGI_FORMAT format, UINT mipSlice = 0)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	desc.Format = format;
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = mipSlice;
	return desc;
}
inline D3D11_UNORDERED_ACCESS_VIEW_DESC D3D11UnorderedAccessViewDescBuffer(DXGI_FORMAT format, UINT firstElement, UINT numElements, UINT flags)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	desc.Format = format;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = firstElement;
	desc.Buffer.NumElements = numElements;
	desc.Buffer.Flags = flags;
	return desc;
}
inline D3D11_DEPTH_STENCIL_VIEW_DESC D3D11DepthStencilViewDescTexture2D(DXGI_FORMAT format, UINT flags = 0, UINT firstMipSlice = 0)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	desc.Format = format;
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Flags = flags;
	desc.Texture2D.MipSlice = firstMipSlice;
	return desc;
}

template<typename ... srvArgs>
void D3D11ContextPSSetShaderResources(ID3D11DeviceContext* d3dContext, UINT startSlot, srvArgs ... srv)
{
	ID3D11ShaderResourceView *d3dSRV [] = { srv ... };
	d3dContext->PSSetShaderResources(startSlot, elemcntof(d3dSRV), d3dSRV);
}
template<typename ... cbArgs>
void D3D11ContextVSSetConstantBuffers(ID3D11DeviceContext* d3dContext, UINT startSlot, cbArgs ... cb)
{
	ID3D11Buffer *d3dCB [] = { cb ... };
	d3dContext->VSSetConstantBuffers(startSlot, elemcntof(d3dCB), d3dCB);
}
template<typename ... cbArgs>
void D3D11ContextPSSetConstantBuffers(ID3D11DeviceContext* d3dContext, UINT startSlot, cbArgs ... cb)
{
	ID3D11Buffer *d3dCB [] = { cb ... };
	d3dContext->PSSetConstantBuffers(startSlot, elemcntof(d3dCB), d3dCB);
}
inline void D3D11ContextIASetVertexBuffer(ID3D11DeviceContext* d3dContext, UINT slot,
	ID3D11Buffer *d3dBuffer, UINT stride, UINT offset = 0)
{
	return d3dContext->IASetVertexBuffers(slot, 1, &d3dBuffer, (UINT*) &stride, (UINT*) &offset);
}
template<typename ... soArgs>
inline void D3D11ContextSOSetTargets(ID3D11DeviceContext* d3dContext, soArgs ... so)
{
	ID3D11Buffer *d3dSO [] = { so ... };
	UINT offsets[elemcntof_constexpr(d3dSO)] = { 0 };
	d3dContext->SOSetTargets(elemcntof(d3dSO), d3dSO, offsets);
}
template<typename ... uavArgs>
inline void D3D11ContextOMSetRenderTargetAndUnorderedAccessViews(ID3D11DeviceContext* d3dContext,
	ID3D11RenderTargetView* d3dRenderTargetView, ID3D11DepthStencilView *d3dDepthStencilView,
	UINT uavStarSlot, uavArgs ... uav)
{
	ID3D11UnorderedAccessView *d3dUAV [] = { uav ... };
	d3dContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &d3dRenderTargetView, d3dDepthStencilView,
		uavStarSlot, elemcntof(d3dUAV), d3dUAV, nullptr);
}

template<UINT startSlot, UINT numViews>
inline void D3D11ContextClearPSShaderResources(ID3D11DeviceContext* d3dContext)
{
	ID3D11ShaderResourceView *d3dSRV[numViews] = { 0 };
	d3dContext->PSSetShaderResources(startSlot, numViews, d3dSRV);
}