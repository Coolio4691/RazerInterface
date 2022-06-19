#ifndef __MOUSE_H__
#define __MOUSE_H__
#include "devicelighting.h"
#include "devicemanager.h"

struct mouse {
    char* id;
    char* name;

    struct lighting lighting;
};

int mouse_from_id(char* id, struct mouse* mouse);

void mouse_draw(struct mouse* mouse);


void mouse_set_light(struct mouse* mouse, int light, int red, int green, int blue);
struct rgb mouse_get_light(struct mouse* mouse, int light);

#endif