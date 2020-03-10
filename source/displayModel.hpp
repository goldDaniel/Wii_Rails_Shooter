#ifndef DISPLAY_MODEL_H
#define DISPLAY_MODEL_H

#include <gccore.h>
#include "displayList.hpp"

class DisplayModel
{
private:
    guVector position;
    
    DisplayList* displayList;

public:
    DisplayModel();
    ~DisplayModel();


    
    void translate(f32 x, f32 y, f32 z);
    void draw(Mtx view);
};

#endif