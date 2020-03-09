#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include "graphics.hpp"

#define DEFAULT_FIFO_SIZE	(256*1024)

ir_t ir;


Renderer::DisplayModel* model;


void initialize()
{
	Renderer::Initialize();
	WPAD_Init();

	WPAD_SetVRes(0, 640, 480);
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR); 
	WPAD_IR(0, &ir);
}

int main(int argc, char **argv)
{
	Mtx view;
	Mtx44 perspective;

	float rtri = 0.0f , rquad = 0.0f;

	Renderer::Initialize();
	initialize();
	

	// setup our camera at the origin
	// looking down the -z axis with y up
	guVector cam = {0.0F, 0.0F, 0.0F},
			up = {0.0F, 1.0F, 0.0F},
		  look = {0.0F, 0.0F, -1.0F};
	guLookAt(view, &cam, &up, &look);


	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 90,
	// and aspect ratio based on the display resolution
	Renderer::SetProjectionMatrix(45, (f32)640/(f32)480, 0.1f, 300.f);

	guVector triAxis = {0,1,0};

	model = new Renderer::DisplayModel();

	while(1) 
	{
		
		WPAD_ScanPads();
		WPAD_IR(0, &ir);
		
		//bits represent pressed buttons for controller index 0
		u16 buttonsDown = WPAD_ButtonsDown(0);
		u16 buttonsHeld = WPAD_ButtonsHeld(0);
		u16 buttonsUp = WPAD_ButtonsUp(0);

		if (buttonsDown & WPAD_BUTTON_HOME) exit(0);

		Renderer::BeginFrame();
		


		model->draw(view);
		GX_DrawDone();

		Renderer::DrawBox((u32*)Renderer::GetCurrentFrameBuffer(), 
						  ir.x - 5, ir.y - 3, 
						  ir.x + 3, ir.y + 5, 
						  COLOR_RED);
	
		Renderer::EndFrame();

		

		rtri+=0.2f;				// Increase The Rotation Variable For The Triangle ( NEW )
		rquad-=0.15f;			// Decrease The Rotation Variable For The Quad     ( NEW )

	}
	return 0;
}
