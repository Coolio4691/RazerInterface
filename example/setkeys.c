#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>
#include "keyboard.h"
#include "mouse.h"
#include "devicemanager.h"
#include "dbus_handler.h"

typedef enum { false, true } bool;

int main() {
    struct keyboard keyboard;
    struct mouse mouse;

    char** deviceIDList;
    int deviceIDListLen;


    devicemanager_get_devices(&deviceIDList, &deviceIDListLen);
    if(deviceIDListLen <= 0) return 0;

    keyboard_from_id(deviceIDList[0], &keyboard);
    printf("\nkeyboard: \nid: %s\nname: %s\nrows: %d\ncols: %d\n", keyboard.id, keyboard.name, keyboard.lighting.matrix.rows, keyboard.lighting.matrix.cols);
    
    device_lighting_set_key(&keyboard.lighting, key_index_from_2D(&keyboard.lighting, 5, 3), 255, 255, 255);
    keyboard_draw(&keyboard);

    return 0;
}