#ifndef INPUT_H
#define INPUT_H

namespace Input
{
    struct ControllerState
    {
        ir_t ir;

        u16 buttonsDown = 0;
		u16 buttonsHeld = 0;
		u16 buttonsUp = 0;
    };


    

    void Initialize()
    {
        WPAD_Init();

        WPAD_SetVRes(0, 640, 480);
        WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR); 
    }

    ControllerState PollController(int controllerIndex)
    {
        ControllerState result;

        WPAD_ScanPads();

        WPAD_IR(0, &result.ir);

        result.buttonsDown = WPAD_ButtonsDown(0);
		result.buttonsHeld = WPAD_ButtonsHeld(0);
		result.buttonsUp = WPAD_ButtonsUp(0);

        return result;
    }
}
#endif