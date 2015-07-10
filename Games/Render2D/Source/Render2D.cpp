#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <windows.ui.xaml.media.dxinterop.h>

#include "D3DHelper.h"
#include "DXGIHelper.h"

#include "Render2D.h"

#include <extypes.matrix3x2.h>

#include "Shaders\Compiled\vsColor.csoh"
#include "Shaders\Compiled\vsTex.csoh"
#include "Shaders\Compiled\vsEllipse.csoh"
#include "Shaders\Compiled\vsTexColor.csoh"
#include "Shaders\Compiled\psColor.csoh"
#include "Shaders\Compiled\psTex.csoh"
#include "Shaders\Compiled\psEllipse.csoh"
#include "Shaders\Compiled\psTexColor.csoh"

#include "DefaultFont.h"

using namespace Render2D;

IDXGIFactory3 *Device::dxgiFactory = nullptr;

Device::Device()
{
	d3dDevice = nullptr;
	d3dContext = nullptr;

	d3dColorIL = nullptr;
	d3dTextureIL = nullptr;
	d3dEllipseIL = nullptr;
	d3dTexColorIL = nullptr;
	d3dColorVS = nullptr;
	d3dTextureVS = nullptr;
	d3dEllipseVS = nullptr;
	d3dTexColorVS = nullptr;
	d3dColorPS = nullptr;
	d3dTexturePS = nullptr;
	d3dEllipsePS = nullptr;
	d3dTexColorPS = nullptr;

	vertexBufferSize = 0;
	d3dVertexBuffer = nullptr;
	d3dTransformVSCB = nullptr;

	d3dDefaultRS = nullptr;
	d3dDefaultSS = nullptr;
	d3dAlphaBS = nullptr;

	currentTarget = nullptr;
	aaPixelSize = nan<float32>();
}
Device::~Device()
{
	SafeComInterfaceRelease(d3dColorIL);
	SafeComInterfaceRelease(d3dTextureIL);
	SafeComInterfaceRelease(d3dEllipseIL);
	SafeComInterfaceRelease(d3dTexColorIL);
	SafeComInterfaceRelease(d3dColorVS);
	SafeComInterfaceRelease(d3dTextureVS);
	SafeComInterfaceRelease(d3dEllipseVS);
	SafeComInterfaceRelease(d3dTexColorVS);
	SafeComInterfaceRelease(d3dColorPS);
	SafeComInterfaceRelease(d3dTexturePS);
	SafeComInterfaceRelease(d3dEllipsePS);
	SafeComInterfaceRelease(d3dTexColorPS);

	SafeComInterfaceRelease(d3dVertexBuffer);
	SafeComInterfaceRelease(d3dTransformVSCB);

	SafeComInterfaceRelease(d3dDefaultRS);
	SafeComInterfaceRelease(d3dDefaultSS);
	SafeComInterfaceRelease(d3dAlphaBS);

	SafeComInterfaceRelease(d3dDevice);
	SafeComInterfaceRelease(d3dContext);
}

bool Device::Create(uint32 _vertexBufferSize)
{
	vertexBufferSize = _vertexBufferSize;

	D3D_FEATURE_LEVEL currentFeatureLevel, featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
	};
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
#ifdef _DEBUG
		D3D11_CREATE_DEVICE_DEBUG |
#endif
		D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevels, elemcntof(featureLevels), 
		D3D11_SDK_VERSION, &d3dDevice, &currentFeatureLevel, &d3dContext);

	if (!dxgiFactory)
		CreateDXGIFactory1(__uuidof(*dxgiFactory), (void**) &dxgiFactory);

	D3D11_INPUT_ELEMENT_DESC d3dVertexColorILDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	D3D11_INPUT_ELEMENT_DESC d3dVertexTexILDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	D3D11_INPUT_ELEMENT_DESC d3dVertexEllipseILDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 1, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	D3D11_INPUT_ELEMENT_DESC d3dVertexTexColorILDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	d3dDevice->CreateInputLayout(d3dVertexColorILDesc, elemcntof(d3dVertexColorILDesc), vsColorData, sizeof(vsColorData), &d3dColorIL);
	d3dDevice->CreateInputLayout(d3dVertexTexILDesc, elemcntof(d3dVertexTexILDesc), vsTexData, sizeof(vsTexData), &d3dTextureIL);
	d3dDevice->CreateInputLayout(d3dVertexEllipseILDesc, elemcntof(d3dVertexEllipseILDesc), vsEllipseData, sizeof(vsEllipseData), &d3dEllipseIL);
	d3dDevice->CreateInputLayout(d3dVertexTexColorILDesc, elemcntof(d3dVertexTexColorILDesc), vsTexColorData, sizeof(vsTexColorData), &d3dTexColorIL);

	d3dDevice->CreateVertexShader(vsColorData, sizeof(vsColorData), nullptr, &d3dColorVS);
	d3dDevice->CreateVertexShader(vsTexData, sizeof(vsTexData), nullptr, &d3dTextureVS);
	d3dDevice->CreateVertexShader(vsEllipseData, sizeof(vsEllipseData), nullptr, &d3dEllipseVS);
	d3dDevice->CreateVertexShader(vsTexColorData, sizeof(vsTexColorData), nullptr, &d3dTexColorVS);

	d3dDevice->CreatePixelShader(psColorData, sizeof(psColorData), nullptr, &d3dColorPS);
	d3dDevice->CreatePixelShader(psTexData, sizeof(psTexData), nullptr, &d3dTexturePS);
	d3dDevice->CreatePixelShader(psEllipseData, sizeof(psEllipseData), nullptr, &d3dEllipsePS);
	d3dDevice->CreatePixelShader(psTexColorData, sizeof(psTexColorData), nullptr, &d3dTexColorPS);

	d3dDevice->CreateBuffer(&D3D11BufferDesc(vertexBufferSize, D3D11_BIND_VERTEX_BUFFER), nullptr, &d3dVertexBuffer);
	d3dDevice->CreateBuffer(&D3D11BufferDesc(sizeof(float32x4) * 2, D3D11_BIND_CONSTANT_BUFFER), nullptr, &d3dTransformVSCB);

	d3dDevice->CreateRasterizerState(&D3D11RasterizerDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE), &d3dDefaultRS);
	d3dDevice->CreateBlendState(&D3D11BlendDesc(
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_OP_ADD, D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE), &d3dAlphaBS);
	d3dDevice->CreateSamplerState(&D3D11SamplerDesc(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP), &d3dDefaultSS);

	currentTarget = nullptr;
	aaPixelSize = nan<float32>();
	SetTransform(matrix3x2::identity());

	return true;
}

void Device::SetTarget(IRenderTarget *target)
{
	currentTarget = target;
	ID3D11RenderTargetView *d3dRenderTargetView = target->GetD3D11RenderTargetView();
	d3dContext->OMSetRenderTargets(1, &d3dRenderTargetView, nullptr);

	uint32x2 targetSize = target->GetSize();
	d3dContext->RSSetViewports(1, &D3D11ViewPort(0.0f, 0.0f, float32(targetSize.x), float32(targetSize.y)));
}
void Render2D::Device::ResetTarget()
{
	currentTarget = nullptr;
	ID3D11RenderTargetView *d3dNullRTV = nullptr;
	d3dContext->OMSetRenderTargets(1, &d3dNullRTV, nullptr);
}
void Device::SetTexture(IShaderResource *texture)
{
	ID3D11ShaderResourceView *d3dShaderResourceView = texture->GetD3D11ShaderResourceView();
	d3dContext->PSSetShaderResources(0, 1, &d3dShaderResourceView);
}
void Device::SetIndexBuffer(IndexBuffer* indexBuffer)
{
	d3dContext->IASetIndexBuffer(indexBuffer->GetID3D11Buffer(), DXGI_FORMAT_R16_UINT, 0);
}
void Device::SetTransform(const matrix3x2& _transform)
{
	struct
	{
		float32x4 row0;
		float32x4 row1;
	} transform;
	transform.row0.set(_transform.data[0][0], _transform.data[1][0], _transform.data[2][0], 0.0f);
	transform.row1.set(_transform.data[0][1], _transform.data[1][1], _transform.data[2][1], 0.0f);
	d3dContext->UpdateSubresource(d3dTransformVSCB, 0, nullptr, &transform, 0, 0);
}
void Device::SetDirectTransform()
{
	uint32x2 size = currentTarget->GetSize();
	SetTransform(matrix3x2::translation(-1.0f, 1.0f) * matrix3x2::scale(2.0f / float32(size.x), -2.0f / float32(size.y)), 1.0f);
}

void Device::Clear(IRenderTarget* target, Color color)
{
	float32 colorf[4];
	color.toFloat4Unorm(colorf);
	d3dContext->ClearRenderTargetView(target->GetD3D11RenderTargetView(), colorf);
}

void Device::UpdateTexture(ITexture* texture, rectu32* rect, void* data)
{
	d3dContext->UpdateSubresource(texture->GetD3D11Texture2D(), 0, 
		&D3D11Box(rect->left, rect->right, rect->top, rect->bottom),
		data, (rect->right - rect->left) * 4, 0);
}
void Device::UpdateIndexBuffer(IndexBuffer* indexBuffer, uint32 left, uint32 right, void* data)
{
	d3dContext->UpdateSubresource(indexBuffer->GetID3D11Buffer(), 0, &D3D11Box(left, sizeof(uint16) * right), data, 0, 0);
}
void Device::UpdateVertexBuffer(VertexColor* vertices, uint32 vertexCount)
{
	d3dContext->UpdateSubresource(d3dVertexBuffer, 0, &D3D11Box(0, sizeof(VertexColor) * vertexCount), vertices, 0, 0);
	setD3DDeviceContextStates(d3dColorIL, d3dColorVS, d3dColorPS, sizeof(VertexColor));
}
void Device::UpdateVertexBuffer(VertexTexture* vertices, uint32 vertexCount)
{
	d3dContext->UpdateSubresource(d3dVertexBuffer, 0, &D3D11Box(0, sizeof(VertexTexture) * vertexCount), vertices, 0, 0);
	setD3DDeviceContextStates(d3dTextureIL, d3dTextureVS, d3dTexturePS, sizeof(VertexTexture));
}
void Device::UpdateVertexBuffer(VertexEllipse* vertices, uint32 vertexCount)
{
	d3dContext->UpdateSubresource(d3dVertexBuffer, 0, &D3D11Box(0, sizeof(VertexEllipse) * vertexCount), vertices, 0, 0);
	setD3DDeviceContextStates(d3dEllipseIL, d3dEllipseVS, d3dEllipsePS, sizeof(VertexEllipse));
}
void Device::UpdateVertexBuffer(VertexTexColor* vertices, uint32 vertexCount)
{
	d3dContext->UpdateSubresource(d3dVertexBuffer, 0, &D3D11Box(0, sizeof(VertexTexColor) * vertexCount), vertices, 0, 0);
	setD3DDeviceContextStates(d3dTexColorIL, d3dTexColorVS, d3dTexColorPS, sizeof(VertexTexColor));
}

inline void Device::setD3DDeviceContextStates(ID3D11InputLayout* d3dInputLayout, 
	ID3D11VertexShader* d3dVertexShader, ID3D11PixelShader* d3dPixelShader, uint32 vertexSize)
{
	d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dContext->IASetInputLayout(d3dInputLayout);
	d3dContext->VSSetShader(d3dVertexShader, nullptr, 0);
	d3dContext->PSSetShader(d3dPixelShader, nullptr, 0);
	d3dContext->PSSetSamplers(0, 1, &d3dDefaultSS);
	d3dContext->RSSetState(d3dDefaultRS);
	d3dContext->OMSetBlendState(d3dAlphaBS, nullptr, 0xffffffff);
	D3D11ContextVSSetConstantBuffers(d3dContext, 0, d3dTransformVSCB);
	D3D11ContextIASetVertexBuffer(d3dContext, 0, d3dVertexBuffer, vertexSize);
}

void Device::Draw(uint32 vertexCount)
{
	d3dContext->Draw(vertexCount, 0);
}
void Device::DrawIndexed(uint32 indexCount)
{
	d3dContext->DrawIndexed(indexCount, 0, 0);
}

IndexBuffer* Device::GetQuadIndexBuffer()
{
	if (!quadIndexBuffer.IsInitialized())
	{
		uint32 indexBufferSize = vertexBufferSize / 2;
		uint16x3 *indexBuffer = (uint16x3*) malloc(sizeof(uint16x3) * indexBufferSize);
		for (uint16 i = 0; i < indexBufferSize / 2; i++)
		{
			indexBuffer[i * 2 + 0].set(i * 4 + 0, i * 4 + 3, i * 4 + 1);
			indexBuffer[i * 2 + 1].set(i * 4 + 1, i * 4 + 3, i * 4 + 2);
		}
		quadIndexBuffer.Create(this, sizeof(uint16x3) * indexBufferSize, indexBuffer);
		free(indexBuffer);
	}
	return &quadIndexBuffer;
}
MonospaceFont* Device::GetDefaultFont()
{
	if (!defaultFont.IsInitialized())
	{
		uint32 bitmapSize = defaultFontWidth * defaultFontHeight * MonospaceFont::charTableSize;
		uint8 *bitmapData = (uint8*) malloc(sizeof(uint8) * bitmapSize);
		for (uint32 i = 0; i < bitmapSize; i++)
			bitmapData[i] = (defaultFontData[i / 32] >> (i % 32)) & 1 ? 0xff : 0;
		defaultFont.Create(this, defaultFontWidth, defaultFontHeight, bitmapData);
		free(bitmapData);
	}
	return &defaultFont;
}

//--------------------------------Interfaces----------------------------------//

inline bool ITexture::InitITexture(ID3D11Device* d3dDevice, uint32 width, uint32 height, void* data, uint32 bindFlags)
{
	return SUCCEEDED(d3dDevice->CreateTexture2D(&D3D11Texture2DDesc(width, height, bindFlags),
		data ? &D3D11SubresourceData(data, width * 4) : nullptr, &d3dTexture));
}
inline bool IShaderResource::InitIShaderResource(ID3D11Device* d3dDevice)
{
	return SUCCEEDED(d3dDevice->CreateShaderResourceView(d3dTexture, nullptr, &d3dShaderResourceView));
}
inline bool IRenderTarget::InitIRenderTarget(ID3D11Device* d3dDevice)
{
	return SUCCEEDED(d3dDevice->CreateRenderTargetView(d3dTexture, nullptr, &d3dRenderTargetView));
}
uint32x2 ITexture::GetSize()
{
	if (d3dTexture)
	{
		D3D11_TEXTURE2D_DESC d3dTextureDesc = { 0 };
		d3dTexture->GetDesc(&d3dTextureDesc);
		return uint32x2(d3dTextureDesc.Width, d3dTextureDesc.Height);
	}
	return uint32x2(0, 0);
}
ITexture::~ITexture()
{
	SafeComInterfaceRelease(d3dTexture);
}
IShaderResource::~IShaderResource()
{
	SafeComInterfaceRelease(d3dShaderResourceView);
}
IRenderTarget::~IRenderTarget()
{
	SafeComInterfaceRelease(d3dRenderTargetView);
}

//--------------------------------Objects----------------------------------//

bool Texture::Create(Device* device, uint32 width, uint32 height, void* data)
{
	bool result = true;
	result &= InitITexture(device->GetD3DDevice(), width, height, data, D3D11_BIND_SHADER_RESOURCE);
	result &= InitIShaderResource(device->GetD3DDevice());

	if (result)
		return true;
	this->~Texture();
	return false;
}
bool RenderTarget::Create(Device* device, uint32 width, uint32 height, void* data)
{
	bool result = true;
	result &= InitITexture(device->GetD3DDevice(), width, height, data, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	result &= InitIShaderResource(device->GetD3DDevice());
	result &= InitIRenderTarget(device->GetD3DDevice());

	if (result)
		return true;
	this->~RenderTarget();
	return false;
}

bool SwapChain::CreateForComposition(Device* device, IUnknown* panel, uint32 width, uint32 height)
{
	bool result = true;
	ISwapChainBackgroundPanelNative *swapChainNativePanel;
	if (FAILED(panel->QueryInterface(__uuidof(ISwapChainBackgroundPanelNative), (void**) &swapChainNativePanel)))
		return false;
	result &= SUCCEEDED(Device::GetDXGIFactory()->CreateSwapChainForComposition(device->GetD3DDevice(),
		&DXGISwapChainDesc1(width, height), nullptr, &dxgiSwapChain));
	swapChainNativePanel->SetSwapChain(dxgiSwapChain);
	dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &d3dTexture);
	result &= InitIRenderTarget(device->GetD3DDevice());
	SafeComInterfaceRelease(swapChainNativePanel);

	if (result)
		return true;
	this->~SwapChain();
	return false;
}
bool SwapChain::CreateForHWnd(Device* device, void* hWnd, uint32 width, uint32 height)
{
	bool result = true;
	result &= SUCCEEDED(Device::GetDXGIFactory()->CreateSwapChainForHwnd(device->GetD3DDevice(), HWND(hWnd),
		&DXGISwapChainDesc1(width, height), nullptr, nullptr, &dxgiSwapChain));
	dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &d3dTexture);
	result &= InitIRenderTarget(device->GetD3DDevice());

	if (result)
		return true;
	this->~SwapChain();
	return false;
}
bool SwapChain::Resize(Device* device, uint32 width, uint32 height)
{
	SafeComInterfaceRelease(d3dRenderTargetView);
	SafeComInterfaceRelease(d3dTexture);

	bool result = true;
	result &= SUCCEEDED(dxgiSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &d3dTexture);
	result &= InitIRenderTarget(device->GetD3DDevice());

	if (result)
		return true;
	this->~SwapChain();
	return false;
}
void SwapChain::Present(bool sync)
{
	if (dxgiSwapChain)
		dxgiSwapChain->Present(sync ? 1 : 0, 0);
}
SwapChain::~SwapChain()
{
	SafeComInterfaceRelease(dxgiSwapChain);
}

bool IndexBuffer::Create(Device* device, uint32 size, void* data)
{
	return SUCCEEDED(device->GetD3DDevice()->CreateBuffer(&D3D11BufferDesc(size, D3D11_BIND_INDEX_BUFFER),
		data ? &D3D11SubresourceData(data) : nullptr, &d3dBuffer));
}
IndexBuffer::~IndexBuffer()
{
	SafeComInterfaceRelease(d3dBuffer);
}

bool MonospaceFont::Create(Device* device, uint32 _width, uint32 _height, void* data)
{
	width = float32(_width);
	height = float32(_height);

	bool result = true;
	result &= SUCCEEDED(device->GetD3DDevice()->CreateTexture2D(&D3D11Texture2DDesc(_width * MonospaceFont::charTableSize, _height,
		D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8_UNORM), &D3D11SubresourceData(data, _width * MonospaceFont::charTableSize), &d3dTexture));
	result &= InitIShaderResource(device->GetD3DDevice());

	if (result)
		return true;
	this->~MonospaceFont();
	return false;
}
MonospaceFont::~MonospaceFont()
{
	SafeComInterfaceRelease(d3dShaderResourceView);
	SafeComInterfaceRelease(d3dTexture);
}

//--------------------------------Batches--------------------------------//

Batch::Batch()
{
	memset(this, 0, sizeof(*this));
}
Batch::~Batch()
{
	Flush();
	if (vertexBuffer && privateMemoryBuffer)
		free(vertexBuffer);
}

void Batch::Create(Device *_device, uint32 _vertexBufferSize)
{
	device = _device;
	vertexBufferSize = _vertexBufferSize ? minval(_vertexBufferSize, device->GetVertexBufferSize()) : device->GetVertexBufferSize();
	vertexBuffer = malloc(vertexBufferSize);
	vertexCount = 0;
	effect = Effect::None;
	quadIndexationEnabled = true;
	privateMemoryBuffer = true;
}
void Batch::Create(Device *_device, void* _vertexBuffer, uint32 _vertexBufferSize)
{
	device = _device;
	vertexBuffer = _vertexBuffer;
	vertexBufferSize = minval(_vertexBufferSize, device->GetVertexBufferSize());
	vertexCount = 0;
	effect = Effect::None;
	quadIndexationEnabled = true;
	privateMemoryBuffer = false;
}