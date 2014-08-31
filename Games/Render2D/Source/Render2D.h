#include <extypes.h>
#include <extypes.util.h>
#include <extypes.vectors.h>
#include <extypes.vectors.math.h>
#include <extypes.matrix3x2.h>

struct IUnknown;

struct ID3D11Device2;
struct ID3D11DeviceContext2;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

struct IDXGIFactory3;
struct IDXGIDevice1;
struct IDXGISwapChain1;

namespace Render2D
{
	class Render;

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

	class INoncopyable abstract
	{
	private:
		inline INoncopyable(const INoncopyable&);
		inline INoncopyable& operator= (const INoncopyable&);

	public:
		inline INoncopyable() {}
		inline ~INoncopyable() {}
	};

	class ITexture abstract : public INoncopyable
	{
		friend Render;

	protected:
		ID3D11Texture2D *d3dTexture;

		inline bool InitITexture(uint32 x, uint32 y, void* data, uint32 bindFlags);

	public:
		inline ITexture() : d3dTexture(nullptr) {}
		~ITexture();

		uint32x2 GetSize();
		void Update(rectu32* rect, void* data);
	};

	class IShaderResource abstract : virtual public ITexture
	{
		friend Render;

	protected:
		ID3D11ShaderResourceView *d3dShaderResourceView;

		inline bool InitIShaderResource();

	public:
		inline IShaderResource() : d3dShaderResourceView(nullptr) {}
		~IShaderResource();
	};

	class IRenderTarget abstract : virtual public ITexture
	{
		friend Render;

	protected:
		ID3D11RenderTargetView *d3dRenderTargetView;

		inline bool InitIRenderTarget();

	public:
		inline IRenderTarget() : d3dRenderTargetView(nullptr) {}
		~IRenderTarget();
	};

	class Texture : public IShaderResource
	{
		friend Render;

	public:
		bool Create(uint32 x, uint32 y, void* data = nullptr);
		bool CreateFromPng(wchar_t* filename);
		inline bool Create(uint32x2 size, void* data = nullptr) { return Create(size.x, size.y, data); }
		inline void Release() { this->~Texture(); }
	};

	class RenderTarget : public IShaderResource, public IRenderTarget
	{
		friend Render;

	public:
		bool Create(uint32 x, uint32 y, void* data = nullptr);
		inline bool Create(uint32x2 size, void* data = nullptr) { return Create(size.x, size.y, data); }
		inline void Release() { this->~RenderTarget(); }
	};

	class SwapChain : public IRenderTarget
	{
		friend Render;

	private:
		IDXGISwapChain1 *dxgiSwapChain;

	public:
		inline SwapChain() : dxgiSwapChain(nullptr) {}
		~SwapChain();

		bool CreateForComposition(IUnknown* panel, uint32 x, uint32 y);
		bool CreateForHWnd(void* hWnd, uint32 x, uint32 y);
		inline bool CreateForComposition(IUnknown* panel, uint32x2 size) { return CreateForComposition(panel, size.x, size.y); }
		inline bool CreateForHWnd(void* hWnd, uint32x2 size) { return CreateForHWnd(hWnd, size.x, size.y); }
		inline void Release() { this->~SwapChain(); }
		bool Resize(uint32 x, uint32 y);
		inline bool Resize(uint32x2 size) { return Resize(size.x, size.y); }
		void Present(bool sync = true);
	};

	class TextFormat : public INoncopyable
	{
		friend Render;

	private:
		static const uint32 height = 14, width = 8;

		Texture texture;

	public:
		inline TextFormat() {}

		bool Create();
	};

	//---------------------------------Private--------------------------------------//

	class _private abstract  
	{
		friend Render;

	private:
		template <uint32 a = 0, uint32 b = 1, uint32 c = 2, uint32 d = 3, typename VertexType>
		static inline void fillIndexedVerticesPosByRect(VertexType* vertexBuffer, const rectf32& rect)
		{
			vertexBuffer[a].pos.set(rect.left, rect.top);
			vertexBuffer[b].pos.set(rect.right, rect.top);
			vertexBuffer[c].pos.set(rect.right, rect.bottom);
			vertexBuffer[d].pos.set(rect.left, rect.bottom);
		}
		template <uint32 a = 0, uint32 b = 1, uint32 c = 2, uint32 d = 3, typename VertexType>
		static inline void fillIndexedVerticesPosByLine(VertexType* vertexBuffer, float32x2 start, float32x2 end, float width)
		{
			float32x2 v = end - start;
			float tmp = v.x; v.x = v.y; v.y = -tmp;
			v.normalize();
			v *= width / 2.0f;
			vertexBuffer[a].pos = start + v;
			vertexBuffer[b].pos = end + v;
			vertexBuffer[c].pos = end - v;
			vertexBuffer[d].pos = start - v;
		}
		template <bool swapDim = false, uint32 a = 0, uint32 b = 1, uint32 c = 2, uint32 d = 3, typename VertexType>
		static inline void fillIndexedVerticesTexByRect(VertexType* vertexBuffer, rectf32* rect)
		{
			if (rect)
			{
				if (swapDim)
				{
					vertexBuffer[a].tex.set(rect->top, rect->left);
					vertexBuffer[b].tex.set(rect->top, rect->right);
					vertexBuffer[c].tex.set(rect->bottom, rect->right);
					vertexBuffer[d].tex.set(rect->bottom, rect->left);
				}
				else
				{
					vertexBuffer[a].tex.set(rect->left, rect->top);
					vertexBuffer[b].tex.set(rect->right, rect->top);
					vertexBuffer[c].tex.set(rect->right, rect->bottom);
					vertexBuffer[d].tex.set(rect->left, rect->bottom);
				}
			}
			else
			{
				if (swapDim)
				{
					vertexBuffer[a].tex.set(0.0f, 0.0f);
					vertexBuffer[b].tex.set(0.0f, 1.0f);
					vertexBuffer[c].tex.set(1.0f, 1.0f);
					vertexBuffer[d].tex.set(1.0f, 0.0f);
				}
				else
				{
					vertexBuffer[a].tex.set(0.0f, 0.0f);
					vertexBuffer[b].tex.set(1.0f, 0.0f);
					vertexBuffer[c].tex.set(1.0f, 1.0f);
					vertexBuffer[d].tex.set(0.0f, 1.0f);
				}
			}
		}
		template <uint32 a = 0, uint32 b = 1, uint32 c = 2, uint32 d = 3, typename VertexType>
		static inline void fillIndexedVerticesOpacity(VertexType* vertexBuffer, float startOpacity, float endOpacity, Direction gradientDirection)
		{
			switch (gradientDirection)
			{
			case Direction::Right:
				vertexBuffer[a].opacity = startOpacity;
				vertexBuffer[b].opacity = endOpacity;
				vertexBuffer[c].opacity = endOpacity;
				vertexBuffer[d].opacity = startOpacity;
				break;

			case Direction::Left:
				vertexBuffer[a].opacity = endOpacity;
				vertexBuffer[b].opacity = startOpacity;
				vertexBuffer[c].opacity = startOpacity;
				vertexBuffer[d].opacity = endOpacity;
				break;

			case Direction::Down:
				vertexBuffer[a].opacity = startOpacity;
				vertexBuffer[b].opacity = startOpacity;
				vertexBuffer[c].opacity = endOpacity;
				vertexBuffer[d].opacity = endOpacity;
				break;

			case Direction::Up:
				vertexBuffer[a].opacity = endOpacity;
				vertexBuffer[b].opacity = endOpacity;
				vertexBuffer[c].opacity = startOpacity;
				vertexBuffer[d].opacity = startOpacity;
				break;
			}
		}

		template <typename VertexType>
		static inline void fillVerticesPosByRect(VertexType* vertexBuffer, const rectf32& rect)
		{
			fillIndexedVerticesPosByRect<0, 1, 4, 2>(vertexBuffer, rect);
			vertexBuffer[3].pos = vertexBuffer[1].pos;
			vertexBuffer[5].pos = vertexBuffer[2].pos;
		}
		template <typename VertexType>
		static inline void fillVerticesPosByLine(VertexType* vertexBuffer, float32x2 start, float32x2 end, float width)
		{
			fillIndexedVerticesPosByLine<0, 1, 4, 2>(vertexBuffer, start, end, width);
			vertexBuffer[3].pos = vertexBuffer[1].pos;
			vertexBuffer[5].pos = vertexBuffer[2].pos;
		}
		template <bool swapDim = false, typename VertexType>
		static inline void fillVerticesTexByRect(VertexType* vertexBuffer, rectf32* rect)
		{
			fillIndexedVerticesTexByRect<swapDim, 0, 1, 4, 2>(vertexBuffer, rect);
			vertexBuffer[3].tex = vertexBuffer[1].tex;
			vertexBuffer[5].tex = vertexBuffer[2].tex;
		}
		template <typename VertexType>
		static inline void fillVerticesOpacity(VertexType* vertexBuffer, float startOpacity, float endOpacity, Direction gradientDirection)
		{
			fillIndexedVerticesOpacity<0, 1, 4, 2>(vertexBuffer, startOpacity, endOpacity, gradientDirection);
			vertexBuffer[3].opacity = vertexBuffer[1].opacity;
			vertexBuffer[5].opacity = vertexBuffer[2].opacity;
		}
	};

	//-----------------------------------Render-------------------------------------//

	enum class SamplerMode : uint8
	{
		None = 0,
		Linear_TextAddressMirrorOnce = 1,
		Linear_TextAddressWrap = 2,
		Default = Linear_TextAddressMirrorOnce,
	};
	enum class BlendState : uint8
	{
		None = 0,
		AlphaBlend = 1,
		Clear = 2,
		Default = AlphaBlend,
	};

	class Render abstract
	{
		friend ITexture;
		friend IShaderResource;
		friend IRenderTarget;

		friend Texture;
		friend RenderTarget;
		friend SwapChain;

	private:
		struct DXResources
		{
			ID3D11Device2 *d3dDevice;
			ID3D11DeviceContext2 *d3dContext;
			IDXGIFactory3 *dxgiFactory;
			IDXGIDevice1 *dxgiDevice;

			ID3D11InputLayout *d3dILBasic;
			ID3D11VertexShader *d3dVSBasic;
			ID3D11PixelShader *d3dPSBasic;
			ID3D11Buffer *d3dVertexBuffer, *d3dIndexBuffer, *d3dVSCBTransform;
			ID3D11BlendState *d3dBlendStateAlpha, *d3dBlendStateClear;
			ID3D11SamplerState *d3dSamplerStateLinearMirrorOnce, *d3dSamplerStateLinearWrap;
			ID3D11RasterizerState *d3dRasterizerState;
		} static dx;

		struct VSCBTransform
		{
			float32x3 transformMatrixRow1;
			uint32 _padding1;
			float32x3 transformMatrixRow2;
			uint32 _padding2;
			float32x2 scale;
			uint32 _padding3, _padding4;

			inline void setTransformMatrix(const matrix3x2& matrix);
			inline VSCBTransform();
		} static vscbTransform;

		struct VertexColor		//24 bytes
		{
			float32x2 pos;
			float32x4 color;
		};
		struct VertexTex		//24 bytes
		{
			float32x2 pos;
			float32x2 tex;
			float opactity;
			float texIdx;
		};
		struct VertexEllipse	//36 bytes
		{
			float32x2 pos;
			float32x2 tex;
			float32x4 color;
			float innerRadius;
		};

		static const uint32 vertexBufferPageSize = 1 << 12;
		static const uint32 vertexBufferPagesCount = 1 << 6;
		static const uint32 verticesColorPerPage = vertexBufferPageSize / sizeof(VertexColor);
		static const uint32 verticesTexPerPage = vertexBufferPageSize / sizeof(VertexTex);
		static const uint32 verticesEllipsePerPage = vertexBufferPageSize / sizeof(VertexEllipse);

		static const uint32 indexedPrimitivesLimit = vertexLimit / 2;
		static const uint32 textureSlotsCount = 4;

		union VertexBufferPage
		{
			uint8 raw[vertexBufferPageSize];
			VertexColor vertexColorBuffer[verticesColorPerPage];
			VertexTex vertexTexBuffer[verticesTexPerPage];
			VertexEllipse vertexEllipseBuffer[verticesEllipsePerPage];
		};

		static VertexBufferPage vertexBuffer[vertexBufferPagesCount];
		static uint32 vertexCount;
		static ID3D11RenderTargetView *d3dRenderTargetView;
		static ID3D11ShaderResourceView *d3dTextureSlotsSRVBuffer[textureSlotsCount];
		static bool indexationEnabled;

		static inline void setDeviceStates(ID3D11InputLayout *_d3dInputLayout, ID3D11VertexShader *_d3dVertexShader,
			ID3D11PixelShader *_d3dPixelShader, uint32 _vertexBufferStride);
		static inline int32 pushTextureInSlot(ID3D11ShaderResourceView* d3dSRV)
		{
			for (int32 i = 0; i < textureSlotsCount; i++)
			{
				if (d3dTextureSlotsSRVBuffer[i] == d3dSRV)
				{
					return i;
				}
				else if (!d3dTextureSlotsSRVBuffer[i])
				{
					d3dTextureSlotsSRVBuffer[i] = d3dSRV;
					return i;
				}
			}
			Flush();
			d3dTextureSlotsSRVBuffer[0] = d3dSRV;
			return 0;
		}

	public:
		static void Init();
		//static void Destroy();

		static void Flush();
		static void Discard();

		static void SetTarget(IRenderTarget *target);
		static void ResetTarget();
		static void Clear(IRenderTarget* target, coloru32 color);
		static void Clear(coloru32 color);
		static void SetTransform(const matrix3x2& transform);

		static void SetSamplerMode(SamplerMode mode);
		static void SetBlendState(BlendState state);
		inline static void EnableIndexation(bool state)
		{
			if (indexationEnabled != state)
			{
				Flush();
				indexationEnabled = state;
			}
		}

		template <bool swapSrcDim = false>
		static inline void DrawBitmap(IShaderResource* bitmap, rectf32* destRect, rectf32* srcRect = nullptr, float opacity = 1.0f)
		{
			if (!bitmap->d3dShaderResourceView) return;
			float ext = -((float) pushTextureInSlot(bitmap->d3dShaderResourceView) + 0.4f);
			if (indexationEnabled)
			{
				if (vertexCount + 4 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillIndexedVerticesPosByRect(currentVertexBuffer, *destRect);
				_private::fillIndexedVerticesTexByRect<swapSrcDim>(currentVertexBuffer, srcRect);
				for (uint32 i = 0; i < 4; i++)
				{
					currentVertexBuffer[i].opacity = opacity;
					currentVertexBuffer[i].ext = ext;
				}
				vertexCount += 4;
			}
			else
			{
				if (vertexCount + 6 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillVerticesPosByRect(currentVertexBuffer, *destRect);
				_private::fillVerticesTexByRect<swapSrcDim>(currentVertexBuffer, srcRect);
				for (uint32 i = 0; i < 6; i++)
				{
					currentVertexBuffer[i].opacity = opacity;
					currentVertexBuffer[i].ext = ext;
				}
				vertexCount += 6;
			}
		}
		template <bool swapSrcDim = false>
		static inline void DrawBitmap(IShaderResource* bitmap, rectf32* destRect, float startOpacity, float endOpacity,
			Direction opacityGradientDirection, rectf32* srcRect = nullptr)
		{
			if (!bitmap->d3dShaderResourceView) return;
			float ext = -((float) pushTextureInSlot(bitmap->d3dShaderResourceView) + 0.4f);
			if (indexationEnabled)
			{
				if (vertexCount + 4 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillIndexedVerticesPosByRect(currentVertexBuffer, *destRect);
				_private::fillIndexedVerticesTexByRect<swapSrcDim>(currentVertexBuffer, srcRect);
				_private::fillIndexedVerticesOpacity(currentVertexBuffer, startOpacity, endOpacity, opacityGradientDirection);
				for (uint32 i = 0; i < 4; i++)
					currentVertexBuffer[i].ext = ext;
				vertexCount += 4;
			}
			else
			{
				if (vertexCount + 6 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillVerticesPosByRect(currentVertexBuffer, *destRect);
				_private::fillVerticesTexByRect<swapSrcDim>(currentVertexBuffer, srcRect);
				_private::fillVerticesOpacity(currentVertexBuffer, startOpacity, endOpacity, opacityGradientDirection);
				for (uint32 i = 0; i < 6; i++)
					currentVertexBuffer[i].ext = ext;
				vertexCount += 6;
			}
		}
		template <bool swapSrcDim = false>
		static inline void DrawBitmap(IShaderResource* bitmap, float32x2 start, float32x2 end, float width, rectf32* srcRect, float opacity)
		{
			if (!bitmap->d3dShaderResourceView) return;
			float ext = -((float) pushTextureInSlot(bitmap->d3dShaderResourceView) + 0.4f);
			if (indexationEnabled)
			{
				if (vertexCount + 4 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillIndexedVerticesPosByLine(currentVertexBuffer, start, end, width);
				_private::fillIndexedVerticesTexByRect<swapSrcDim>(currentVertexBuffer, srcRect);
				for (uint32 i = 0; i < 4; i++)
				{
					currentVertexBuffer[i].opacity = opacity;
					currentVertexBuffer[i].ext = ext;
				}
				vertexCount += 4;
			}
			else
			{
				if (vertexCount + 6 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillVerticesPosByLine(currentVertexBuffer, start, end, width);
				_private::fillVerticesTexByRect<swapSrcDim>(currentVertexBuffer, srcRect);
				for (uint32 i = 0; i < 6; i++)
				{
					currentVertexBuffer[i].opacity = opacity;
					currentVertexBuffer[i].ext = ext;
				}
				vertexCount += 6;
			}
		}
		static inline void DrawFillRect(rectf32* destRect, coloru32 color)
		{
			float32x4_nc colorf;
			color.toFloat4Unorm((float*) &colorf);
			if (indexationEnabled)
			{
				if (vertexCount + 4 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillIndexedVerticesPosByRect(currentVertexBuffer, *destRect);
				for (uint32 i = 0; i < 4; i++)
					currentVertexBuffer[i].color = colorf;
				vertexCount += 4;
			}
			else
			{
				if (vertexCount + 6 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillVerticesPosByRect(currentVertexBuffer, *destRect);
				for (uint32 i = 0; i < 6; i++)
					currentVertexBuffer[i].color = colorf;
				vertexCount += 6;
			}
		}
		static inline void DrawLine(float32x2 start, float32x2 end, coloru32 color, float width = 1.0f)
		{
			float32x4_nc colorf;
			color.toFloat4Unorm((float*) &colorf);
			if (indexationEnabled)
			{
				if (vertexCount + 4 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillIndexedVerticesPosByLine(currentVertexBuffer, start, end, width);
				for (uint32 i = 0; i < 4; i++)
					currentVertexBuffer[i].color = colorf;
				vertexCount += 4;
			}
			else
			{
				if (vertexCount + 6 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillVerticesPosByLine(currentVertexBuffer, start, end, width);
				for (uint32 i = 0; i < 6; i++)
					currentVertexBuffer[i].color = colorf;
				vertexCount += 6;
			}
		}
		static inline void DrawTriangle(float32x2* vertices, coloru32 color)
		{
			if (indexationEnabled)
				return;

			float32x4_nc colorf;
			color.toFloat4Unorm((float*) &colorf);
			if (vertexCount + 3 > vertexLimit)
				Flush();
			VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
			for (uint32 i = 0; i < 3; i++)
			{
				currentVertexBuffer[i].pos = vertices[i];
				currentVertexBuffer[i].color = colorf;
			}
			vertexCount += 3;
		}
		static inline void DrawQuadrangle(float32x2* vertices, coloru32 color)
		{
			float32x4_nc colorf;
			color.toFloat4Unorm((float*) &colorf);
			if (indexationEnabled)
			{
				if (vertexCount + 4 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				for (uint32 i = 0; i < 4; i++)
				{
					currentVertexBuffer[i].pos = vertices[i];
					currentVertexBuffer[i].color = colorf;
				}
				vertexCount += 4;
			}
			else
			{
				if (vertexCount + 6 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				currentVertexBuffer[0].pos = vertices[0];
				currentVertexBuffer[1].pos = vertices[1];
				currentVertexBuffer[4].pos = vertices[2];
				currentVertexBuffer[2].pos = vertices[3];
				currentVertexBuffer[3].pos = currentVertexBuffer[1].pos;
				currentVertexBuffer[5].pos = currentVertexBuffer[2].pos;
				for (uint32 i = 0; i < 6; i++)
					currentVertexBuffer[i].color = colorf;
				vertexCount += 6;
			}
		}
		static inline void DrawEllipse(float32x2 center, float radius, coloru32 color, float width)
		{
			float innerEllipse = (1.0f - width / radius) / 2.0f;
			rectf32 destRect(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
			if (indexationEnabled)
			{
				if (vertexCount + 4 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillIndexedVerticesPosByRect(currentVertexBuffer, destRect);
				_private::fillIndexedVerticesTexByRect(currentVertexBuffer, nullptr);
				for (uint32 i = 0; i < 4; i++)
				{
					currentVertexBuffer[i].tex.x += (float)color.r;
					currentVertexBuffer[i].tex.y += (float)color.g;
					currentVertexBuffer[i].ext = -(4.0f + (float) color.b / 256.0f + (float) color.a);
					currentVertexBuffer[i].opacity = innerEllipse;
				}
				vertexCount += 4;
			}
			else
			{
				if (vertexCount + 6 > vertexLimit)
					Flush();
				VertexBasic *currentVertexBuffer = vertexBuffer + vertexCount;
				_private::fillVerticesPosByRect(currentVertexBuffer, destRect);
				_private::fillVerticesTexByRect(currentVertexBuffer, nullptr);
				for (uint32 i = 0; i < 6; i++)
				{
					currentVertexBuffer[i].tex.x += (float) color.r;
					currentVertexBuffer[i].tex.y += (float) color.g;
					currentVertexBuffer[i].ext = -(4.0f + (float) color.b / 256.0f + (float) color.a);
					currentVertexBuffer[i].opacity = innerEllipse;
				}
				vertexCount += 6;
			}
		}
		static inline void DrawEllipse(float32x2 center, float radius, coloru32 color)
		{
			DrawEllipse(center, radius, color, radius);
		}
#undef DrawText
		static inline void DrawText(float32x2 position, float size, char* text, uint32 length, TextFormat* textFormat)
		{
			unsigned char *_text = (unsigned char*) text;
			for (uint32 i = 0; i < length; i++)
			{
				int charIndex = (int)(unsigned char) text[i];
				float width = size * textFormat->width / textFormat->height;
				rectf32 rect(position.x, position.y, position.x + width, position.y + size);
				DrawBitmap(&textFormat->texture, &rect, &rectf32((float) charIndex / 256.0f, 0.0f, (float) (charIndex + 1) / 256.0f, 1.0f));
				position.x += width;
			}
		}
	};
}