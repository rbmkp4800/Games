#include <stdlib.h>

#include "types.h"

#include "Render.h"
#include "Game2.h"

float32x3 cameraSphericPosition(0.0f, 0.0f, 40.0f);

unsigned long __stdcall GameThreadMain(void *hWnd)
{
	uint16x2 outputSize(OUTPUT_SIZE_X, OUTPUT_SIZE_Y);

	Render::Init();
	RenderTarget renderTarget(hWnd);
	Render::SetRenderTarget(&renderTarget);

	/*VertexPosTex vertexBuffer [] =
	{
		{ { 10, 10, 10 }, { 1.0f, 1.0f } },
		{ { -10, 10, 10 }, { 0.0f, 1.0f } },
		{ { 10, -10, 10 }, { 1.0f, 0.0f } },
		{ { -10, -10, 10 }, { 0.0f, 0.0f } },
		{ { 10, 10, -10 }, { 1.0f, 0.0f } },
		{ { -10, 10, -10 }, { 0.0f, 0.0f } },
		{ { 10, -10, -10 }, { 0.0f, 1.0f } },
		{ { -10, -10, -10 }, { 1.0f, 1.0f } },
	};

	uint16x3 indexBuffer [] =
	{
		{ 1, 2, 0 },
		{ 2, 1, 3 },
		{ 6, 5, 4 },
		{ 5, 6, 7 },

		{ 0, 6, 4 },
		{ 6, 0, 2 },
		{ 7, 1, 5 },
		{ 1, 7, 3 },

		{ 1, 4, 5 },
		{ 4, 1, 0 },
		{ 3, 6, 2 },
		{ 6, 3, 7 },
	};*/

	VertexPosNorm vertexBuffer[256];
	for (int32 i = 0; i < 16; i++)
	{
		for (int32 j = 0; j < 16; j++)
		{
			vertexBuffer[i * 16 + j].pos.set(i - 8.0f, (rand() % 200) / 100.0f, j - 8.0f);
		}
	}
	uint16x3 indexBuffer[15 * 15 * 2];
	for (uint32 i = 0; i < 15; i++)
	{
		for (uint32 j = 0; j < 15; j++)
		{
			indexBuffer[(i * 15 + j) * 2].set((i + 1) * 16 + j, i * 16 + j, i * 16 + j + 1);
			indexBuffer[(i * 15 + j) * 2 + 1].set((i + 1) * 16 + j + 1, (i + 1) * 16 + j, i * 16 + j + 1);
		}
	}

	CalucateVertexBufferNormals(vertexBuffer, indexBuffer, 256, 15 * 15 * 2);
	Mesh mesh(vertexBuffer, indexBuffer, elemcntof(vertexBuffer), elemcntof(indexBuffer), EffectType::_PosNorm);
	Matrix4x4 projection = Matrix4x4::Perspective(3.141592654f / 3.0f, outputSize.aspect(), 2.0f, 200.0f);

	for (;;)
	{
		float32x3 cameraPosition(
			cameraSphericPosition.z * sinf(cameraSphericPosition.x) * cosf(cameraSphericPosition.y),
			cameraSphericPosition.z * sinf(cameraSphericPosition.y),
			cameraSphericPosition.z * cosf(cameraSphericPosition.x) * cosf(cameraSphericPosition.y));

		//float32x3 cameraPosition(25.0f, -25.0f, 25.0f);

		Matrix4x4 resultTransform = projection * Matrix4x4::LookAt(cameraPosition, float32x3(0.0f, 0.0f, 0.0f), float32x3(0.0f, 10.0f, 0.0f))/* * Matrix4x4::RotateX(cameraSphericPosition.x) * Matrix4x4::RotateZ(cameraSphericPosition.y)*/;
		Render::SetTransform(&resultTransform);

		renderTarget.Clear();
		Render::Draw(&mesh);
		renderTarget.Present();
	}

	return 0;
}