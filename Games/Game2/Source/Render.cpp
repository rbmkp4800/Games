#include <d3d11_2.h>
#include <dxgi1_3.h>

#include "types.h"

#include "Render.h"
#include "DXHelper.h"

#include "Shaders\vsPosTex.h"
#include "Shaders\vsPosNorm.h"
//#include "Shaders\vsPosNormTex.h"
#include "Shaders\psPosTex.h"
#include "Shaders\psPosNorm.h"
//#include "Shaders\psPosNormTex.h"

using namespace DXHelper;

ID3D11Device2 *Render::d3dDevice = nullptr;
ID3D11DeviceContext2 *Render::d3dContext = nullptr;
IDXGIFactory3 *Render::dxgiFactory = nullptr;

ID3D11PixelShader *Render::d3dPSPosTex, *Render::d3dPSPosNorm, *Render::d3dPSPosNormTex;
ID3D11VertexShader *Render::d3dVSPosTex, *Render::d3dVSPosNorm, *Render::d3dVSPosNormTex;
ID3D11InputLayout *Render::d3dILPosTex, *Render::d3dILPosNorm, *Render::d3dILPosNormTex;
ID3D11Buffer *Render::d3dVSCBTransform;

void Render::Init()
{
	D3D_FEATURE_LEVEL featureLevel;
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION,
		(ID3D11Device**) &d3dDevice, &featureLevel, (ID3D11DeviceContext**) &d3dContext);
	CreateDXGIFactory2(0, __uuidof(IDXGIFactory3), (void**) &dxgiFactory);

	D3D11_INPUT_ELEMENT_DESC d3dILDescPosTex [] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	D3D11_INPUT_ELEMENT_DESC d3dILDescPosNorm [] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	D3D11_INPUT_ELEMENT_DESC d3dILDescPosNormTex [] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	d3dDevice->CreateInputLayout(d3dILDescPosTex, elemcntof(d3dILDescPosTex), vsPosTexData, sizeof(vsPosTexData), &d3dILPosTex);
	d3dDevice->CreateInputLayout(d3dILDescPosNorm, elemcntof(d3dILDescPosNorm), vsPosNormData, sizeof(vsPosNormData), &d3dILPosNorm);
	//d3dDevice->CreateInputLayout(d3dILDescPosNormTex, elemcntof(d3dILDescPosNormTex), vsPosNormTexData, sizeof(vsPosNormTexData), &d3dILPosNormTex);
	d3dDevice->CreateVertexShader(vsPosTexData, sizeof(vsPosTexData), nullptr, &d3dVSPosTex);
	d3dDevice->CreateVertexShader(vsPosNormData, sizeof(vsPosNormData), nullptr, &d3dVSPosNorm);
	//d3dDevice->CreateVertexShader(vsPosNormTexData, sizeof(vsPosNormTexData), nullptr, &d3dVSPosNormTex);
	d3dDevice->CreatePixelShader(psPosTexData, sizeof(psPosTexData), nullptr, &d3dPSPosTex);
	d3dDevice->CreatePixelShader(psPosNormData, sizeof(psPosNormData), nullptr, &d3dPSPosNorm);
	//d3dDevice->CreatePixelShader(psPosNormTexData, sizeof(psPosNormTexData), nullptr, &d3dPSPosNormTex);
	d3dDevice->CreateBuffer(&D3D11BufferDesc(sizeof(Matrix4x4), D3D11_BIND_CONSTANT_BUFFER), nullptr, &d3dVSCBTransform);

	d3dContext->VSSetConstantBuffers(0, 1, &d3dVSCBTransform);
	d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11BlendState *d3dBlendStateAlpha;

	d3dDevice->CreateBlendState(&D3D11BlendDesc(
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_OP_ADD, D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE), &d3dBlendStateAlpha);
	d3dContext->OMSetBlendState(d3dBlendStateAlpha, nullptr, 0xffffffff);
}

void Render::SetEffect(EffectType type)
{
	static EffectType lastEffectType = EffectType::None;
	if (lastEffectType == type)
		return;

	ID3D11PixelShader *d3dPS = nullptr;
	ID3D11VertexShader *d3dVS = nullptr;
	ID3D11InputLayout *d3dIL = nullptr;
	switch (type)
	{
	case EffectType::_PosTex:
		d3dPS = d3dPSPosTex;
		d3dVS = d3dVSPosTex;
		d3dIL = d3dILPosTex;
		break;

	case EffectType::_PosNorm:
		d3dPS = d3dPSPosNorm;
		d3dVS = d3dVSPosNorm;
		d3dIL = d3dILPosNorm;
		break;

	case EffectType::_PosNormTex:
		d3dPS = d3dPSPosNormTex;
		d3dVS = d3dVSPosNormTex;
		d3dIL = d3dILPosNormTex;
		break;

	default:
		return;
	}
	d3dContext->PSSetShader(d3dPS, nullptr, 0);
	d3dContext->VSSetShader(d3dVS, nullptr, 0);
	d3dContext->IASetInputLayout(d3dIL);
}

void Render::SetTransform(Matrix4x4* transform)
{
	d3dContext->UpdateSubresource(d3dVSCBTransform, 0, nullptr, transform, 0, 0);
}

void Render::SetRenderTarget(RenderTarget* renderTarget)
{
	d3dContext->OMSetRenderTargets(1, &renderTarget->d3dRenderTargetView, renderTarget->d3dDepthStencilView);
	d3dContext->RSSetViewports(1, &D3D11ViewPort(0.0f, 0.0f, renderTarget->size.x, renderTarget->size.y));
}

void Render::Draw(Mesh* mesh)
{
	SetEffect(mesh->effectType);
	uint32 stride = mesh->vertexSize, offset = 0;
	Render::d3dContext->IASetVertexBuffers(0, 1, &mesh->d3dVertexBuffer, &stride, &offset);
	Render::d3dContext->IASetIndexBuffer(mesh->d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	d3dContext->DrawIndexed(mesh->indexCount, 0, 0);
}

//--------------------------------RenderTarget-----------------------------------//

RenderTarget::RenderTarget(void* hWnd)
{
	RECT clientRect;
	GetClientRect((HWND) hWnd, &clientRect);
	size.set((uint16) (clientRect.right - clientRect.left),
		(uint16) (clientRect.bottom - clientRect.top));

	ID3D11Texture2D *d3dRenderTargetTexture, *d3dDepthStencilTexture;
	Render::dxgiFactory->CreateSwapChainForHwnd(Render::d3dDevice, (HWND) hWnd,
		&DXGISwapChainDesc1(size), nullptr, nullptr, (IDXGISwapChain1**) &dxgiSwapChain);
	dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &d3dRenderTargetTexture);
	Render::d3dDevice->CreateTexture2D(&D3D11Texture2DDesc(size.x, size.y, D3D11_BIND_DEPTH_STENCIL,
		DXGI_FORMAT_D24_UNORM_S8_UINT), nullptr, &d3dDepthStencilTexture);
	Render::d3dDevice->CreateRenderTargetView(d3dRenderTargetTexture, nullptr, &d3dRenderTargetView);
	Render::d3dDevice->CreateDepthStencilView(d3dDepthStencilTexture, nullptr, &d3dDepthStencilView);
}
void RenderTarget::Clear()
{
	float rgba[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	Render::d3dContext->ClearRenderTargetView(d3dRenderTargetView, rgba);
	Render::d3dContext->ClearDepthStencilView(d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void RenderTarget::Present()
{
	dxgiSwapChain->Present(1, 0);
}

//----------------------------------Mesh---------------------------------------//

Mesh::Mesh(void* vertexBuffer, void* indexBuffer, uint32 _vertexCount, uint32 triangleCount, EffectType _effectType)
	: vertexCount(_vertexCount), indexCount(triangleCount * 3), effectType(_effectType)
{
	vertexSize = GetVertexSize(GetVertexFormatByEffectType(effectType));
	Render::d3dDevice->CreateBuffer(&D3D11BufferDesc(vertexSize * vertexCount, D3D11_BIND_VERTEX_BUFFER),
		&D3D11SubresourceData(vertexBuffer), &d3dVertexBuffer);
	Render::d3dDevice->CreateBuffer(&D3D11BufferDesc(sizeof(uint16) * indexCount, D3D11_BIND_INDEX_BUFFER),
		&D3D11SubresourceData(indexBuffer), &d3dIndexBuffer);
}