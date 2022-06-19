#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
#include "devicelighting.h"

struct keyboard {
    char* id;
    char* name;

    struct lighting lighting;
};

int keyboard_from_id(char* id, struct keyboard* keyboard);

void keyboard_draw(struct keyboard* keyboard);


void keyboard_set_key_light(struct keyboard* keyboard, int key, int red, int green, int blue);
struct rgb keyboard_get_key_light(struct keyboard* keyboard, int key);

#endif