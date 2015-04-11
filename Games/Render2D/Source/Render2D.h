#include <extypes.h>
#include <extypes.vectors.h>

struct IUnknown;

struct IDXGIFactory3;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11RasterizerState;
struct ID3D11BlendState;

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
struct IDXGISwapChain1;

struct matrix3x2;

namespace Render2D
{
	enum class Direction : uint8
	{
		None = 0,
		Up = 1,
		Right = 2,
		Down = 3,
		Left = 4,
	};
	enum class Orientation : uint8
	{
		None = 0,
		Horizontal = 1,
		Vertical = 2,
	};
	struct colors
	{
		const static uint32
			transparent = 0x00000000,
			black = 0xff000000,
			white = 0xffffffff,
			red = 0xff0000ff,
			green = 0xff00ff00,
			blue = 0xffff0000,
			yellow = 0xff00ffff,
			magenta = 0xffff00ff,
			cyan = 0xffffff00,
			darkRed = 0xff00007f,
			darkGreen = 0xff007f00,
			darkBlue = 0xff7f0000,
			darkYellow = 0xff007f7f,
			darkMagenta = 0xff7f007f,
			darkCyan = 0xff7f7f00,
			gray = 0xff7f7f7f,
			cornflowerBlue = 0xffed9564,
			lightRed = 0xff7f7fff,
			lightGreen = 0xff7fff7f,
			lightBlue = 0xffe6d8ad,
			lightSalmon = 0xff7aa0ff,
			skyBlue = 0xffebce87,
			lightGray = 0xffc0c0c0,
			darkGray = 0xff3f3f3f;
	};

	struct coloru32
	{
		union
		{
			struct
			{
				uint8 r, g, b, a;
			};
			uint32 rgba;
		};
		inline coloru32() : rgba(0) {}
		inline coloru32(uint32 _rgba) : rgba(_rgba) {}
		inline coloru32(uint32 rgb, uint8 _a) : rgba((rgb & 0xffffff) | (_a << 24)) {}
		inline coloru32(uint32 rgb, float _a) : rgba((rgb & 0xffffff) | ((uint32)(saturate(_a) * 255.0f) << 24)) {}
		inline coloru32(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 0xff) : r(_r), g(_g), b(_b), a(_a) {}
		inline void set(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 0xff) { r = _r; g = _g; b = _b; a = _a; }
		inline void set(uint32 rgb, uint8 _a = 0xff) { rgba = rgb & 0xffffff | _a << 24; }
		inline float32x4 toFloat4Unorm() { return float32x4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); }
		inline void toFloat4Unorm(float* result)
		{
			result[0] = r / 255.0f;
			result[1] = g / 255.0f;
			result[2] = b / 255.0f;
			result[3] = a / 255.0f;
		}
	};

	//--------------------------------Interfaces----------------------------------//

	class Device;

	class INoncopyable abstract
	{
	private:
		INoncopyable(const INoncopyable&) = delete;
		INoncopyable& operator= (const INoncopyable&) = delete;

	public:
		INoncopyable() = default;
		~INoncopyable() = default;
	};

	class ITexture abstract : public INoncopyable
	{
	protected:
		ID3D11Texture2D *d3dTexture;

		inline bool InitITexture(ID3D11Device* d3dDevice, uint32 x, uint32 y, void* data, uint32 bindFlags);

	public:
		inline ITexture() : d3dTexture(nullptr) {}
		~ITexture();

		uint32x2 GetSize();
		inline ID3D11Texture2D* GetD3D11Texture2D() { return d3dTexture; }
	};

	class IShaderResource abstract : virtual public ITexture
	{
	protected:
		ID3D11ShaderResourceView *d3dShaderResourceView;

		inline bool InitIShaderResource(ID3D11Device* d3dDevice);

	public:
		inline IShaderResource() : d3dShaderResourceView(nullptr) {}
		~IShaderResource();

		inline ID3D11ShaderResourceView* GetD3D11ShaderResourceView() { return d3dShaderResourceView; }
	};

	class IRenderTarget abstract : virtual public ITexture
	{
	protected:
		ID3D11RenderTargetView *d3dRenderTargetView;

		inline bool InitIRenderTarget(ID3D11Device* d3dDevice);

	public:
		inline IRenderTarget() : d3dRenderTargetView(nullptr) {}
		~IRenderTarget();

		inline ID3D11RenderTargetView* GetD3D11RenderTargetView() { return d3dRenderTargetView; }
	};

	//--------------------------------Objects----------------------------------//

	class Texture : public IShaderResource
	{
	public:
		bool Create(Device* device, uint32 x, uint32 y, void* data = nullptr);
	};

	class RenderTarget : public IShaderResource, public IRenderTarget
	{
	public:
		bool Create(Device* device, uint32 x, uint32 y, void* data = nullptr);
	};

	class SwapChain : public IRenderTarget
	{
	private:
		IDXGISwapChain1 *dxgiSwapChain;

	public:
		inline SwapChain() : dxgiSwapChain(nullptr) {}
		~SwapChain();

		bool CreateForComposition(Device* device, IUnknown* panel, uint32 x, uint32 y);
		bool CreateForHWnd(Device* device, void* hWnd, uint32 x, uint32 y);
		bool Resize(uint32 x, uint32 y);
		void Present(bool sync = true);
	};

	//-----------------------------------Device-------------------------------------//

	struct VertexColor	//12 bytes, 6 floats
	{
		float32x2 pos;
		coloru32 color;
	};
	struct VertexTex	//20 bytes, 5 floats
	{
		float32x2 pos;
		float32x2 tex;
		float alpha;
	};
	struct VertexEllipse	//24 bytes, 9 floats
	{
		float32x2 pos;
		float32x2 tex;
		float innerRadius;
		coloru32 color;
	};

	class Device : public INoncopyable
	{
	private:
		static IDXGIFactory3 *dxgiFactory;

		ID3D11Device *d3dDevice;
		ID3D11DeviceContext *d3dContext;

		ID3D11InputLayout *d3dColorIL, *d3dTexIL, *d3dEllipseIL;
		ID3D11VertexShader *d3dColorVS, *d3dTexVS, *d3dEllipseVS;
		ID3D11PixelShader *d3dColorPS, *d3dTexPS, *d3dEllipsePS;

		ID3D11Buffer *d3dVertexBuffer, *d3dQuadIndexBuffer, *d3dTransformVSCB;

		ID3D11RasterizerState *d3dDefaultRS;
		ID3D11SamplerState *d3dDefaultSS;
		ID3D11BlendState *d3dAlphaBS;

		inline void setStates(ID3D11InputLayout* d3dIL, ID3D11VertexShader* d3dVS, ID3D11PixelShader* d3dPS);
		inline void draw(void* vertices, uint32 vertexCount, uint32 vertexSize);
		inline void drawIndexedQuads(void* vertices, uint32 vertexCount, uint32 vertexSize);

		static const uint32 vertexBufferSize = 0x4000;
		static const uint32 vertexColorLimit = vertexBufferSize / sizeof(VertexColor);
		static const uint32 vertexTexLimit = vertexBufferSize / sizeof(VertexTex);
		static const uint32 vertexEllipseLimit = vertexBufferSize / sizeof(VertexEllipse);
		static const uint32 indexedQuadsLimit = vertexColorLimit / 6;

	public:
		Device();
		~Device();

		bool Create();

		void SetTarget(IRenderTarget* target);
		void SetTexture(IShaderResource* texture);
		void SetTransform(matrix3x2& _transform);

		void Clear(IRenderTarget* target, coloru32 color);
		void Clear(coloru32 color);
		void UpdateTexture(ITexture* texture, rectu32* rect, void* data);

		void DrawColored(VertexColor* vertices, uint32 vertexCount, bool indexedQuads);
		void DrawTextured(VertexTex* vertices, uint32 vertexCount, bool indexedQuads);
		void DrawEllipses(VertexEllipse* vertices, uint32 vertexCount, bool indexedQuads);

		static inline IDXGIFactory3* GetDXGIFactory() { return dxgiFactory; }
		inline ID3D11Device* GetD3DDevice() { return d3dDevice; }
		inline ID3D11DeviceContext* GetD3DDeviceContext() { return d3dContext; }
	};

	//-----------------------------------Batches-------------------------------------//

	class UniversalBatch
	{
	private:

	public:

	};

	/*class FastLinearAllocator
	{
	private:
		void *buffer;
		uintptr bufferSize, usedSize;

	public:
		FastLinearAllocator(uintptr initialSize);
		~FastLinearAllocator();

		void *Allocate(uintptr size);
		void Reset();
	};

	class Batch
	{
	private:

	public:

	};

	template<uint layersNumber, uint bufferAllocDepth, uint firstLayerBufferSize = 32>
	class LayeredBatch
	{
	private:
		struct
		{
			struct
			{
				VertexColor *buffers[bufferAllocDepth];
				uint32 allocBuffersCount;
				uint32 lastBufferSize, lastBufferUsedSize;
			} color;
			struct
			{
				VertexTex *buffers[bufferAllocDepth];
				uint32 allocBuffersCount;
				uint32 lastBufferSize, lastBufferUsedSize;
				TexturePack *texturePack;
			} tex;
			struct
			{
				VertexEllipse *buffers[bufferAllocDepth];
				uint32 allocBuffersCount;
				uint32 lastBufferSize, lastBufferUsedSize;
			} ellipse;
		} layers[layersNumber];
		
		FastLinearAllocator allocator;

	public:
		LayeredBatch(uint32 globalBufferInitialSize = 0x1000) : allocator(globalBufferInitialSize)
		{
			for (uint32 layerIdx = 0; layerIdx < layersNumber; layerIdx++)
			{
				layers[layerIdx].color.allocBuffersCount = 0;
				layers[layerIdx].color.lastBufferSize = 0;
				layers[layerIdx].tex.allocBuffersCount = 0;
				layers[layerIdx].tex.lastBufferSize = 0;
				layers[layerIdx].tex.texturePack = nullptr;
				layers[layerIdx].ellipse.allocBuffersCount = 0;
				layers[layerIdx].ellipse.lastBufferSize = 0;
			}
		}
		~LayeredBatch() {}
		void PushTexture(uint32 layer, const rectf32& destRect, const rectf32& srcRect, uint32 textureId);
		void PushRect(uint32 layer, const rectf32& destRect, coloru32 color);
		void PushEllipse(uint32 layer, const rectf32& destRect, coloru32 color, float innerRadius = -1.0f);
		void Clear();
		void BindLayerToTexturePack(uint32 layer, TexturePack* texturePack);
	};*/
}