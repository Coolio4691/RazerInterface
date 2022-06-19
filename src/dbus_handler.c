#include "dbus_handler.h"
#include <stdio.h>

DBusHandlerCode send_message(DBusBusType bus, char* bus_name, char* path, char* iface, char* method, DBusMessage** msg) {
    // create connection and error variable
    DBusError err;
    DBusConnection* con;

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

    // create pending call
    DBusPendingCall* pending;

    // create message and send then wait till reply
    DBusMessage* message = dbus_message_new_method_call(bus_name, path, iface, method);
    dbus_connection_send_with_reply(con, message, &pending, -1);

    // wait until reply
    dbus_pending_call_block(pending);

    // set msg to reply
    *msg = dbus_pending_call_steal_reply(pending);

    // free the error variable
    dbus_error_free(&err); 

    // flush connection
    dbus_connection_flush(con);

    return SUCCESS;
}


DBusHandlerCode send_message_no_reply(DBusBusType bus, char* bus_name, char* path, char* iface, char* method) {
    // create connection and error variable
    DBusError err;
    DBusConnection* con;

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
    DBusMessage* message;
    message = dbus_message_new_method_call(bus_name, path, iface, method);

    dbus_connection_send(con, message, NULL);
    dbus_message_unref(message);
    // free the error variable
    dbus_error_free(&err); 

    // flush connection
    dbus_connection_flush(con);

    return SUCCESS;
}