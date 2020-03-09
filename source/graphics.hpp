#ifndef RENDERER_H
#define RENDERER_H

#include <stdio.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <string.h>
#include <malloc.h>

namespace Renderer
{
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	struct DisplayList
	{
		void* data;
		u32 sizeInBytes;

		~DisplayList()
		{
			free(data);
		}
	};

	void Initialize();

	void* GetCurrentFrameBuffer();
	void SwapFrameBuffer();
	void SetProjectionMatrix(f32 fov, f32 ratio, f32 near, f32 far);

	void BeginFrame();
	void EndFrame();

	void DrawHLine(u32* frameBuffer, int x1, int x2, int y, int color);
	void DrawVLine(u32* frameBuffer, int x, int y1, int y2, int color); 
	void DrawBox(u32* frameBuffer, int x1, int y1, int x2, int y2, int color);

	DisplayList* AllocatePyramidDisplayList();

	class DisplayModel
	{
	private:
		Mtx transform;
		DisplayList* displayList;

	public:
		DisplayModel()
		{
			guMtxIdentity(transform);

			translate(0, 0, -5);

			displayList = AllocatePyramidDisplayList();
		}

		~DisplayModel()
		{
			delete displayList;
		}

		void translate(f32 x, f32 y, f32 z)
		{
			guMtxTransApply(transform, transform, x, y, z);
		}

		void draw(Mtx view)
		{
			Mtx modelview;
			guMtxConcat(view, transform, modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);

			GX_CallDispList(displayList->data, displayList->sizeInBytes);
		}
	};

	
}

#endif






