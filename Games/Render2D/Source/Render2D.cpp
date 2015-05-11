#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <windows.ui.xaml.media.dxinterop.h>

#include "D3DHelper.h"
#include "DXGIHelper.h"

#include <extypes.matrix3x2.h>

#include "Render2D.h"

#include "Shaders\Compiled\vsColor.csoh"
#include "Shaders\Compiled\vsTex.csoh"
#include "Shaders\Compiled\vsEllipse.csoh"
#include "Shaders\Compiled\psColor.csoh"
#include "Shaders\Compiled\psTex.csoh"
#include "Shaders\Compiled\psEllipse.csoh"

using namespace Render2D;

IDXGIFactory3 *Device::dxgiFactory = nullptr;

Device::Device()
{
	memset(this, 0, sizeof(*this));
}
Device::~Device()
{
	SafeComInterfaceRelease(d3dColorIL);
	SafeComInterfaceRelease(d3dTexIL);
	SafeComInterfaceRelease(d3dEllipseIL);
	SafeComInterfaceRelease(d3dColorVS);
	SafeComInterfaceRelease(d3dTexVS);
	SafeComInterfaceRelease(d3dEllipseVS);
	SafeComInterfaceRelease(d3dColorPS);
	SafeComInterfaceRelease(d3dTexPS);
	SafeComInterfaceRelease(d3dEllipsePS);

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

	d3dDevice->CreateInputLayout(d3dVertexColorILDesc, elemcntof(d3dVertexColorILDesc), vsColorData, sizeof(vsColorData), &d3dColorIL);
	d3dDevice->CreateInputLayout(d3dVertexTexILDesc, elemcntof(d3dVertexTexILDesc), vsTexData, sizeof(vsTexData), &d3dTexIL);
	d3dDevice->CreateInputLayout(d3dVertexEllipseILDesc, elemcntof(d3dVertexEllipseILDesc), vsEllipseData, sizeof(vsEllipseData), &d3dEllipseIL);
	d3dDevice->CreateVertexShader(vsColorData, sizeof(vsColorData), nullptr, &d3dColorVS);
	d3dDevice->CreateVertexShader(vsTexData, sizeof(vsTexData), nullptr, &d3dTexVS);
	d3dDevice->CreateVertexShader(vsEllipseData, sizeof(vsEllipseData), nullptr, &d3dEllipseVS);
	d3dDevice->CreatePixelShader(psColorData, sizeof(psColorData), nullptr, &d3dColorPS);
	d3dDevice->CreatePixelShader(psTexData, sizeof(psTexData), nullptr, &d3dTexPS);
	d3dDevice->CreatePixelShader(psEllipseData, sizeof(psEllipseData), nullptr, &d3dEllipsePS);

	d3dDevice->CreateBuffer(&D3D11BufferDesc(vertexBufferSize, D3D11_BIND_VERTEX_BUFFER), nullptr, &d3dVertexBuffer);
	d3dDevice->CreateBuffer(&D3D11BufferDesc(sizeof(float32x4) * 2, D3D11_BIND_CONSTANT_BUFFER), nullptr, &d3dTransformVSCB);

	d3dDevice->CreateRasterizerState(&D3D11RasterizerDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE), &d3dDefaultRS);
	d3dDevice->CreateBlendState(&D3D11BlendDesc(
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_OP_ADD, D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE), &d3dAlphaBS);
	d3dDevice->CreateSamplerState(&D3D11SamplerDesc(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP), &d3dDefaultSS);

	SetTransform(matrix3x2::identity());

	return true;
}

void Device::SetTarget(IRenderTarget *target)
{
	ID3D11RenderTargetView *d3dRenderTargetView = target->GetD3D11RenderTargetView();
	d3dContext->OMSetRenderTargets(1, &d3dRenderTargetView, nullptr);

	uint32x2 targetSize = target->GetSize();
	d3dContext->RSSetViewports(1, &D3D11ViewPort(0.0f, 0.0f, float(targetSize.x), float(targetSize.y)));
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

void Device::Clear(IRenderTarget* target, coloru32 color)
{
	float colorf[4];
	color.toFloat4Unorm(colorf);
	d3dContext->ClearRenderTargetView(target->GetD3D11RenderTargetView(), colorf);
}
void Device::Clear(coloru32 color)
{
	float colorf[4];
	color.toFloat4Unorm(colorf);
	ID3D11RenderTargetView *d3dRenderTargetView = nullptr;
	d3dContext->OMGetRenderTargets(1, &d3dRenderTargetView, nullptr);
	if (d3dRenderTargetView)
		d3dContext->ClearRenderTargetView(d3dRenderTargetView, colorf);
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
void Device::UpdateVertexBuffer(VertexTex* vertices, uint32 vertexCount)
{
	d3dContext->UpdateSubresource(d3dVertexBuffer, 0, &D3D11Box(0, sizeof(VertexTex) * vertexCount), vertices, 0, 0);
	setD3DDeviceContextStates(d3dTexIL, d3dTexVS, d3dTexPS, sizeof(VertexTex));
}
void Device::UpdateVertexBuffer(VertexEllipse* vertices, uint32 vertexCount)
{
	d3dContext->UpdateSubresource(d3dVertexBuffer, 0, &D3D11Box(0, sizeof(VertexEllipse) * vertexCount), vertices, 0, 0);
	setD3DDeviceContextStates(d3dEllipseIL, d3dEllipseVS, d3dEllipsePS, sizeof(VertexEllipse));
}

inline void Device::setD3DDeviceContextStates(ID3D11InputLayout* d3dInputLayout, 
	ID3D11VertexShader* d3dVertexShader, ID3D11PixelShader* d3dPixelShader, uint32 vertexSize)
{
	d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dContext->IASetInputLayout(d3dInputLayout);
	d3dContext->VSSetShader(d3dVertexShader, nullptr, 0);
	d3dContext->PSSetShader(d3dPixelShader, nullptr, 0);
	d3dContext->PSSetSamplers(0, 1, &d3dDefaultSS);
	//d3dContext->RSSetState(d3dDefaultRS);
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
	if (!quadIndexBuffer.GetID3D11Buffer())
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

//--------------------------------Interfaces----------------------------------//

inline bool ITexture::InitITexture(ID3D11Device* d3dDevice, uint32 x, uint32 y, void* data, uint32 bindFlags)
{
	return SUCCEEDED(d3dDevice->CreateTexture2D(&D3D11Texture2DDesc(x, y, bindFlags),
		data ? &D3D11SubresourceData(data, x * 4) : nullptr, &d3dTexture));
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
		D3D11_TEXTURE2D_DESC d3dTextureDesc;
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

bool Texture::Create(Device* device, uint32 x, uint32 y, void* data)
{
	bool result = true;
	result &= InitITexture(device->GetD3DDevice(), x, y, data, D3D11_BIND_SHADER_RESOURCE);
	result &= InitIShaderResource(device->GetD3DDevice());

	if (result)
		return true;
	this->~Texture();
	return false;
}
/*bool Texture::CreateFromPng(wchar_t* filename)
{
	HANDLE hFile = CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

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
}*/
bool RenderTarget::Create(Device* device, uint32 x, uint32 y, void* data)
{
	bool result = true;
	result &= InitITexture(device->GetD3DDevice(), x, y, data, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	result &= InitIShaderResource(device->GetD3DDevice());
	result &= InitIRenderTarget(device->GetD3DDevice());

	if (result)
		return true;
	this->~RenderTarget();
	return false;
}

bool SwapChain::CreateForComposition(Device* device, IUnknown* panel, uint32 x, uint32 y)
{
	bool result = true;
	ISwapChainBackgroundPanelNative *swapChainNativePanel;
	if (FAILED(panel->QueryInterface(__uuidof(ISwapChainBackgroundPanelNative), (void**) &swapChainNativePanel)))
		return false;
	result &= SUCCEEDED(Device::GetDXGIFactory()->CreateSwapChainForComposition(device->GetD3DDevice(),
		&DXGISwapChainDesc1(x, y), nullptr, &dxgiSwapChain));
	swapChainNativePanel->SetSwapChain(dxgiSwapChain);
	dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &d3dTexture);
	result &= InitIRenderTarget(device->GetD3DDevice());

	if (result)
		return true;
	this->~SwapChain();
	return false;
}
bool SwapChain::CreateForHWnd(Device* device, void* hWnd, uint32 x, uint32 y)
{
	bool result = true;
	result &= SUCCEEDED(Device::GetDXGIFactory()->CreateSwapChainForHwnd(device->GetD3DDevice(), (HWND) hWnd,
		&DXGISwapChainDesc1(x, y), nullptr, nullptr, &dxgiSwapChain));
	dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &d3dTexture);
	result &= InitIRenderTarget(device->GetD3DDevice());

	if (result)
		return true;
	this->~SwapChain();
	return false;
}
bool SwapChain::Resize(uint32 x, uint32 y)
{
	ID3D11Device *d3dDevice = nullptr;
	d3dRenderTargetView->GetDevice(&d3dDevice);

	SafeComInterfaceRelease(d3dRenderTargetView);
	SafeComInterfaceRelease(d3dTexture);
	bool result = true;
	result &= SUCCEEDED(dxgiSwapChain->ResizeBuffers(0, x, y, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &d3dTexture);
	result &= InitIRenderTarget(d3dDevice);

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