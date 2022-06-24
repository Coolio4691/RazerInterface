#include "dbus_handler.h"
#include <stdio.h>

DBusHandlerCode send_message(DBusBusType bus, char* bus_name, char* path, char* iface, char* method, DBusMessage** message) {
    // create connection and error variable
    DBusConnection* con;
    DBusError err;

    // initialize error variable
    dbus_error_init(&err);

    // connect to the bus
    con = dbus_bus_get(bus, &err);
    
    // check if error or if session bus == null
    if(dbus_error_is_set(&err)) {
        // print error message 
        fprintf(stderr, "Connection Error (%s)\n", err.message); 

        // free error variable
        dbus_error_free(&err); 
    }

    // return exit code 1 if bus did not connect 
    if(!con) {
        return BUSCONNECT_ERROR;
    }

    // create message and send then wait till reply
    DBusMessage* msg = dbus_message_new_method_call(bus_name, path, iface, method);
    *message = dbus_connection_send_with_reply_and_block(con, msg, -1, &err);

    // free the error variable
    dbus_error_free(&err); 
    // free msg
    dbus_message_unref(msg);
    // flush connection
    dbus_connection_flush(con);

    return SUCCESS;
}


DBusHandlerCode send_message_no_reply(DBusBusType bus, char* bus_name, char* path, char* iface, char* method) {
    // create connection and error variable
    DBusConnection* con;
    DBusError err;

    // initialize error variable
    dbus_error_init(&err);

    // connect to the bus
    con = dbus_bus_get(bus, &err);
    
    // check if error or if session bus == null
    if(dbus_error_is_set(&err)) {
        // print error message 
        fprintf(stderr, "Connection Error (%s)\n", err.message); 

        // free error variable
        dbus_error_free(&err); 
    }

    // return exit code 1 if bus did not connect 
    if(!con) {
        return BUSCONNECT_ERROR;
    }

    // create message and send
    DBusMessage* msg;
    msg = dbus_message_new_method_call(bus_name, path, iface, method);

    // set message
    DBusMessage* message = dbus_connection_send_with_reply_and_block(con, msg, -1, &err);

    // unref message
    dbus_message_unref(msg);
    dbus_message_unref(message);
    // free the error variable
    dbus_error_free(&err); 
    // flush connection
    dbus_connection_flush(con);

    // return successcode
    return SUCCESS;
}
