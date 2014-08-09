#include <memory.h>
#include <malloc.h>

#include "Math.h"

struct ID3D11Device2;
struct ID3D11DeviceContext2;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

struct IDXGIFactory3;
struct IDXGISwapChain2;

class Render;

struct VertexPosTex
{
	float32x3 pos;
	float32x2 tex;
};
struct VertexPosNorm
{
	float32x3 pos;
	float32x3 norm;
};
struct VertexPosNormTex
{
	float32x3 pos;
	float32x3 norm;
	float32x2 tex;
};

enum class VertexFormat : uint8
{
	None = 0,
	PosTex = 1,
	PosNorm = 2,
	PosNormTex = 3,
};

enum class EffectType : uint8
{
	None = 0,
	_PosTex = 1,
	_PosNorm = 2,
	_PosNormTex = 3,
};

template<VertexFormat> struct VertexTypeByFormat {};
template<> struct VertexTypeByFormat<VertexFormat::PosTex>
{
	typedef VertexPosTex type;
};
template<> struct VertexTypeByFormat<VertexFormat::PosNorm>
{
	typedef VertexPosNorm type;
};
template<> struct VertexTypeByFormat<VertexFormat::PosNormTex>
{
	typedef VertexPosNormTex type;
};

inline uint32 GetVertexSize(VertexFormat format)
{
	switch (format)
	{
	case VertexFormat::PosTex:
		return sizeof(VertexPosTex);
	case VertexFormat::PosNorm:
		return sizeof(VertexPosNorm);
	case VertexFormat::PosNormTex:
		return sizeof(VertexPosNormTex);
	}
	return 0;
}

inline VertexFormat GetVertexFormatByEffectType(EffectType type)
{
	switch (type)
	{
	case EffectType::_PosTex:
		return VertexFormat::PosTex;
	case EffectType::_PosNorm:
		return VertexFormat::PosNorm;
	case EffectType::_PosNormTex:
		return VertexFormat::PosNormTex;
	}
	return VertexFormat::None;
}

class Mesh
{
private:
	ID3D11Buffer *d3dVertexBuffer, *d3dIndexBuffer;
	uint32 vertexCount, indexCount;
	uint8 vertexSize;
	EffectType effectType;

	friend Render;

public:
	Mesh(void* vertexBuffer, void* indexBuffer, uint32 _vertexCount,
		uint32 _triangleCount, EffectType _effectType);
};

template<typename VertexType>
inline void CalucateVertexBufferNormals(VertexType* vertexBuffer, uint16x3* indexBuffer, uint32 vertexCount, uint32 triangleCount)
{
	uint16 *triPerVrtCntBuffer = (uint16*) malloc(sizeof(*triPerVrtCntBuffer) * triangleCount);
	memset(triPerVrtCntBuffer, 0, sizeof(*triPerVrtCntBuffer) * triangleCount);
	for (uint32 vrtIdx = 0; vrtIdx < vertexCount; vrtIdx++)
		vertexBuffer[vrtIdx].norm.set(0.0f, 0.0f, 0.0f);
	for (uint32 triIdx = 0; triIdx < triangleCount; triIdx++)
	{
		float32x3 v1 = vertexBuffer[indexBuffer[triIdx].x].pos;
		float32x3 v2 = vertexBuffer[indexBuffer[triIdx].y].pos;
		float32x3 v3 = vertexBuffer[indexBuffer[triIdx].z].pos;
		float32x3 normal(
			v1.y * (v2.z - v3.z) + v2.y * (v3.z - v1.z) + v3.y * (v1.z - v2.z),
			v1.z * (v2.x - v3.x) + v2.z * (v3.x - v1.x) + v3.z * (v1.x - v2.x),
			v1.x * (v2.y - v3.y) + v2.x * (v3.y - v1.y) + v3.x * (v1.y - v2.y));
		normal.normalize();
		vertexBuffer[indexBuffer[triIdx].x].norm += normal;
		vertexBuffer[indexBuffer[triIdx].y].norm += normal;
		vertexBuffer[indexBuffer[triIdx].z].norm += normal;
		triPerVrtCntBuffer[indexBuffer[triIdx].x]++;
		triPerVrtCntBuffer[indexBuffer[triIdx].y]++;
		triPerVrtCntBuffer[indexBuffer[triIdx].z]++;
	}
	for (uint32 vrtIdx = 0; vrtIdx < vertexCount; vrtIdx++)
	{
		if (triPerVrtCntBuffer[vrtIdx])
			vertexBuffer[vrtIdx].norm /= (float) triPerVrtCntBuffer[vrtIdx];
	}
	free(triPerVrtCntBuffer);
}

class RenderTarget
{
private:
	IDXGISwapChain2* dxgiSwapChain;
	ID3D11RenderTargetView *d3dRenderTargetView;
	ID3D11DepthStencilView *d3dDepthStencilView;
	uint16x2 size;

	friend Render;

public:
	RenderTarget(void* hWnd);
	void Clear();
	void Present();
};

class Render abstract
{
private:
	static ID3D11Device2 *d3dDevice;
	static ID3D11DeviceContext2 *d3dContext;
	static IDXGIFactory3 *dxgiFactory;

	static ID3D11PixelShader *d3dPSPosTex, *d3dPSPosNorm, *d3dPSPosNormTex;
	static ID3D11VertexShader *d3dVSPosTex, *d3dVSPosNorm, *d3dVSPosNormTex;
	static ID3D11InputLayout *d3dILPosTex, *d3dILPosNorm, *d3dILPosNormTex;
	static ID3D11Buffer *d3dVSCBTransform;

	friend RenderTarget;
	friend Mesh;

	static void SetEffect(EffectType type);

public:
	static void Init();
	static void SetRenderTarget(RenderTarget* renderTarget);
	static void SetTransform(Matrix4x4* transform);
	static void Draw(Mesh* mesh);
};