#include <extypes.h>
#include <extypes.vectors.h>
#include <extypes.vectors.math.h>

struct IUnknown;

struct IDXGIFactory3;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;
struct ID3D11BlendState;

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
struct IDXGISwapChain1;

struct matrix3x2;

namespace Render2D
{
	static const float opacityThreshold = 0.01f;

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

	struct colors
	{
		static const uint32
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
			dodgerBlue = 0xffff901e,
			gray = 0xff7f7f7f,
			cornflowerBlue = 0xffed9564,
			lightRed = 0xff7f7fff,
			lightGreen = 0xff7fff7f,
			lightBlue = 0xffe6d8ad,
			lightSalmon = 0xff7aa0ff,
			lightSkyBlue = 0xffface87,
			skyBlue = 0xffebce87,
			lightGray = 0xffc0c0c0,
			darkGray = 0xff3f3f3f;
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

	class IndexBuffer : public INoncopyable
	{
	private:
		ID3D11Buffer *d3dBuffer;

	public:
		inline IndexBuffer() : d3dBuffer(nullptr) {}
		~IndexBuffer();

		bool Create(Device* device, uint32 size, void* data = nullptr);

		inline ID3D11Buffer* GetID3D11Buffer() { return d3dBuffer; }
	};

	//-----------------------------------Device-------------------------------------//

	struct VertexColor	//12 bytes, 6 floats
	{
		float32x2 pos;
		coloru32 color;

		inline void set(float32x2 _pos, coloru32 _color) { pos = _pos; color = _color; }
	};
	struct VertexTex	//20 bytes, 5 floats
	{
		float32x2 pos;
		float32x2 tex;
		float alpha;

		inline void set(float32x2 _pos, float32x2 _tex, float _alpha) { pos = _pos; tex = _tex; alpha = _alpha; }
	};
	struct VertexEllipse	//32 bytes, 14 floats
	{
		float32x2 pos;
		float32x2 tex;
		float outerRadius, innerRadius;
		coloru32 outerColor, innerColor;

		inline void set(float32x2 _pos, float32x2 _tex, float _outerRadius, float _innerRadius, coloru32 _outerColor, coloru32 _innerColor)
		{ pos = _pos; tex = _tex; outerRadius = _outerRadius; innerRadius = _innerRadius; outerColor = _outerColor; innerColor = _innerColor; }
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

		uint32 vertexBufferSize;
		ID3D11Buffer *d3dVertexBuffer;
		ID3D11Buffer *d3dTransformVSCB;

		ID3D11RasterizerState *d3dDefaultRS;
		ID3D11SamplerState *d3dDefaultSS;
		ID3D11BlendState *d3dAlphaBS;

		inline void setD3DDeviceContextStates(ID3D11InputLayout* d3dInputLayout,
			ID3D11VertexShader* d3dVertexShader, ID3D11PixelShader* d3dPixelShader, uint32 vertexSize);

		IndexBuffer quadIndexBuffer;
		float aaPixelSize;

	public:
		static const uint32 defaultVertexBufferSize = 0x10000;

		Device();
		~Device();

		bool Create(uint32 _vertexBufferSize = defaultVertexBufferSize);

		void SetTarget(IRenderTarget* target);
		void SetTexture(IShaderResource* texture);
		void SetIndexBuffer(IndexBuffer* indexBuffer);
		void SetTransform(const matrix3x2& _transform);
		inline void SetTransform(const matrix3x2& transform, float _aaPixelSize)
		{
			aaPixelSize = _aaPixelSize;
			SetTransform(transform);
		}

		void Clear(IRenderTarget* target, coloru32 color);
		void Clear(coloru32 color);

		void UpdateTexture(ITexture* texture, rectu32* rect, void* data);
		void UpdateIndexBuffer(IndexBuffer* indexBuffer, uint32 left, uint32 right, void* data);
		void UpdateVertexBuffer(VertexColor* vertices, uint32 vertexCount);
		void UpdateVertexBuffer(VertexTex* vertices, uint32 vertexCount);
		void UpdateVertexBuffer(VertexEllipse* vertices, uint32 vertexCount);

		void Draw(uint32 vertexCount);
		void DrawIndexed(uint32 indexCount);

		template <typename VertexType>
		inline void Draw(VertexType* vertices, uint32 vertexCount)
		{
			UpdateVertexBuffer(vertices, vertexCount);
			Draw(vertexCount);
		}
		template <typename VertexType>
		inline void DrawIndexed(VertexType* vertices, uint32 vertexCount, uint32 indexCount)
		{
			UpdateVertexBuffer(vertices, vertexCount);
			DrawIndexed(indexCount);
		}
		template <typename VertexType>
		inline void DrawIndexed(VertexType* vertices, uint32 vertexCount, IndexBuffer* indexBuffer, uint32 indexCount)
		{
			UpdateVertexBuffer(vertices, vertexCount);
			UpdateIndexBuffer(indices, indexCount);
			DrawIndexed(indexCount);
		}

		IndexBuffer* GetQuadIndexBuffer();

		static inline IDXGIFactory3* GetDXGIFactory() { return dxgiFactory; }
		inline ID3D11Device* GetD3DDevice() { return d3dDevice; }
		inline ID3D11DeviceContext* GetD3DDeviceContext() { return d3dContext; }
		inline uint32 GetVertexBufferSize() { return vertexBufferSize; }
		inline float GetAAPixelSize() { return aaPixelSize; }
		inline bool IsInitialized() { return d3dDevice ? true : false; }
	};

	//-----------------------------------Batches-------------------------------------//

	enum class LineGradientType : uint8
	{
		None = 0,
		LeftToRight = 1,
		BeginToEnd = 2,
	};

	inline rectf32 circle(float32x2 center, float radius)
	{
		return rectf32(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
	}
	inline rectf32 ellipse(float32x2 center, float radiusx, float radiusy)
	{
		return rectf32(center.x - radiusx, center.y - radiusy, center.x + radiusx, center.y + radiusy);
	}

	template <uintptr size>
	class LocalMemoryBuffer
	{
	private:
		uint8 buffer[size];

	public:
		void* GetPointer() { return buffer; }
		uintptr GetSize() { return size; }
	};

	class Batch : public INoncopyable
	{
	private:
		Device *device;

		void* vertexBuffer;
		uint32 vertexBufferSize, vertexCount;
		enum class Effect : uint8
		{
			None = 0,
			Color = 1,
			Tex = 2,
			Ellipse = 3,
		} effect;
		bool quadIndexationEnabled;
		bool privateMemoryBuffer;

		inline void checkState(Effect _effect, uint32 quadIndexedVertexCount,
			uint32 quadUnindexedVertexCount, uint32 vertexSize)
		{
			if (effect != _effect && vertexCount)
				Flush();
			else if (vertexCount + (quadIndexationEnabled ? quadIndexedVertexCount : quadUnindexedVertexCount) > vertexBufferSize / vertexSize)
				Flush();
			effect = _effect;
		}

	public:
		Batch();
		~Batch();

		inline Batch(Device *_device, uint32 _vertexBufferSize = 0) { Create(_device, _vertexBufferSize); }
		inline Batch(Device *_device, void* _vertexBuffer, uint32 _vertexBufferSize) { Create(_device, _vertexBuffer, _vertexBufferSize); }

		void Create(Device *_device, uint32 _vertexBufferSize = 0);
		void Create(Device *_device, void* _vertexBuffer, uint32 _vertexBufferSize);

		inline void Flush()
		{
			if (vertexCount)
			{
				if (quadIndexationEnabled)
				{
					device->SetIndexBuffer(device->GetQuadIndexBuffer());
					switch (effect)
					{
					case Effect::Color:
						device->DrawIndexed((VertexColor*) vertexBuffer, vertexCount, vertexCount / 4 * 6);
						break;
					case Effect::Tex:
						device->DrawIndexed((VertexTex*) vertexBuffer, vertexCount, vertexCount / 4 * 6);
						break;
					case Effect::Ellipse:
						device->DrawIndexed((VertexEllipse*) vertexBuffer, vertexCount, vertexCount / 4 * 6);
						break;
					}
				}
				else
				{
					switch (effect)
					{
					case Effect::Color:
						device->Draw((VertexColor*) vertexBuffer, vertexCount);
						break;
					case Effect::Tex:
						device->Draw((VertexTex*) vertexBuffer, vertexCount);
						break;
					case Effect::Ellipse:
						device->Draw((VertexEllipse*) vertexBuffer, vertexCount);
						break;
					}
				}
				vertexCount = 0;
			}
		}

		inline void PushRectangle(const rectf32& rect, coloru32 color)
		{
			checkState(Effect::Color, 4, 6, sizeof(VertexColor));
			VertexColor* vb = (VertexColor*) vertexBuffer + vertexCount;
			if (quadIndexationEnabled)
			{
				vb[0].set(float32x2(rect.left, rect.top), color);
				vb[1].set(float32x2(rect.right, rect.top), color);
				vb[2].set(float32x2(rect.right, rect.bottom), color);
				vb[3].set(float32x2(rect.left, rect.bottom), color);
				vertexCount += 4;
			}
			else
			{
				vb[0].set(float32x2(rect.left, rect.top), color);
				vb[1].set(float32x2(rect.left, rect.bottom), color);
				vb[2].set(float32x2(rect.right, rect.top), color);
				vb[3].set(float32x2(rect.right, rect.top), color);
				vb[4].set(float32x2(rect.left, rect.bottom), color);
				vb[5].set(float32x2(rect.right, rect.bottom), color);
				vertexCount += 6;
			}
		}
		inline void PushGradientEllipse(const rectf32& rect, coloru32 innerColor, coloru32 outerColor, float innerRadius = -0.0f, float outerRadius = 1.0f)
		{
			checkState(Effect::Ellipse, 4, 6, sizeof(VertexEllipse));
			VertexEllipse* vb = (VertexEllipse*) vertexBuffer + vertexCount;
			if (quadIndexationEnabled)
			{
				vb[0].set(float32x2(rect.left, rect.top), float32x2(-1.0f, -1.0f), outerRadius, innerRadius, outerColor, innerColor);
				vb[1].set(float32x2(rect.right, rect.top), float32x2(1.0f, -1.0f), outerRadius, innerRadius, outerColor, innerColor);
				vb[2].set(float32x2(rect.right, rect.bottom), float32x2(1.0f, 1.0f), outerRadius, innerRadius, outerColor, innerColor);
				vb[3].set(float32x2(rect.left, rect.bottom), float32x2(-1.0f, 1.0f), outerRadius, innerRadius, outerColor, innerColor);
				vertexCount += 4;
			}
			else
			{
				vb[0].set(float32x2(rect.left, rect.top), float32x2(-1.0f, -1.0f), outerRadius, innerRadius, outerColor, innerColor);
				vb[1].set(float32x2(rect.left, rect.bottom), float32x2(-1.0f, 1.0f), outerRadius, innerRadius, outerColor, innerColor);
				vb[2].set(float32x2(rect.right, rect.top), float32x2(1.0f, -1.0f), outerRadius, innerRadius, outerColor, innerColor);
				vb[3].set(float32x2(rect.right, rect.top), float32x2(1.0f, -1.0f), outerRadius, innerRadius, outerColor, innerColor);
				vb[4].set(float32x2(rect.left, rect.bottom), float32x2(-1.0f, 1.0f), outerRadius, innerRadius, outerColor, innerColor);
				vb[5].set(float32x2(rect.right, rect.bottom), float32x2(1.0f, 1.0f), outerRadius, innerRadius, outerColor, innerColor);
				vertexCount += 6;
			}
		}
		inline void PushEllipse(const rectf32& rect, coloru32 color, float innerRadius = -0.0f, float outerRadius = 1.0f)
		{
			PushGradientEllipse(rect, color, color, innerRadius, outerRadius);
		}
		inline void PushCircleAA(float32x2 center, float radius, coloru32 color, float innerRadius = -0.0f)
		{
			float halfPixel = device->GetAAPixelSize() / 2.0f;
			float halfPixelByRadius = halfPixel / (radius + halfPixel);
			rectf32 rect = circle(center, radius + halfPixel);
			coloru32 tansparentColor(color.rgba, uint8(0));
			if (innerRadius > 0.0f)
			{
				innerRadius *= radius / (radius + halfPixel);
				PushGradientEllipse(rect, tansparentColor, color, innerRadius - halfPixelByRadius, innerRadius + halfPixelByRadius);
				PushGradientEllipse(rect, color, color, innerRadius + halfPixelByRadius, 1.0f - halfPixelByRadius * 2.0f);
			}
			else
			{
				PushGradientEllipse(rect, color, color, -0.0f, 1.0f - halfPixelByRadius * 2.0f);
			}
			PushGradientEllipse(rect, color, tansparentColor, 1.0f - halfPixelByRadius * 2.0f, 1.0f);
		}
		inline void PushLineAligned(float32x2 begin, float32x2 end, float width, coloru32 color1, coloru32 color2, LineGradientType gradientType)
		{
			float32x2 v = normal(normalize(end - begin)) * (width / 2.0f);
			checkState(Effect::Color, 4, 6, sizeof(VertexColor));
			VertexColor* vb = (VertexColor*) vertexBuffer + vertexCount;
			if (quadIndexationEnabled)
			{
				switch (gradientType)
				{
				case LineGradientType::LeftToRight:
					vb[0].set(begin + v, color1);
					vb[1].set(end + v, color1);
					vb[2].set(end, color2);
					vb[3].set(begin, color2);
					break;
				case LineGradientType::BeginToEnd:
					vb[0].set(begin + v, color1);
					vb[1].set(end + v, color2);
					vb[2].set(end, color2);
					vb[3].set(begin, color1);
					break;
				}
				vertexCount += 4;
			}
			else
			{
				switch (gradientType)
				{
				case LineGradientType::LeftToRight:
					vb[0].set(begin + v, color1);
					vb[1].set(begin, color2);
					vb[2].set(end + v, color1);
					vb[3].set(end + v, color1);
					vb[4].set(begin, color2);
					vb[5].set(end, color2);

					break;
				case LineGradientType::BeginToEnd:
					vb[0].set(begin + v, color1);
					vb[1].set(begin, color1);
					vb[2].set(end + v, color2);
					vb[3].set(end + v, color2);
					vb[4].set(begin, color1);
					vb[5].set(end, color2);
					break;
				}
				vertexCount += 6;
			}
		}
		inline void PushLineAligned(float32x2 begin, float32x2 end, float width, coloru32 color)
		{
			float32x2 v = normal(normalize(end - begin)) * (width / 2.0f);
			checkState(Effect::Color, 4, 6, sizeof(VertexColor));
			VertexColor* vb = (VertexColor*) vertexBuffer + vertexCount;
			if (quadIndexationEnabled)
			{
				vb[0].set(begin + v, color);
				vb[1].set(end + v, color);
				vb[2].set(end, color);
				vb[3].set(begin, color);
				vertexCount += 4;
			}
			else
			{
				vb[0].set(begin + v, color);
				vb[1].set(begin, color);
				vb[2].set(end + v, color);
				vb[3].set(end + v, color);
				vb[4].set(begin, color);
				vb[5].set(end, color);
				vertexCount += 6;
			}
		}
		inline void PushLine(float32x2 begin, float32x2 end, float width, coloru32 color1, coloru32 color2, LineGradientType gradientType)
		{
			float32x2 v = normal(normalize(end - begin)) * (width / 2.0f);
			checkState(Effect::Color, 4, 6, sizeof(VertexColor));
			VertexColor* vb = (VertexColor*) vertexBuffer + vertexCount;
			if (quadIndexationEnabled)
			{
				switch (gradientType)
				{
				case LineGradientType::LeftToRight:
					vb[0].set(begin + v, color1);
					vb[1].set(end + v, color1);
					vb[2].set(end - v, color2);
					vb[3].set(begin - v, color2);
					break;
				case LineGradientType::BeginToEnd:
					vb[0].set(begin + v, color1);
					vb[1].set(end + v, color2);
					vb[2].set(end - v, color2);
					vb[3].set(begin - v, color1);
					break;
				}
				vertexCount += 4;
			}
			else
			{
				switch (gradientType)
				{
				case LineGradientType::LeftToRight:
					vb[0].set(begin + v, color1);
					vb[1].set(begin - v, color2);
					vb[2].set(end + v, color1);
					vb[3].set(end + v, color1);
					vb[4].set(begin - v, color2);
					vb[5].set(end - v, color2);

					break;
				case LineGradientType::BeginToEnd:
					vb[0].set(begin + v, color1);
					vb[1].set(begin - v, color1);
					vb[2].set(end + v, color2);
					vb[3].set(end + v, color2);
					vb[4].set(begin - v, color1);
					vb[5].set(end - v, color2);
					break;
				}
				vertexCount += 6;
			}
		}
		inline void PushLine(float32x2 begin, float32x2 end, float width, coloru32 color)
		{
			float32x2 v = normal(normalize(end - begin)) * (width / 2.0f);
			checkState(Effect::Color, 4, 6, sizeof(VertexColor));
			VertexColor* vb = (VertexColor*) vertexBuffer + vertexCount;
			if (quadIndexationEnabled)
			{
				vb[0].set(begin + v, color);
				vb[1].set(end + v, color);
				vb[2].set(end - v, color);
				vb[3].set(begin - v, color);
				vertexCount += 4;
			}
			else
			{
				vb[0].set(begin + v, color);
				vb[1].set(begin - v, color);
				vb[2].set(end + v, color);
				vb[3].set(end + v, color);
				vb[4].set(begin - v, color);
				vb[5].set(end - v, color);
				vertexCount += 6;
			}
		}
		inline void SetQuadIndexationState(bool state)
		{
			if (state != quadIndexationEnabled)
			{
				Flush();
				quadIndexationEnabled = state;
			}
		}

		inline Device* GetDevice() { return device; }
	};

	class TextBatch : public INoncopyable
	{

	};
}