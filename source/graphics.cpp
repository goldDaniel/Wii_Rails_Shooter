#include "graphics.hpp"
#include "displayModel.hpp"

#include <stdio.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <string.h>
#include <malloc.h>

#define DEFAULT_FIFO_SIZE	(256*1024)

static u32 fbIndex = 0;
static void *frameBuffer[2] = { NULL, NULL};

static GXRModeObj *rmode;


void Renderer::Initialize()
{
    VIDEO_Init();

    rmode = VIDEO_GetPreferredMode(NULL);

	// allocate 2 framebuffers for double buffering
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[0]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();

	if(rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    f32 yscale;
	u32 xfbHeight;

    // setup the fifo and then init the flipper
	void *gp_fifo = NULL;
	gp_fifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gp_fifo,0,DEFAULT_FIFO_SIZE);
	GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);
	// clears the bg to color and clears the z buffer
	GX_SetCopyClear({0, 0, 0, 0xff}, 0x00ffffff);

	// other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer[0],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
}

void* Renderer::GetCurrentFrameBuffer()
{
    return frameBuffer[fbIndex];
}

void Renderer::SwapFrameBuffer()
{
    fbIndex ^= 1;
}

void Renderer::SetProjectionMatrix(f32 fov, f32 ratio, f32 near, f32 far)
{
    Mtx44 perspective;
    guPerspective(perspective, fov, ratio, near, far);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);   
}

DisplayList* Renderer::AllocatePyramidDisplayList()
{
    // setup the vertex descriptor
    // tells the flipper to expect direct data
    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

    // setup the vertex attribute table
    // describes the data
    // args: vat location 0-7, type of data, data format, size, scale
    // so for ex. in the first call we are sending position data with
    // 3 values X,Y,Z of size F32. scale sets the number of fractional
    // bits for non float data.
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGB8, 0);

    GX_SetNumChans(1);
    GX_SetNumTexGens(0);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    DisplayList* result = new DisplayList();
    
    u32 dataSize = (12 * 12 + 12 * 5);
    result->data = memalign(32, dataSize);
    memset(result->data, 0, dataSize);

    DCInvalidateRange(result->data, dataSize);

    GX_BeginDispList(result->data, dataSize);
    {
        GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 12);		// Draw A Pyramid
        {
            GX_Position3f32( 0.0f, 1.0f, 0.0f);		// Top of Triangle (front)
            GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
            GX_Position3f32(-1.0f,-1.0f, 1.0f);	// Left of Triangle (front)
            GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green
            GX_Position3f32( 1.0f,-1.0f, 1.0f);	// Right of Triangle (front)
            GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue

            GX_Position3f32( 0.0f, 1.0f, 0.0f);		// Top of Triangle (Right)
            GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
            GX_Position3f32( 1.0f,-1.0f, 1.0f);	// Left of Triangle (Right)
            GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue
            GX_Position3f32( 1.0f,-1.0f,-1.0f);	// Right of Triangle (Right)
            GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green

            GX_Position3f32( 0.0f, 1.0f, 0.0f);		// Top of Triangle (Back)
            GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
            GX_Position3f32(-1.0f,-1.0f,-1.0f);	// Left of Triangle (Back)
            GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue
            GX_Position3f32( 1.0f,-1.0f,-1.0f);	// Right of Triangle (Back)
            GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green

            GX_Position3f32( 0.0f, 1.0f, 0.0f);		// Top of Triangle (Left)
            GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
            GX_Position3f32(-1.0f,-1.0f,-1.0f);	// Left of Triangle (Left)
            GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue
            GX_Position3f32(-1.0f,-1.0f, 1.0f);	// Right of Triangle (Left)
            GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green
        }
        GX_End();
    }
    result->sizeInBytes = GX_EndDispList();

    return result;
}

void Renderer::BeginFrame()
{
    // do this before drawing
    GX_SetViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1);
}

void Renderer::EndFrame()
{
    
    Renderer::SwapFrameBuffer();
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);
    GX_CopyDisp(Renderer::GetCurrentFrameBuffer(), GX_TRUE);

    VIDEO_SetNextFramebuffer(Renderer::GetCurrentFrameBuffer());

    VIDEO_Flush();

    VIDEO_WaitVSync();
}

void Renderer::DrawHLine(u32* frameBuffer, int x1, int x2, int y, int color) 
{
    int i;
    y = 320 * y;
    x1 >>= 1;
    x2 >>= 1;
    for (i = x1; i <= x2; i++) 
    {
        frameBuffer[y+i] = color;
    }
}
void Renderer::DrawVLine(u32* frameBuffer, int x, int y1, int y2, int color) 
{
    int i;
    x >>= 1;
    for (i = y1; i <= y2; i++) 
    {
        frameBuffer[x + (640 * i) / 2] = color;
    }
}
void Renderer::DrawBox(u32* frameBuffer, int x1, int y1, int x2, int y2, int color) 
{
    if(x1 < 0) x1 = 0;
    if(x2 < 0) x2 = 0;
    if(y1 < 0) y1 = 0;
    if(y1 < 0) y2 = 0;

    if(x1 > SCREEN_WIDTH) x1 = SCREEN_WIDTH;
    if(x2 > SCREEN_WIDTH) x2 = SCREEN_WIDTH;
    if(y1 > SCREEN_HEIGHT) y1 = SCREEN_HEIGHT;
    if(y2 > SCREEN_HEIGHT) y2 = SCREEN_HEIGHT;


    DrawHLine (frameBuffer, x1, x2, y1, color);
    DrawHLine (frameBuffer, x1, x2, y2, color);
    DrawVLine (frameBuffer, x1, y1, y2, color);
    DrawVLine (frameBuffer, x2, y1, y2, color);
}