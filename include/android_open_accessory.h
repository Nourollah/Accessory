// **************************************************************************************************************** //
/**
 * Android Open usb_accessory connection implementation for libusb-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <syslog.h>

#include "android_transporter.h"

#ifndef ANDROID_OPEN_ACCESSORY_H
#define ANDROID_OPEN_ACCESSORY_H
#define GCC

#define USB_ACCESSORY_VENDOR_ID             0x18D1
//AOA1.0 Specific
#define USB_ACCESSORY_PRODUCT_ID            0x2D00
#define USB_ACCESSORY_ADB_PRODUCT_ID        0x2D01

//AOA2.0 Specific
#define USB_AUDIO_PRODUCT_ID                0x2D02
#define USB_AUDIO_ADB_PRODUCT_ID            0x2D03
#define USB_ACCESSORY_AUDIO_PRODUCT_ID      0x2D04
#define USB_ACCESSORY_AUDIO_ADB_PRODUCT_ID  0x2D05

#define ACCESSORY_PID                       0x2D01
#define ACCESSORY_PID_ALT                   0x2D00

#define ACCESSORY_STRING_MANUFACTURER       0
#define ACCESSORY_STRING_MODEL              1
#define ACCESSORY_STRING_DESCRIPTION        2
#define ACCESSORY_STRING_VERSION            3
#define ACCESSORY_STRING_URI                4
#define ACCESSORY_STRING_SERIAL             5

#define WEIGH_REPORT_SIZE                   7


#define ACCESSORY_GET_PROTOCOL              51
#define ACCESSORY_SEND_STRING               52
#define ACCESSORY_START                     53

// for async transfer.
#define EP_COMMAND    (2 | LIBUSB_ENDPOINT_OUT) // OUT of PC to USB-device //DEVICE_SPECIFIC
#define EP_RESPONSE   (4 | LIBUSB_ENDPOINT_IN ) // IN  PC from  USB-device //DEVICE_SPECIFIC

#define IN 0x85
#define OUT 0x07

// **************************************************************************************************************** //
/**
 * This class presentation function to get USB usb_accessory mode on Android, read and write stream over USB
 */

const char *manufacturer;
const char *model;
const char *description;
const char *version;
const char *uri;
const char *serial;

libusb_transfer_cb_fn cbTransfer;



int search_device(libusb_context *ctx, uint16_t *idVendor, uint16_t *idProduct);
int find_end_Point(libusb_device *dev);
int get_protocol();
void android_open_accessory(const char *manufacturer, const char *model, const char *description, const char *version, const char *uri, const char *serial);
void android_close_accessory();
int connect(int);
void disconnect();
int register_accessory(unsigned char ioBuffer[2]);
void unregister_accessory(libusb_device_handle *handle);
int hid_register(unsigned char data[7]); //human interface device
int hid_unregister();
int send_touch();
void r_run();
void w_run();
void *r_loop();
void *w_loop();


#endif /* ANDROID_OPEN_ACCESSORY_H */


