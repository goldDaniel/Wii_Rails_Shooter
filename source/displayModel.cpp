#include "displayModel.hpp"

#include "graphics.hpp"

DisplayModel::DisplayModel()
{
    guMtxIdentity(transform);

    translate(0, 0, -5);

    displayList = Renderer::AllocatePyramidDisplayList();
}

DisplayModel::~DisplayModel()
{
    delete displayList;
}

void DisplayModel::translate(f32 x, f32 y, f32 z)
{
    guMtxTransApply(transform, transform, x, y, z);
}

void DisplayModel::draw(Mtx view)
{
    Mtx modelview;
    guMtxConcat(view, transform, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_CallDispList(displayList->data, displayList->sizeInBytes);
}