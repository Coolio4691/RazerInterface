#include "keyboard.h"
#include "dbus_handler.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int keyboard_from_id(char* id, struct keyboard* keyboard) {
    // get around weird error with id
    size_t idLen = strlen(id);
    
    keyboard->id = (char*)malloc((idLen + 1) * sizeof(char));
    memcpy(keyboard->id, id, idLen);
    keyboard->id[idLen] = 0;
    
    // set name and vidpid
    keyboard->name = devicemanager_get_device_name(keyboard->id);
    keyboard->VIDPID = devicemanager_get_device_vidpid(keyboard->id);
    
    // init device lighting
    device_lighting_init(keyboard->id, &keyboard->lighting);

    return 0;
}

void keyboard_free(struct keyboard* keyboard) {
    device_lighting_free(&keyboard->lighting);
}

void keyboard_draw(struct keyboard* keyboard) {
    // if the device is not found return
    if(!devicemanager_has_device(keyboard->id)) {
        return;
    }

    unsigned char* bytes;

    // get lighting array bytes 
    size_t bytesSize = lighting_colour_bytes(&keyboard->lighting, &bytes);

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
    size_t pathStrSize = strlen(keyboard->id) + 19;

    // create char* for path and devpath + null terminator
    char* pathStr = (char*)malloc(pathStrSize * sizeof(char));
    
    // copy devpathstr contents to pathstr
    strcpy(pathStr, "/org/razer/device/");
    // append device id to pathstr
    strcat(pathStr, keyboard->id);
    pathStr[pathStrSize - 1] = 0;

    // create message and send then wait till reply
    DBusMessage* msg = dbus_message_new_method_call("org.razer", pathStr, "razer.device.lighting.chroma", "setKeyRow");
    // free pathstr variable
    free(pathStr);
    
    // add byte array to arguments
    dbus_message_append_args(msg, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE, &bytes, bytesSize, DBUS_TYPE_INVALID);
    // free bytes
    free(bytes);
    
    // send message
    DBusMessage* message = dbus_connection_send_with_reply_and_block(con, msg, -1, &err);
    
    // unref message
    dbus_message_unref(message);
    dbus_message_unref(msg);
    
    // free the error variable
    dbus_error_free(&err); 
    // flush connection
    dbus_connection_flush(con);
    dbus_connection_unref(con);


    // call setcustom method
    devicemanager_call_device_method_no_args(keyboard->id, "razer.device.lighting.chroma", "setCustom"); // set custom effect again to load new buffer
    // reset lighting array to all unset
    device_lighting_reset(&keyboard->lighting);
}

void keyboard_set_key_light(struct keyboard* keyboard, int key, int red, int green, int blue) {
    // set rgb at key
    device_lighting_set_led(&keyboard->lighting, key, red, green, blue);
}

struct rgb keyboard_get_key_light(struct keyboard* keyboard, int key) {
    // get rgb at key
    return device_lighting_get_led(&keyboard->lighting, key);
}