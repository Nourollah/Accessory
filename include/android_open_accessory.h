// **************************************************************************************************************** //
/**
 * Android Open usb_accessory connection implementation for libusb-1.0
 */

#ifndef __AOA_h__
#define __AOA_h__

#include "android_transporter.h"
// **************************************************************************************************************** //
/**
 * This class presentation function to get USB usb_accessory mode on Android, read and write stream over USB
 */
class android_open_accessory : protected android_transporter
{
private:
    const char *manufacturer;
    const char *model;
    const char *description;
    const char *version;
    const char *uri;
    const char *serial;

    libusb_context *contex;
    struct libusb_device_handle* handle;
    uint8_t inEP;
    uint8_t outEP;
    int verProtocol;

    libusb_transfer* inTransfer;

    int search_device(libusb_context *ctx, uint16_t *idVendor, uint16_t *idProduct);
    int find_end_Point(libusb_device *dev);
    int get_protocol();

public:
    android_open_accessory
    (const char *manufacturer, const char *model, const char *description, const char *version, const char *uri, const char *serial);

    ~android_open_accessory();
    int connect(int);
    void disconnect();
    int register_accessory(unsigned char ioBuffer[2]);
    void unregister_accessory(libusb_device_handle *handle);
    int hid_register(unsigned char data[7]); //human interface device
    int hid_unregister();

};


#endif /* __AOA_h__ */


