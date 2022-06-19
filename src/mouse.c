#include "mouse.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int mouse_from_id(char* id, struct mouse* mouse) {
    (*mouse).id = id;
    (*mouse).name = devicemanager_get_device_name(id);

    device_lighting_init(id, &mouse->lighting);

    return 0;
}


void mouse_draw(struct mouse* mouse) {
    unsigned char* bytes;
    size_t bytesSize;

    // get bytes 
    lighting_colour_bytes(&mouse->lighting, &bytes, &bytesSize);

    // create connection and error variable
    DBusError err;
    DBusConnection* con;

    // initialize error variable
    dbus_error_init(&err);

    // connect to the bus
    con = dbus_bus_get(DBUS_BUS_SESSION, &err);
    
    // check if error or if session bus == null
    if(dbus_error_is_set(&err)) {
        // print error message 
        fprintf(stderr, "Connection Error (%s)\n", err.message); 

        // free error variable
        dbus_error_free(&err); 
    }

    // exit if bus did not connect 
    if(!con) {
        return;
    }

    // create pending call
    DBusPendingCall* pending;
    dbus_bool_t dbret;

    // create char* for path and devpath
    char* devPathStr = "/org/razer/device/";
    char* pathStr = malloc(strlen(devPathStr) + strlen(mouse->id) + 1);
    
    // copy devpathstr contents to pathstr
    strcpy(pathStr, devPathStr);
    // append device id to pathstr
    strcat(pathStr, mouse->id);


    // create message and send then wait till reply
    DBusMessage* message = dbus_message_new_method_call("org.razer", pathStr, "razer.device.lighting.chroma", "setKeyRow");
    
    // add byte array to arguments
    dbus_message_append_args(message, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE, &bytes, bytesSize, DBUS_TYPE_INVALID);
    
    // send message then wait
    dbus_connection_send_with_reply(con, message, &pending, -1);
    // wait until reply
    dbus_pending_call_block(pending);

    // set msg to reply
    DBusMessage* reply = dbus_pending_call_steal_reply(pending);

    // free the error variable
    dbus_error_free(&err); 

    // flush connection
    dbus_connection_flush(con);

    DBusMessage* msg; // add just to fulfill the function requirements
    devicemanager_get_device_property(mouse->id, "razer.device.lighting.chroma", "setCustom", &msg); // set custom effect again to load new buffer

    free(bytes);
    device_lighting_reset(&mouse->lighting);
}

void mouse_set_light(struct mouse* mouse, int light, int red, int green, int blue) {
    // set rgb at key
    device_lighting_set_key(&mouse->lighting, light, red, green, blue);
}

struct rgb mouse_get_light(struct mouse* mouse, int light) {
    // get rgb at key
    return device_lighting_get_key(&mouse->lighting, light);
}