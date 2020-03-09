#ifndef DISPLAY_LIST_H
#define DISPLAY_LIST_H

#include <malloc.h>

struct DisplayList
{
    void* data;
    u32 sizeInBytes;

    ~DisplayList()
    {
        free(data);
    }
};

#endif