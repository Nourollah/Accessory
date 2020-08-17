// **************************************************************************************************************** //

/**
 * Android Open Accessory implementation for libusb-1.0
 */

#ifndef __AOA_h__
#define __AOA_h__

#include <libusb-1.0/libusb.h>

// **************************************************************************************************************** //
/**
 * This class presentation function to get USB Accessory mode on Android, read and write stream over USB
 */
class AOA
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
    char versionString[15]; // Bigger than 10, just in case

    int search_device(libusb_context *ctx, uint16_t *idVendor, uint16_t *idProduct);
    int find_end_Point(libusb_device *dev);
    int is_accessory_device (libusb_context *ctx);
    int get_protocol();
    int send_string(int index, const char *str);

public:
    AOA
    (
        const char *manufacturer,
        const char *model,
        const char *description,
        const char *version,
        const char *uri,
        const char *serial
    );
    ~AOA();
    int connect(int);
    void disconnect();
    int read(unsigned char *buf, int len, unsigned int timeout);
    int read_async(libusb_transfer_cb_fn callback, char* buffer, int bufferLen, void* userData, unsigned int timeout);
    int handle_async(struct timeval* tv);
    int write(unsigned char *buf, int len, unsigned int timeout);
};


#endif /* __AOA_h__ */


