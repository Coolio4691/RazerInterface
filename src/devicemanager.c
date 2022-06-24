#include "devicemanager.h"

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

    // create message, pending variable for response
    DBusMessage* message;

    // send message to dbus and get response to msg
    DBusHandlerCode code = send_message(DBUS_BUS_SESSION, "org.razer", "/org/razer", "razer.devices", "getDevices", &message);

    if(code != SUCCESS) {
        // print dbus error then exit
        printf("DBus Error: %d\n", code);

        exit(code);
    }

    // create iter variables
    DBusMessageIter iter, string;
    
    // initialize iter value
    dbus_message_iter_init(message, &iter); 

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

        // add the returned strings length to the output total length + null terminator
        size_t strSize = strlen(str);
        outputLen += strSize;

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
        *deviceIDs = (char**)malloc(outputLen + 1);

        // if elements > 0 copy old output to output
        if(*deviceIDsLen > 0) {
            for(int i = 0; i < *deviceIDsLen; i++) {
                (*deviceIDs)[i] = oldOutput[i];
            }

            // free old output memory 
            free(oldOutput);
        }
        
        // set output[outputlength + 1] to the returned str
        (*deviceIDs)[*deviceIDsLen] = str;
        (*deviceIDs)[(*deviceIDsLen)++][strSize] = 0;
    }
    while (dbus_message_iter_next(&string));

    // unref return value
    dbus_message_unref(message);
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
            free(devices);
            return 1;
        }
    }

    // if the device is not found free devices and return false 
    free(devices);
    return 0;
}

int devicemanager_get_device_property(char* device, char* interface, char* property, DBusMessage** message) {
    // if the device is not found return none
    if(!devicemanager_has_device(device)) {
        // return -1 if no device found
        return -1;
    }

    // create char* for path and devpath
    size_t pathStrLen = strlen(device) + 19;
    char* pathStr = malloc(pathStrLen);
    
    // copy devpathstr contents to pathstr
    strcpy(pathStr, "/org/razer/device/");
    // append device id to pathstr
    strcat(pathStr, device);
    // add null terminator
    pathStr[pathStrLen - 1] = 0;

    // send message to dbus sessionbus getting device type
    send_message(DBUS_BUS_SESSION, "org.razer", pathStr, interface, property, message);
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

    // create msg, iter for getting argument
    DBusMessage* message;
    DBusMessageIter iter;

    int code;

    if(code = devicemanager_get_device_property(device, "razer.device.misc", "getDeviceType", &message) <= -1) {
        printf("GetDeviceType Error: %d", code);

        return NONE;
    }

    dbus_message_iter_init(message, &iter);

    // create char* for iter return value
    char* type;
    // get string from message arg
    dbus_message_iter_get_basic(&iter, &type);

    // unref message
    dbus_message_unref(message);

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
    DBusMessage* message;

    // create iter for getting message arguments
    DBusMessageIter iter;
    int code;

    // if error return nothing
    if(code = devicemanager_get_device_property(device, "razer.device.misc", "getDeviceName", &message) <= -1) {
        printf("GetDeviceName Error: %d", code);

        return NULL;
    }

    // create char* for iter return value
    char* name;

    // get args from message
    dbus_message_iter_init(message, &iter);

    // get string from message arg
    dbus_message_iter_get_basic(&iter, &name);

    // free return value
    dbus_message_unref(message);

    // return name string
    return name;
}

struct VIDPID devicemanager_get_device_vidpid(char* device) {
    struct VIDPID VIDPID;

    // if the device is not found return none
    if(!devicemanager_has_device(device)) {
        return VIDPID;
    }

    // create message, iter for getting arguments
    DBusMessage* message;
    DBusMessageIter iter, values;

    int code;

    // if error return nothing
    if(code = devicemanager_get_device_property(device, "razer.device.misc", "getVidPid", &message) <= -1) {
        printf("GetDeviceVIDPID Error: %d", code);

        return VIDPID;
    }

    // get args from msg
    dbus_message_iter_init(message, &iter);
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

    // unref pending
    dbus_message_unref(message);

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

    // create message variable
    DBusMessage* message;

    // create iter for getting arguments
    DBusMessageIter iter, values;
    int code;

    // if error return nothing
    if(code = devicemanager_get_device_property(device, "razer.device.misc", "getMatrixDimensions", &message) <= -1) {
        printf("GetDeviceMatrix Error: %d", code);

        return -1;
    }

    // initialize iter value
    dbus_message_iter_init(message, &iter); 
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

    // free return value
    dbus_message_unref(message);

    // return name string
    return SUCCESS;
}