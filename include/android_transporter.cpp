//
// Created by amirmasoud on 8/17/20.
//

#include "android_transporter.h"
#include <cstring>
#include <pthread.h>


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


// **************************************************************************************************************** //
/**
 * read() - read data from accessory device
 *
 * argument:
 *         buf     : data buffer
 *         len     : data length
 *         timeout : wait time(ms)
 *
 * return:
 *       <0 : Error (libusb_bulk_transfer error code)
 *       >=0 : Succes(received bytes)
 */
int android_transporter::read(unsigned char *buf, int len, unsigned int timeout, pthread_t readThread)
{
    int xferred;
    int res = libusb_bulk_transfer(handle, inEP, buf, len, &xferred, timeout);
    if(res == 0) res = xferred;
    return(res);
}

// **************************************************************************************************************** //
/**
 * handle_async() - read async data from accessory device
 *
 * argument:
 *         callback: callback function.
 *         buffer: Buffer content data for read
 *         bufferLen: Length of data buffer
 *         userData: User data to pass to callback function
 *         timeOut: Timeout for the transfer in milliseconds
 *
 *         void function(struct libusb_transfer *transfer)
 *
 * return:
 *       <0 : LIBUSB_ERROR_NO_DEVICE if the device has been disconnected and a LIBUSB_ERROR code on other failure.
 *       =0 : Succes
 */
int android_transporter::read_async(libusb_transfer_cb_fn callback, char* buffer, int bufferLen, void* userData, unsigned int timeOut, pthread_t readThread)
{
    libusb_fill_bulk_transfer(inTransfer, handle, inEP, /* EP_RESPONSE */ (uint8_t*)buffer, bufferLen, callback, userData, timeOut);
    inTransfer->type = LIBUSB_TRANSFER_TYPE_BULK;
    int result = libusb_submit_transfer(inTransfer);
    return result;
}

// **************************************************************************************************************** //
/**
 * handle_async() - read async data from accessory device
 *
 * argument:
 *         tv : timeout value.
 *
 * return:
 *       <0 : ERROR
 *       =0 : Succes
 */
int android_transporter::handle_async(struct timeval* tv) {
    return libusb_handle_events_timeout(contex, tv);
}

// **************************************************************************************************************** //
/**
 * write() - write data to accessory device
 *
 * argument:
 *         *buf    : data buffer
 *         len     : data length
 *         timeout : wait time(ms)
 *
 * return:
 *       <0 : Error (libusb_bulk_transfer error code)
 *       >=0 : Succes(received bytes)
 */
int android_transporter::write(unsigned char *buf, int len, unsigned int timeout, pthread_t writeThread)
{
    int xferred;
    int res = libusb_bulk_transfer(handle, outEP, buf, len, &xferred, timeout);
    if(res == 0) res = xferred;
    return(res);
}

// **************************************************************************************************************** //
/**
 * send_string() - send accessory identifications
 *
 * argument:
 *         index   : string ID
 *         str     : identification string(zero terminated UTF8 string)
 *
 * return:
 *       <0 : Error (libusb_bulk_transfer error code)
 *       0 : Success
 */
int android_transporter::send_string(int index, const char *str)
{
    int res;
    res = libusb_control_transfer(handle, 0x40 , ACCESSORY_SEND_STRING, 0, index, (unsigned char*)str, strlen(str) + 1, 2000);
    return(res);
}

// **************************************************************************************************************** //
/**
 * handle_async() - write async data from accessory device
 *
 * argument:
 *         callback: callback function.
 *         buffer: Buffer content data to write
 *         bufferLen: Length of data buffer
 *         userData: User data to pass to callback function
 *         timeOut: Timeout for the transfer in milliseconds
 *
 *         void function(struct libusb_transfer *transfer)
 *
 * return:
 *       <0 : LIBUSB_ERROR_NO_DEVICE if the device has been disconnected and a LIBUSB_ERROR code on other failure.
 *       =0 : Succes
 */
int android_transporter::write_async(libusb_transfer_cb_fn callback, char* buffer, int bufferLen, void* userData, unsigned int timeOut, pthread_t writeThread)
{
    libusb_fill_bulk_transfer(inTransfer, handle, outEP, /* EP_RESPONSE */ (uint8_t*)buffer, bufferLen, callback, userData, timeOut);
    inTransfer->type = LIBUSB_TRANSFER_TYPE_BULK;
    int result = libusb_submit_transfer(inTransfer);
    return result;
}

