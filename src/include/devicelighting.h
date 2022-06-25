#ifndef __RAZER_DEVICE_LIGHTING_H__
#define __RAZER_DEVICE_LIGHTING_H__
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
void device_lighting_free(struct lighting* lighting);

void device_lighting_reset(struct lighting* lighting);

void device_lighting_set_led(struct lighting* lighting, int key, int r, int g, int b);
struct rgb device_lighting_get_led(struct lighting* lighting, int key);


int key_index_from_2D(struct lighting* lighting, int col, int row);
int* key_2D_from_index(struct lighting* lighting, int index);


size_t lighting_row_bytes(struct lighting* lighting, int row, unsigned char** bytes);
size_t lighting_colour_bytes(struct lighting* lighting, unsigned char** bytes);

#endif