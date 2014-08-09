#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <windows.ui.xaml.media.dxinterop.h>
#include "LodePNG\lodepng.h"
#include "DXHelper.h"

#include "Render2D.h"

#include "Shaders\vsBasic.csch"
#include "Shaders\psBasic.csch"

#define TEXTURE_FORMAT	DXGI_FORMAT_R8G8B8A8_UNORM

using namespace Render2D;
using namespace DXHelper;

Render::DXResources Render::dx = { 0 };
Render::VSCBTransform Render::vscbTransform;

VertexBasic* Render::vertexBuffer = nullptr;
uint32 Render::vertexCount = 0;
ID3D11RenderTargetView *Render::d3dRenderTargetView = nullptr;
ID3D11ShaderResourceView *Render::d3dTextureSlotsSRVBuffer[Render::textureSlotsCount] = { 0 };
bool Render::indexationEnabled = true;

void Render::Init()
{
	D3D_FEATURE_LEVEL currentFeatureLevel, featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		//D3D_FEATURE_LEVEL_9_2,
		//D3D_FEATURE_LEVEL_9_1,
	};
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
#ifdef _DEBUG
		D3D11_CREATE_DEVICE_DEBUG |
#endif
		D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevels, elemcntof(featureLevels), D3D11_SDK_VERSION,
		(ID3D11Device**) &dx.d3dDevice, &currentFeatureLevel, (ID3D11DeviceContext**) &dx.d3dContext);
	dx.d3dDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**) &dx.dxgiDevice);
	dx.dxgiDevice->SetMaximumFrameLatency(1);
	CreateDXGIFactory1(__uuidof(*dx.dxgiFactory), (void**) &dx.dxgiFactory);

	D3D11_INPUT_ELEMENT_DESC vertexBasicDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	dx.d3dDevice->CreateInputLayout(vertexBasicDesc, elemcntof(vertexBasicDesc), vsBasicData, sizeof(vsBasicData), &dx.d3dILBasic);
	dx.d3dDevice->CreateVertexShader(vsBasicData, sizeof(vsBasicData), nullptr, &dx.d3dVSBasic);
	dx.d3dDevice->CreatePixelShader(psBasicData, sizeof(psBasicData), nullptr, &dx.d3dPSBasic);

	uint16x3 indexBuffer[indexedPrimitivesLimit];
	for (uint16 i = 0; i < indexedPrimitivesLimit / 2; i++)
	{
		indexBuffer[i * 2].set(i * 4, i * 4 + 1, i * 4 + 3);
		indexBuffer[i * 2 + 1].set(i * 4 + 1, i * 4 + 2, i * 4 + 3);
	}
	dx.d3dDevice->CreateBuffer(&D3D11BufferDesc(sizeof(indexBuffer), D3D11_BIND_INDEX_BUFFER),
		&D3D11SubresourceData(indexBuffer), &dx.d3dIndexBuffer);
	dx.d3dDevice->CreateBuffer(&D3D11BufferDesc(vertexBufferSize, D3D11_BIND_VERTEX_BUFFER), nullptr, &dx.d3dVertexBuffer);
	dx.d3dDevice->CreateBuffer(&D3D11BufferDesc(sizeof(VSCBTransform), D3D11_BIND_CONSTANT_BUFFER), nullptr, &dx.d3dVSCBTransform);

	dx.d3dDevice->CreateRasterizerState(&D3D11RasterizerDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE), &dx.d3dRasterizerState);
	dx.d3dDevice->CreateSamplerState(&D3D11SamplerDesc(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP), &dx.d3dSamplerStateLinearWrap);
	dx.d3dDevice->CreateSamplerState(&D3D11SamplerDesc(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MIRROR_ONCE, D3D11_TEXTURE_ADDRESS_MIRROR_ONCE, D3D11_TEXTURE_ADDRESS_MIRROR_ONCE), &dx.d3dSamplerStateLinearMirrorOnce);
	dx.d3dDevice->CreateBlendState(&D3D11BlendDesc(
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_OP_ADD, D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE), &dx.d3dBlendStateAlpha);
	dx.d3dDevice->CreateBlendState(&D3D11BlendDesc(
		D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_ZERO,
		D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_ZERO), &dx.d3dBlendStateClear);

	dx.d3dContext->IASetIndexBuffer(dx.d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	dx.d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dx.d3dContext->VSSetConstantBuffers(0, 1, &dx.d3dVSCBTransform);
	dx.d3dContext->RSSetState(dx.d3dRasterizerState);
	SetSamplerMode(SamplerMode::Default);
	SetBlendState(BlendState::Default);

	vertexBuffer = (VertexBasic*) malloc(vertexBufferSize);
}

inline Render::VSCBTransform::VSCBTransform()
{
	setTransformMatrix(matrix3x2::identity());
	_padding1 = 0xffffffff;
	_padding2 = 0xffffffff;
	_padding3 = 0xffffffff;
	_padding4 = 0xffffffff;
	scale.set(0.0f, 0.0f);
}
inline void Render::VSCBTransform::setTransformMatrix(const matrix3x2& matrix)
{
	transformMatrixRow1.set(matrix.data[0][0], matrix.data[1][0], matrix.data[2][0]);
	transformMatrixRow2.set(matrix.data[0][1], matrix.data[1][1], matrix.data[2][1]);
}
inline void Render::setDeviceStates(ID3D11InputLayout *_d3dInputLayout, ID3D11VertexShader *_d3dVertexShader,
	ID3D11PixelShader *_d3dPixelShader, uint32 _vertexBufferStride)
{
	static ID3D11InputLayout *d3dInputLayout = nullptr;
	static ID3D11VertexShader *d3dVertexShader = nullptr;
	static ID3D11PixelShader *d3dPixelShader = nullptr;
	static uint32 vertexBufferStride = 0;

	if (d3dInputLayout != _d3dInputLayout)
	{
		d3dInputLayout = _d3dInputLayout;
		dx.d3dContext->IASetInputLayout(d3dInputLayout);
	}
	if (d3dVertexShader != _d3dVertexShader)
	{
		d3dVertexShader = _d3dVertexShader;
		dx.d3dContext->VSSetShader(d3dVertexShader, nullptr, 0);
	}
	if (d3dPixelShader != _d3dPixelShader)
	{
		d3dPixelShader = _d3dPixelShader;
		dx.d3dContext->PSSetShader(d3dPixelShader, nullptr, 0);
	}
	if (vertexBufferStride != _vertexBufferStride)
	{
		vertexBufferStride = _vertexBufferStride;
		uint32 offset = 0;
		dx.d3dContext->IASetVertexBuffers(0, 1, &dx.d3dVertexBuffer, &vertexBufferStride, &offset);
	}
}

void Render::Flush()
{
	if (!vertexCount)
		return;

	setDeviceStates(dx.d3dILBasic, dx.d3dVSBasic, dx.d3dPSBasic, sizeof(VertexBasic));
	if (d3dTextureSlotsSRVBuffer[0])
		dx.d3dContext->PSSetShaderResources(0, textureSlotsCount, d3dTextureSlotsSRVBuffer);
	dx.d3dContext->UpdateSubresource(dx.d3dVertexBuffer, 0, &DXHelper::D3D11Box(0, vertexCount * sizeof(VertexBasic)), vertexBuffer, 0, 0);
	if (indexationEnabled)
		dx.d3dContext->DrawIndexed(vertexCount / 4 * 6, 0, 0);
	else
		dx.d3dContext->Draw(vertexCount, 0);
	Discard();
}
void Render::Discard()
{
	vertexCount = 0;
	memset(d3dTextureSlotsSRVBuffer, 0, sizeof(d3dTextureSlotsSRVBuffer));
}

void Render::SetTarget(IRenderTarget *target)
{
	if (d3dRenderTargetView == target->d3dRenderTargetView)
		return;

	Flush();
	dx.d3dContext->PSSetShaderResources(0, textureSlotsCount, d3dTextureSlotsSRVBuffer);
	d3dRenderTargetView = target->d3dRenderTargetView;
	uint32x2 targetSize = target->GetSize();
	vscbTransform.scale.set(2.0f / targetSize.x, -2.0f / targetSize.y);
	dx.d3dContext->OMSetRenderTargets(1, &target->d3dRenderTargetView, nullptr);
	dx.d3dContext->RSSetViewports(1, &DXHelper::D3D11ViewPort(0.0f, 0.0f, (float) targetSize.x, (float) targetSize.y));
	dx.d3dContext->UpdateSubresource(dx.d3dVSCBTransform, 0, nullptr, &vscbTransform, 0, 0);
}
void Render::ResetTarget()
{
	Flush();
	d3dRenderTargetView = nullptr;
	dx.d3dContext->OMSetRenderTargets(1, &d3dRenderTargetView, nullptr);
}
void Render::Clear(IRenderTarget* target, coloru32 color)
{
	if (d3dRenderTargetView == target->d3dRenderTargetView)
		Discard();
	dx.d3dContext->ClearRenderTargetView(target->d3dRenderTargetView, (float*) &color.toFloat4());
}
void Render::Clear(coloru32 color)
{
	Discard();
	dx.d3dContext->ClearRenderTargetView(d3dRenderTargetView, (float*) &color.toFloat4());
}
void Render::SetTransform(const matrix3x2& transform)
{
	Flush();
	vscbTransform.setTransformMatrix(transform);
	dx.d3dContext->UpdateSubresource(dx.d3dVSCBTransform, 0, nullptr, &vscbTransform, 0, 0);
}

void Render::SetSamplerMode(SamplerMode mode)
{
	static SamplerMode lastSamplerMode = SamplerMode::None;
	if (lastSamplerMode == mode)
		return;

	Flush();
	switch (mode)
	{
	case SamplerMode::Linear_TextAddressMirrorOnce:
		dx.d3dContext->PSSetSamplers(0, 1, &dx.d3dSamplerStateLinearMirrorOnce);
		break;
	case SamplerMode::Linear_TextAddressWrap:
		dx.d3dContext->PSSetSamplers(0, 1, &dx.d3dSamplerStateLinearWrap);
		break;
	default:
		return;
	}
	lastSamplerMode = mode;
}
void Render::SetBlendState(BlendState state)
{
	static BlendState lastBlendState = BlendState::None;
	if (lastBlendState == state)
		return;

	Flush();
	switch (state)
	{
	case BlendState::AlphaBlend:
		dx.d3dContext->OMSetBlendState(dx.d3dBlendStateAlpha, nullptr, 0xffffffff);
		break;
	case BlendState::Clear:
		dx.d3dContext->OMSetBlendState(dx.d3dBlendStateClear, nullptr, 0xffffffff);
		break;
	default:
		return;
	}
	lastBlendState = state;
}
void Render::SetEnableIndexation(bool state)
{
	if (indexationEnabled != state)
	{
		Flush();
		indexationEnabled = state;
	}
}

//--------------------------------Interfaces----------------------------------//

inline bool ITexture::InitITexture(uint32 x, uint32 y, void* data, uint32 bindFlags)
{
	return SUCCEEDED(Render::dx.d3dDevice->CreateTexture2D(&D3D11Texture2DDesc(x, y, bindFlags),
		data ? &D3D11SubresourceData(data, x * 4) : nullptr, &d3dTexture));
}
inline bool IShaderResource::InitIShaderResource()
{
	return SUCCEEDED(Render::dx.d3dDevice->CreateShaderResourceView(d3dTexture, nullptr, &d3dShaderResourceView));
}
inline bool IRenderTarget::InitIRenderTarget()
{
	return SUCCEEDED(Render::dx.d3dDevice->CreateRenderTargetView(d3dTexture, nullptr, &d3dRenderTargetView));
}
void ITexture::Update(rectu32* rect, void* data)
{
	Render::dx.d3dContext->UpdateSubresource(d3dTexture, 0, &D3D11Box(rect->left, rect->right, rect->top, rect->bottom),
		data, (rect->right - rect->left) * 4, 0);
}
uint32x2 ITexture::GetSize()
{
	if (d3dTexture)
	{
		D3D11_TEXTURE2D_DESC d3dTextureDesc;
		d3dTexture->GetDesc(&d3dTextureDesc);
		return uint32x2(d3dTextureDesc.Width, d3dTextureDesc.Height);
	}
	return uint32x2(0, 0);
}
ITexture::~ITexture()
{
	SafeComObjectRelease(d3dTexture);
}
IShaderResource::~IShaderResource()
{
	SafeComObjectRelease(d3dShaderResourceView);
}
IRenderTarget::~IRenderTarget()
{
	SafeComObjectRelease(d3dRenderTargetView);
}

bool Texture::Create(uint32 x, uint32 y, void* data)
{
	bool result = true;
	result &= InitITexture(x, y, data, D3D11_BIND_SHADER_RESOURCE);
	result &= InitIShaderResource();

	if (result)
		return true;
	Release();
	return false;
}
bool Texture::CreateFromPng(wchar_t* filename)
{
	HANDLE hFile = CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
		throw;

	FILE_STANDARD_INFO fileInfo;
	GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileInfo, sizeof(FILE_STANDARD_INFO));
	uint32 fileSize = (uint32) fileInfo.EndOfFile.QuadPart;
	void* fileData = malloc(fileSize);
	ReadFile(hFile, fileData, fileSize, nullptr, nullptr);
	CloseHandle(hFile);

	uint8* textureData = nullptr;
	uint32 width = 0, height = 0;
	uint32 pngDecodeResult = lodepng_decode32(&textureData, &width, &height, (uint8*) fileData, fileSize);
	free(fileData);
	if (pngDecodeResult)
		return false;
	bool result = Create(width, height, textureData);
	free(textureData);
	return result;
}
bool RenderTarget::Create(uint32 x, uint32 y, void* data)
{
	bool result = true;
	result &= InitITexture(x, y, data, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	result &= InitIShaderResource();
	result &= InitIRenderTarget();

	if (result)
		return true;
	Release();
	return false;
}
bool SwapChain::CreateForComposition(IUnknown* panel, uint32 x, uint32 y)
{
	bool result = true;
	ISwapChainBackgroundPanelNative *swapChainNativePanel;
	if (FAILED(panel->QueryInterface(__uuidof(ISwapChainBackgroundPanelNative), (void**) &swapChainNativePanel)))
		return false;
	result &= SUCCEEDED(Render::dx.dxgiFactory->CreateSwapChainForComposition(Render::dx.d3dDevice, 
		&DXGISwapChainDesc1(x, y, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL), nullptr, &dxgiSwapChain));
	swapChainNativePanel->SetSwapChain(dxgiSwapChain);
	dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &d3dTexture);
	result &= InitIRenderTarget();

	if (result)
		return true;
	Release();
	return false;
}
bool SwapChain::CreateForHWnd(void* hWnd, uint32 x, uint32 y)
{
	bool result = true;
	result &= SUCCEEDED(Render::dx.dxgiFactory->CreateSwapChainForHwnd(Render::dx.d3dDevice, (HWND)hWnd,
		&DXGISwapChainDesc1(x, y, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_EFFECT_SEQUENTIAL), nullptr, nullptr, &dxgiSwapChain));
	dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &d3dTexture);
	result &= InitIRenderTarget();

	if (result)
		return true;
	Release();
	return false;
}
bool SwapChain::Resize(uint32 x, uint32 y)
{
	bool result = true;
	bool isActive = Render::d3dRenderTargetView == d3dRenderTargetView;

	if (isActive)
		Render::ResetTarget();

	SafeComObjectRelease(d3dRenderTargetView);
	SafeComObjectRelease(d3dTexture);
	result &= SUCCEEDED(dxgiSwapChain->ResizeBuffers(0, x, y, TEXTURE_FORMAT, 0));
	dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &d3dTexture);
	result &= InitIRenderTarget();

	if (isActive)
		Render::SetTarget(this);

	if (result)
		return true;
	Release();
	return false;
}
void SwapChain::Present(bool sync)
{
	dxgiSwapChain->Present(sync ? 1 : 0, 0);
}
SwapChain::~SwapChain()
{
	SafeComObjectRelease(dxgiSwapChain);
}

bool TextFormat::Create()
{
	return texture.CreateFromPng(L"font.png");
}