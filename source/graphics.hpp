#ifndef RENDERER_H
#define RENDERER_H

#include <stdio.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <string.h>
#include <malloc.h>

struct DisplayList;
class DisplayModel;

namespace Renderer
{
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

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
}
#endif






