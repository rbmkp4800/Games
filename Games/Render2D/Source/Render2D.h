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
	constexpr float32 opacityThreshold = 0.01f;

	constexpr inline uint32 operator "" _rgb(uint64 val) { return ((val & 0xff) << 16) | (val & 0xff00) | ((val & 0xff0000) >> 16) | 0xff000000; }
	constexpr inline uint32 operator "" _rgba(uint64 val) { return ((val & 0xff) << 24) | ((val & 0xff00) << 8) | ((val & 0xff0000) >> 8) | ((val & 0xff000000) >> 24); }

	struct Color
	{
		union
		{
			struct
			{
				uint8 r, g, b, a;
			};
			uint32 rgba;
		};
		inline Color() : rgba(0) {}
		inline Color(uint32 _rgba) : rgba(_rgba) {}
		inline Color(uint32 rgb, uint8 _a) { rgba = rgb; a = _a; }
		inline Color(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 0xff) : r(_r), g(_g), b(_b), a(_a) {}

		inline void set(uint32 _rgba) { rgba = _rgba; }
		inline void set(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 0xff) { r = _r; g = _g; b = _b; a = _a; }

		inline operator uint32() const { return rgba; }
		inline float32x4 toFloat4Unorm() const { return float32x4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); }
		inline void toFloat4Unorm(float32* result) const { result[0] = r / 255.0f; result[1] = g / 255.0f; result[2] = b / 255.0f; result[3] = a / 255.0f; }
	};

	inline Color lerp(const Color& a, const Color& b, float32 coef)
	{
		return Color(
			uint8(::lerp(float32(a.r), float32(b.r), coef)),
			uint8(::lerp(float32(a.g), float32(b.g), coef)),
			uint8(::lerp(float32(a.b), float32(b.b), coef)),
			uint8(::lerp(float32(a.a), float32(b.a), coef)));
	}
	inline Color lerp_clamp(const Color& a, const Color& b, float32 coef)
	{
		return Color(
			uint8(clamp(::lerp(float32(a.r), float32(b.r), coef), 0.0f, 255.0f)),
			uint8(clamp(::lerp(float32(a.g), float32(b.g), coef), 0.0f, 255.0f)),
			uint8(clamp(::lerp(float32(a.b), float32(b.b), coef), 0.0f, 255.0f)),
			uint8(clamp(::lerp(float32(a.a), float32(b.a), coef), 0.0f, 255.0f)));
	}

	class Colors abstract
	{
	public:
		static constexpr uint32
			Black =			0x000000_rgb,
			White =			0xffffff_rgb,
			Red =			0xff0000_rgb,
			Green =			0x00ff00_rgb,
			Blue =			0x0000ff_rgb,
			Yellow =		0xffff00_rgb,
			Magenta =		0xff00ff_rgb,
			Cyan =			0xffff00_rgb,

			DarkRed =		0x7f0000_rgb,
			DarkGreen =		0x007f00_rgb,
			DarkBlue =		0x00007f_rgb,
			DarkYellow =	0x7f7f00_rgb,
			DarkMagenta =	0x7f007f_rgb,
			DarkCyan =		0x007f7f_rgb,

			LightRed =		0xff7f7f_rgb,
			LightGreen =	0x90ee90_rgb,
			LightBlue =		0xadd8e6_rgb,
			LightYellow =	0xffff7f_rgb,
			LightSalmon =	0xffa07a_rgb,
			LightSkyBlue =	0x87cefa_rgb,
			SkyBlue =		0x87ceeb_rgb,
			DeepSkyBlue =	0x00bfff_rgb,

			Gray =			0x7f7f7f_rgb,
			LightGray =		0xc0c0c0_rgb,
			DarkGray =		0x3f3f3f_rgb,

			CornflowerBlue = 0xffed9564,
			DodgerBlue = 0x1e90ff;
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

		inline bool InitITexture(ID3D11Device* d3dDevice, uint32 width, uint32 height, void* data, uint32 bindFlags);

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
		bool Create(Device* device, uint32 width, uint32 height, void* data = nullptr);
	};

	class RenderTarget : public IShaderResource, public IRenderTarget
	{
	public:
		bool Create(Device* device, uint32 width, uint32 height, void* data = nullptr);
	};

	class SwapChain : public IRenderTarget
	{
	private:
		IDXGISwapChain1 *dxgiSwapChain;

	public:
		inline SwapChain() : dxgiSwapChain(nullptr) {}
		~SwapChain();

		bool CreateForComposition(Device* device, IUnknown* panel, uint32 width, uint32 height);
		bool CreateForHWnd(Device* device, void* hWnd, uint32 width, uint32 height);
		bool Resize(Device* device, uint32 width, uint32 height);
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
		inline bool IsInitialized() { return d3dBuffer ? true : false; }
	};

	class MonospaceFont : public IShaderResource
	{
	public:
		static constexpr uint8 firstCharCode = 33;
		static constexpr uint8 lastCharCode = 255;
		static constexpr uint32 charTableSize = lastCharCode - firstCharCode + 1;
		static constexpr float32 tableCharWidth = 1.0f / float32(charTableSize);

	private:
		float32 width, height;

	public:
		inline MonospaceFont() : width(0.0f), height(0.0f) {}
		~MonospaceFont();

		bool Create(Device* device, uint32 _width, uint32 _height, void* data);

		inline bool IsInitialized() { return d3dShaderResourceView ? true : false; }
		inline float32 GetHeight() { return height; }
		inline float32 GetAspect() { return width / height; }
	};

	//-----------------------------------Device-------------------------------------//

	struct VertexColor	//12 bytes, 6 floats
	{
		float32x2 pos;
		uint32 color;

		inline void set(float32x2 _pos, uint32 _color) { pos = _pos; color = _color; }
	};
	struct VertexTexture	//20 bytes, 5 floats
	{
		float32x2 pos;
		float32x2 tex;
		float32 alpha;

		inline void set(float32x2 _pos, float32x2 _tex, float32 _alpha) { pos = _pos; tex = _tex; alpha = _alpha; }
	};
	struct VertexEllipse	//32 bytes, 14 floats
	{
		float32x2 pos;
		float32x2 tex;
		float32 outerRadius, innerRadius;
		uint32 outerColor, innerColor;

		inline void set(float32x2 _pos, float32x2 _tex, float32 _outerRadius, float32 _innerRadius, uint32 _outerColor, uint32 _innerColor)
		{ pos = _pos; tex = _tex; outerRadius = _outerRadius; innerRadius = _innerRadius; outerColor = _outerColor; innerColor = _innerColor; }
	};
	struct VertexTexColor	//20 bytes, 8 floats
	{
		float32x2 pos;
		float32x2 tex;
		uint32 color;

		inline void set(float32x2 _pos, float32x2 _tex, uint32 _color) { pos = _pos; tex = _tex; color = _color; }
	};

	class Device : public INoncopyable
	{
	private:
		static IDXGIFactory3 *dxgiFactory;

		ID3D11Device *d3dDevice;
		ID3D11DeviceContext *d3dContext;

		ID3D11InputLayout *d3dColorIL, *d3dTextureIL, *d3dEllipseIL, *d3dTexColorIL;
		ID3D11VertexShader *d3dColorVS, *d3dTextureVS, *d3dEllipseVS, *d3dTexColorVS;
		ID3D11PixelShader *d3dColorPS, *d3dTexturePS, *d3dEllipsePS, *d3dTexColorPS;

		uint32 vertexBufferSize;
		ID3D11Buffer *d3dVertexBuffer;
		ID3D11Buffer *d3dTransformVSCB;

		ID3D11RasterizerState *d3dDefaultRS;
		ID3D11SamplerState *d3dDefaultSS;
		ID3D11BlendState *d3dAlphaBS;

		inline void setD3DDeviceContextStates(ID3D11InputLayout* d3dInputLayout,
			ID3D11VertexShader* d3dVertexShader, ID3D11PixelShader* d3dPixelShader, uint32 vertexSize);

		IndexBuffer quadIndexBuffer;
		MonospaceFont defaultFont;

		IRenderTarget *currentTarget;
		float32 aaPixelSize;

	public:
		static constexpr uint32 defaultVertexBufferSize = 0x10000;

		Device();
		~Device();

		bool Create(uint32 _vertexBufferSize = defaultVertexBufferSize);

		void SetTarget(IRenderTarget* target);
		void ResetTarget();
		void SetTexture(IShaderResource* texture);
		void SetIndexBuffer(IndexBuffer* indexBuffer);
		void SetTransform(const matrix3x2& _transform);
		void SetDirectTransform();
		inline void SetTransform(const matrix3x2& transform, float32 _aaPixelSize) { aaPixelSize = _aaPixelSize; SetTransform(transform); }

		void Clear(IRenderTarget* target, Color color);
		inline void Clear(Color color) { Clear(currentTarget, color); }

		void UpdateTexture(ITexture* texture, rectu32* rect, void* data);
		void UpdateIndexBuffer(IndexBuffer* indexBuffer, uint32 left, uint32 right, void* data);
		void UpdateVertexBuffer(VertexColor* vertices, uint32 vertexCount);
		void UpdateVertexBuffer(VertexTexture* vertices, uint32 vertexCount);
		void UpdateVertexBuffer(VertexEllipse* vertices, uint32 vertexCount);
		void UpdateVertexBuffer(VertexTexColor* vertices, uint32 vertexCount);

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
		MonospaceFont *GetDefaultFont();

		static inline IDXGIFactory3* GetDXGIFactory() { return dxgiFactory; }
		inline ID3D11Device* GetD3DDevice() { return d3dDevice; }
		inline ID3D11DeviceContext* GetD3DDeviceContext() { return d3dContext; }
		inline uint32 GetVertexBufferSize() { return vertexBufferSize; }
		inline float32 GetAAPixelSize() { return aaPixelSize; }
		inline bool IsInitialized() { return d3dDevice ? true : false; }
	};

	//-----------------------------------Batches-------------------------------------//

	enum class GradientType : uint8
	{
		None = 0,
		Horizontal = 1,
		Vertical = 2,
	};

	constexpr inline rectf32 circle(float32x2 center, float32 radius)
	{
		return rectf32(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
	}
	constexpr inline rectf32 ellipse(float32x2 center, float32 radiusx, float32 radiusy)
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
		constexpr uintptr GetSize() { return size; }
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
			TexColor = 4,
		} effect;
		IShaderResource *currentTexture;
		bool quadIndexationEnabled;
		bool privateMemoryBuffer;

		inline void checkState(Effect _effect, uint32 quadIndexedVertexCount, uint32 quadUnindexedVertexCount, uint32 vertexSize)
		{
			if (effect != _effect)
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
						device->SetTexture(currentTexture);
						device->DrawIndexed((VertexTexture*) vertexBuffer, vertexCount, vertexCount / 4 * 6);
						break;
					case Effect::Ellipse:
						device->DrawIndexed((VertexEllipse*) vertexBuffer, vertexCount, vertexCount / 4 * 6);
						break;
					case Effect::TexColor:
						device->SetTexture(currentTexture);
						device->DrawIndexed((VertexTexColor*)vertexBuffer, vertexCount, vertexCount / 4 * 6);
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
						device->SetTexture(currentTexture);
						device->Draw((VertexTexture*) vertexBuffer, vertexCount);
						break;
					case Effect::Ellipse:
						device->Draw((VertexEllipse*) vertexBuffer, vertexCount);
						break;
					case Effect::TexColor:
						device->SetTexture(currentTexture);
						device->Draw((VertexTexColor*)vertexBuffer, vertexCount);
						break;
					}
				}
				vertexCount = 0;
			}
		}

		inline void PushRectangle(const rectf32& rect, Color color)
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
		inline void PushGradientRect(const rectf32& rect, Color color1, Color color2, GradientType gradientType)
		{
			checkState(Effect::Color, 4, 6, sizeof(VertexColor));
			VertexColor* vb = (VertexColor*) vertexBuffer + vertexCount;
			if (quadIndexationEnabled)
			{
				switch (gradientType)
				{
				case GradientType::Horizontal:
					vb[0].set(float32x2(rect.left, rect.top), color1);
					vb[1].set(float32x2(rect.right, rect.top), color2);
					vb[2].set(float32x2(rect.right, rect.bottom), color2);
					vb[3].set(float32x2(rect.left, rect.bottom), color1);
					break;
				case GradientType::Vertical:
					vb[0].set(float32x2(rect.left, rect.top), color2);
					vb[1].set(float32x2(rect.right, rect.top), color2);
					vb[2].set(float32x2(rect.right, rect.bottom), color1);
					vb[3].set(float32x2(rect.left, rect.bottom), color1);
					break;
				}
				vertexCount += 4;
			}
			else
			{
				switch (gradientType)
				{
				case GradientType::Horizontal:
					vb[0].set(float32x2(rect.left, rect.top), color1);		//0
					vb[3].set(float32x2(rect.left, rect.bottom), color1);	//3
					vb[1].set(float32x2(rect.right, rect.top), color2);		//1
					vb[1].set(float32x2(rect.right, rect.top), color2);		//1
					vb[3].set(float32x2(rect.left, rect.bottom), color1);	//3
					vb[2].set(float32x2(rect.right, rect.bottom), color2);	//2
					break;
				case GradientType::Vertical:
					vb[0].set(float32x2(rect.left, rect.top), color2);		//0
					vb[3].set(float32x2(rect.left, rect.bottom), color1);	//3
					vb[1].set(float32x2(rect.right, rect.top), color2);		//1
					vb[1].set(float32x2(rect.right, rect.top), color2);		//1
					vb[3].set(float32x2(rect.left, rect.bottom), color1);	//3
					vb[2].set(float32x2(rect.right, rect.bottom), color1);	//2
					break;
				}
			}
		}
		inline void PushGradientEllipse(const rectf32& rect, Color innerColor, Color outerColor, float32 innerRadius = -0.0f, float32 outerRadius = 1.0f)
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
		inline void PushEllipse(const rectf32& rect, Color color, float32 innerRadius = -0.0f, float32 outerRadius = 1.0f)
		{
			PushGradientEllipse(rect, color, color, innerRadius, outerRadius);
		}
		inline void PushCircleAA(float32x2 center, float32 radius, Color color, float32 innerRadius = -0.0f)
		{
			float32 halfPixel = device->GetAAPixelSize() / 2.0f;
			float32 halfPixelByRadius = halfPixel / (radius + halfPixel);
			rectf32 rect = circle(center, radius + halfPixel);
			if (innerRadius > 0.0f)
			{
				innerRadius *= radius / (radius + halfPixel);
				PushGradientEllipse(rect, Color(color, 0), color, innerRadius - halfPixelByRadius, innerRadius + halfPixelByRadius);
				PushGradientEllipse(rect, color, color, innerRadius + halfPixelByRadius, 1.0f - halfPixelByRadius * 2.0f);
			}
			else
			{
				PushGradientEllipse(rect, color, color, -0.0f, 1.0f - halfPixelByRadius * 2.0f);
			}
			PushGradientEllipse(rect, color, Color(color, 0), 1.0f - halfPixelByRadius * 2.0f, 1.0f);
		}
		inline void PushLineAligned(float32x2 begin, float32x2 end, float32 width, Color color1, Color color2, GradientType gradientType)
		{
			float32x2 v = normal(normalize(end - begin)) * (width / 2.0f);
			checkState(Effect::Color, 4, 6, sizeof(VertexColor));
			VertexColor* vb = (VertexColor*) vertexBuffer + vertexCount;
			if (quadIndexationEnabled)
			{
				switch (gradientType)
				{
				case GradientType::Horizontal:
					vb[0].set(begin + v, color1);
					vb[1].set(end + v, color1);
					vb[2].set(end, color2);
					vb[3].set(begin, color2);
					break;
				case GradientType::Vertical:
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
				case GradientType::Horizontal:
					vb[0].set(begin + v, color1);
					vb[1].set(begin, color2);
					vb[2].set(end + v, color1);
					vb[3].set(end + v, color1);
					vb[4].set(begin, color2);
					vb[5].set(end, color2);

					break;
				case GradientType::Vertical:
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
		inline void PushLineAligned(float32x2 begin, float32x2 end, float32 width, Color color)
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
		inline void PushLine(float32x2 begin, float32x2 end, float32 width, Color color1, Color color2, GradientType gradientType)
		{
			float32x2 v = normal(normalize(end - begin)) * (width / 2.0f);
			checkState(Effect::Color, 4, 6, sizeof(VertexColor));
			VertexColor* vb = (VertexColor*) vertexBuffer + vertexCount;
			if (quadIndexationEnabled)
			{
				switch (gradientType)
				{
				case GradientType::Horizontal:
					vb[0].set(begin + v, color1);
					vb[1].set(end + v, color1);
					vb[2].set(end - v, color2);
					vb[3].set(begin - v, color2);
					break;
				case GradientType::Vertical:
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
				case GradientType::Horizontal:
					vb[0].set(begin + v, color1);
					vb[1].set(begin - v, color2);
					vb[2].set(end + v, color1);
					vb[3].set(end + v, color1);
					vb[4].set(begin - v, color2);
					vb[5].set(end - v, color2);

					break;
				case GradientType::Vertical:
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
		inline void PushLine(float32x2 begin, float32x2 end, float32 width, Color color)
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

		inline void PushText(MonospaceFont* font, float32x2 position, float32 height, char *string, uint32 length = uint32(-1), Color color = 0xffffffff)
		{
			if (!quadIndexationEnabled)
				return;
			if (effect != Effect::TexColor || currentTexture != font)
			{
				Flush();
				effect = Effect::TexColor;
				currentTexture = font;
			}
			float32 charWidth = height * font->GetAspect();
			float32 leftBorder = position.x;
			for (uint32 i = 0; i < length; i++)
			{
				uint8 character = string[i];
				if (!character)
					break;
				if (character == ' ')
				{
					position.x += charWidth;
					continue;
				}
				if (character == '\n')
				{
					position.y += height;
					position.x = leftBorder;
					continue;
				}
				if (character >= MonospaceFont::firstCharCode)
				{
					character -= MonospaceFont::firstCharCode;
					if (vertexCount + 4 > vertexBufferSize / sizeof(VertexTexColor))
						Flush();
					VertexTexColor* vb = (VertexTexColor*)vertexBuffer + vertexCount;
					float32 charTexCoord = MonospaceFont::tableCharWidth * float32(character);
					vb[0].set(position, float32x2(charTexCoord, 0.0f), color);
					vb[3].set(float32x2(position.x, position.y + height), float32x2(charTexCoord, 1.0f), color);
					position.x += charWidth;
					charTexCoord += MonospaceFont::tableCharWidth;
					vb[1].set(position, float32x2(charTexCoord, 0.0f), color);
					vb[2].set(float32x2(position.x, position.y + height), float32x2(charTexCoord, 1.0f), color);
					vertexCount += 4;
				}
			}
		}
		inline void PushText(MonospaceFont* font, float32x2 position, char *string, uint32 length = uint32(-1), Color color = 0xffffffff)
		{
			PushText(font, position, font->GetHeight(), string, length, color);
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
}