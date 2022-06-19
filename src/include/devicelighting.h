#ifndef __DEVICE_LIGHTING_H__
#define __DEVICE_LIGHTING_H__
#include "devicematrix.h"
#include "dbus_handler.h"

struct rgb {
    int red;
    int green;
    int blue;
};

struct lighting {
    struct matrix matrix;

    struct rgb* colourArray;
};

int device_lighting_init(char* device, struct lighting* lighting);

void device_lighting_reset(struct lighting* lighting);

void device_lighting_set_key(struct lighting* lighting, int key, int r, int g, int b);
struct rgb device_lighting_get_key(struct lighting* lighting, int key);


int key_index_from_2D(struct lighting* lighting, int x, int y);
int* key_2D_from_index(struct lighting* lighting, int index);


void lighting_row_bytes(struct lighting* lighting, int row, unsigned char** bytes, int* bytesSize);
void lighting_colour_bytes(struct lighting* lighting, unsigned char** bytes, int* bytesSize);

#endif