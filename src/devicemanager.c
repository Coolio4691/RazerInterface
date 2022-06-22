#include "devicemanager.h"
#include "dbus_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void devicemanager_get_devices(char*** deviceIDs, int *deviceIDsLen) {
    // create buffer for previous output
    char** oldOutput; 

    // set length to 0(no elements)
    *deviceIDsLen = 0; 

    // set total length of arrays to 0
    int outputLen = 0;
    int oldOutputLen = 0;

    // create message variable for response
    DBusMessage* msg; 

    // send message to dbus and get response to msg
    DBusHandlerCode code = send_message(DBUS_BUS_SESSION, "org.razer", "/org/razer", "razer.devices", "getDevices", &msg);

    if(code != SUCCESS) {
        // print dbus error then exit
        printf("DBus Error: %d\n", code);

        exit(code);
    }

    // create iter variables
    DBusMessageIter iter, string;
    
    // initialize iter value
    dbus_message_iter_init(msg, &iter); 

    // create str buffer
    char* str;

    // get array from openrazer in iter
    dbus_message_iter_recurse(&iter, &string); 

    // loop through array until no more elements
    do {
        // set oldlen to current len
        oldOutputLen = outputLen; 

        // get str from array
        dbus_message_iter_get_basic(&string, &str);

        // add the returned strings length to the output total length
        outputLen += strlen(str);

        // if elements > 0 create old output buffer
        if(*deviceIDsLen > 0) {
            // allocate memory to oldoutput
            oldOutput = malloc(oldOutputLen); 
            
            // copy output to old output
            for(int i = 0; i < *deviceIDsLen; i++) {
                oldOutput[i] = (*deviceIDs)[i];
            }
            
            // free current outout for new allocation
            free(*deviceIDs);
        }
        
        // allocate memory to output
        *deviceIDs = malloc(outputLen + 1);

        // if elements > 0 copy old output to output
        if(*deviceIDsLen > 0) {
            for(int i = 0; i < *deviceIDsLen; i++) {
                (*deviceIDs)[i] = oldOutput[i];
            }

            // free old output memory 
            free(oldOutput);
        }
        
        // set output[outputlength + 1] to the returned str
        (*deviceIDs)[(*deviceIDsLen)++] = str;
    }
    while (dbus_message_iter_next(&string));

    // unref msg
    dbus_message_unref(msg);
}

int devicemanager_has_device(char* device) {
    // create variable for get devices return
    char** devices;
    int devicesLen;

    // get all devices
    devicemanager_get_devices(&devices, &devicesLen);

    // loop through devices and compare if it is device
    for(int i = 0; i < devicesLen; i++) {
        if(strcmp(devices[i], device) == 0) {
            // free devices then return true
            free(devices);
            return 1;
        }
    }

    // if the device is not found free devices and return false 
    free(devices);
    return 0;
}

int devicemanager_get_device_property(char* device, char* interface, char* property, DBusMessage** msg) {
    // if the device is not found return none
    if(!devicemanager_has_device(device)) {
        // return -1 if no device found
        return -1;
    }

    // create char* for path and devpath
    char* devPathStr = "/org/razer/device/";
    char* pathStr = malloc(strlen(devPathStr) + strlen(device) + 1);
    
    // copy devpathstr contents to pathstr
    strcpy(pathStr, devPathStr);
    // append device id to pathstr
    strcat(pathStr, device);

    // send message to dbus sessionbus getting device type
    send_message(DBUS_BUS_SESSION, "org.razer", pathStr, interface, property, msg);
    // free pathstr
    free(pathStr);

    // return success
    return SUCCESS;
}

int devicemanager_call_device_method_no_args(char* device, char* interface, char* property) {
    // if the device is not found return none
    if(!devicemanager_has_device(device)) {
        // return -1 if no device found
        return -1;
    }

    // create char* for path and devpath
    char* devPathStr = "/org/razer/device/";
    char* pathStr = malloc(strlen(devPathStr) + strlen(device) + 1);
    
    // copy devpathstr contents to pathstr
    strcpy(pathStr, devPathStr);
    // append device id to pathstr
    strcat(pathStr, device);

    // send message to dbus sessionbus getting device type
    send_message_no_reply(DBUS_BUS_SESSION, "org.razer", pathStr, interface, property);
    // free pathstr
    free(pathStr);

    // return success
    return SUCCESS;
}

DeviceType devicemanager_get_device_type(char* device) {
    // if the device is not found return none
    if(!devicemanager_has_device(device)) {
        return NONE;
    }

    // create iter for getting argument
    DBusMessage* msg;
    DBusMessageIter iter;
    int code;

    if(code = devicemanager_get_device_property(device, "razer.device.misc", "getDeviceType", &msg) <= -1) {
        printf("GetDeviceType Error: %d", code);

        return NONE;
    }

    dbus_message_iter_init(msg, &iter);
    dbus_message_unref(msg);

    // create char* for iter return value
    char* type;
    // get string from message arg
    dbus_message_iter_get_basic(&iter, &type);

    if(strcmp(type, "keyboard") == 0) {
        // return keyboard if type == keyboard
        return KEYBOARD;
    }
    else if(strcmp(type, "mouse") == 0) {
        // return mouse if type == mouse
        return MOUSE;
    } 

    // return value
    return NONE;
}
char* devicemanager_get_device_name(char* device) {
    // if the device is not found return none
    if(!devicemanager_has_device(device)) {
        return NULL;
    }

    // create message variable
    DBusMessage* msg;

    // create iter for getting message arguments
    DBusMessageIter iter;
    int code;

    // if error return nothing
    if(code = devicemanager_get_device_property(device, "razer.device.misc", "getDeviceName", &msg) <= -1) {
        printf("GetDeviceName Error: %d", code);

        return NULL;
    }

    // create char* for iter return value
    char* name;

    // get args from message
    dbus_message_iter_init(msg, &iter);

    // get string from message arg
    dbus_message_iter_get_basic(&iter, &name);
    // unref msg
    dbus_message_unref(msg);

    // return name string
    return name;
}

struct VIDPID devicemanager_get_device_vidpid(char* device) {
    struct VIDPID VIDPID;

    // if the device is not found return none
    if(!devicemanager_has_device(device)) {
        return VIDPID;
    }

    // create iter for getting argument
    DBusMessage* msg;
    DBusMessageIter iter, values;
    int code;

    // if error return nothing
    if(code = devicemanager_get_device_property(device, "razer.device.misc", "getVidPid", &msg) <= -1) {
        printf("GetDeviceVIDPID Error: %d", code);

        return VIDPID;
    }

    // get args from msg
    dbus_message_iter_init(msg, &iter);
    // get int[2] from first arg
    dbus_message_iter_recurse(&iter, &values);

    int vid;
    int pid;

    // get int from array
    dbus_message_iter_get_basic(&values, &vid);
    // get next value
    dbus_message_iter_next(&values);
    // get int from array
    dbus_message_iter_get_basic(&values, &pid);

    // unref msg
    dbus_message_unref(msg);

    // set vidint to vid
    VIDPID.vidInt = vid;
    // set pidint to pid
    VIDPID.pidInt = pid;

    // format int to hex string
    sprintf(VIDPID.vid, "%04X", vid);
    // format int to hex string
    sprintf(VIDPID.pid, "%04X", pid);
    
    // return vidpid struct with values
    return VIDPID;
}

int devicemanager_get_device_matrix(char* device, struct matrix* matrix) {    
    // if the device is not found return none
    if(!devicemanager_has_device(device)) {
        return -1;
    }

    // create msg variable
    DBusMessage* msg;

    // create iter for getting arguments
    DBusMessageIter iter, values;
    int code;

    // if error return nothing
    if(code = devicemanager_get_device_property(device, "razer.device.misc", "getMatrixDimensions", &msg) <= -1) {
        printf("GetDeviceMatrix Error: %d", code);

        return -1;
    }

    // initialize iter value
    dbus_message_iter_init(msg, &iter); 
    // get int[2] from first arg
    dbus_message_iter_recurse(&iter, &values);
    
    // value buffer
    int val;

    // get rows from array
    dbus_message_iter_get_basic(&values, &val);
    // set matrix rows to output
    matrix->rows = val;

    // get next value
    dbus_message_iter_next(&values);
    // get cols from array
    dbus_message_iter_get_basic(&values, &val);
    // set matrix cols to val
    matrix->cols = val;

    // return name string
    return SUCCESS;
}