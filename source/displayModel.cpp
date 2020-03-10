#include "displayModel.hpp"

#include "graphics.hpp"

DisplayModel::DisplayModel()
{
    position.z = -5.0f;

    displayList = Renderer::AllocatePyramidDisplayList();
}

DisplayModel::~DisplayModel()
{
    delete displayList;
}

void DisplayModel::translate(f32 x, f32 y, f32 z)
{
    position.x += x;
    position.y += y;
    position.z += z;
}

void DisplayModel::draw(Mtx view)
{
    Mtx model;
    guMtxIdentity(model);
    guMtxTransApply(model, model, position.x, position.y, position.z);

    Mtx modelview;

    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_CallDispList(displayList->data, displayList->sizeInBytes);
}