// **************************************************************************************************************** //
// Created by amirmasoud on 8/17/20.
/**
 * Android Open usb_accessory transporter implementation for libusb-1.0
 */


#include <string.h>
#include <pthread.h>



#ifndef ACCESSORY_ANDROID_TRANSPORTER_H
#define ACCESSORY_ANDROID_TRANSPORTER_H

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

#define ACCESSORY_GET_PROTOCOL              51
#define ACCESSORY_SEND_STRING               52
#define ACCESSORY_START                     53

// for async transfer.
#define EP_COMMAND    (2 | LIBUSB_ENDPOINT_OUT) // OUT of PC to USB-device //DEVICE_SPECIFIC
#define EP_RESPONSE   (4 | LIBUSB_ENDPOINT_IN ) // IN  PC from  USB-device //DEVICE_SPECIFIC

#define IN 0x85
#define OUT 0x07


#include <libusb-1.0/libusb.h>
#define GCC



    libusb_context *context;
    struct libusb_device_handle* handle;
    uint8_t inEP;
    uint8_t outEP;
    int verProtocol;
    struct libusb_transfer* inTransfer;
    char versionString[15]; // Bigger than 10, just in case




    void android_transporter();
    void android_transporter();

    int io_read(unsigned char *buf, int len, unsigned int timeout);
    int io_async_read(libusb_transfer_cb_fn callback, unsigned char* buffer, int bufferLen, void* userData, unsigned int timeOut);
    int io_write(unsigned char *buf, int len, unsigned int timeout);
    int io_async_write(libusb_transfer_cb_fn callback, unsigned char* buffer, int bufferLen, void* userData, unsigned int timeOut);
    int handle_async(struct timeval* tv);
    int send_string(int index, const char *str);



#endif //ACCESSORY_ANDROID_TRANSPORTER_H