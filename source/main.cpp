#include <gccore.h>

#include "graphics.hpp"
#include "input.hpp"
#include "displayModel.hpp"

DisplayModel* model;

int main(int argc, char **argv)
{
	Mtx view;

	Input::Initialize();
	Renderer::Initialize();
		
	guVector  cam = {0.0F, 0.0F, 1.0F};
	guVector   up = {0.0F, 1.0F, 0.0F};
	guVector look = {0.0F, 0.0F, 0.0F};
	guLookAt(view, &cam, &up, &look);

	Renderer::SetProjectionMatrix(45, (f32)640/(f32)480, 0.1f, 300.f);

	model = new DisplayModel();

	while(1) 
	{
		Input::ControllerState state = Input::PollController(0);

		Renderer::BeginFrame();

		model->draw(view);

		GX_DrawDone();

		Renderer::DrawBox((u32*)Renderer::GetCurrentFrameBuffer(), 
						   state.ir.x - 5, state.ir.y - 3, 
						   state.ir.x + 3, state.ir.y + 5, 
						   COLOR_RED);
	
		Renderer::EndFrame();
	}

	delete model;

	return 0;
}
