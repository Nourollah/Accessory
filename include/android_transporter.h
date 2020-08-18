// **************************************************************************************************************** //
// Created by amirmasoud on 8/17/20.
/**
 * Android Open usb_accessory transporter implementation for libusb-1.0
 */

#ifndef ACCESSORY_ANDROID_TRANSPORTER_H
#define ACCESSORY_ANDROID_TRANSPORTER_H

#define GCC
extern  "C"
{
#include <libusb-1.0/libusb.h>

class android_transporter
{
private:
    libusb_context *contex;
    struct libusb_device_handle* handle;
    uint8_t inEP;
    uint8_t outEP;
    int verProtocol;
    libusb_transfer* inTransfer;
    char versionString[15]; // Bigger than 10, just in case



public:

    android_transporter();
    ~android_transporter();

    int io_read(unsigned char *buf, int len, unsigned int timeout);
    int io_aread(libusb_transfer_cb_fn callback, unsigned char* buffer, int bufferLen, void* userData, unsigned int timeOut);
    int io_write(unsigned char *buf, int len, unsigned int timeout);
    int io_awrite(libusb_transfer_cb_fn callback, unsigned char* buffer, int bufferLen, void* userData, unsigned int timeOut);
    int handle_async(struct timeval* tv);
    int send_string(int index, const char *str);

};

};
#endif //ACCESSORY_ANDROID_TRANSPORTER_H