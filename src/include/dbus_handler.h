#ifndef __RAZER_DBUS_HANDLER_H__
#define __RAZER_DBUS_HANDLER_H__
#include <dbus/dbus.h>

typedef enum DBusHandlerCode { SUCCESS, BUSCONNECT_ERROR } DBusHandlerCode;

DBusHandlerCode send_message(DBusBusType bus, char* bus_name, char* path, char* iface, char* method, DBusMessage** message);
DBusHandlerCode send_message_no_reply(DBusBusType bus, char* bus_name, char* path, char* iface, char* method);

#endif