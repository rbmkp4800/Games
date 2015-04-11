inline DXGI_SWAP_CHAIN_DESC1 DXGISwapChainDesc1(UINT x, UINT y, DXGI_USAGE usage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL,
	UINT bufferCount = 1, DXGI_SCALING scaling = DXGI_SCALING_STRETCH)
{
	DXGI_SWAP_CHAIN_DESC1 desc = { 0 };
	desc.Width = x;
	desc.Height = y;
	desc.Format = format;
	desc.Stereo = false;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = usage;
	desc.BufferCount = bufferCount;
	desc.Scaling = scaling;
	desc.SwapEffect = swapEffect;
	desc.Flags = 0;
	return desc;
}