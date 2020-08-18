#include "android_transporter.h"

void android_transporter()
{
    //pt_read = pthread_create(&readThread, NULL, write, &write_async, );
};

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
int io_read(unsigned char *buf, int len, unsigned int timeout)
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
int io_async_read(libusb_transfer_cb_fn callback, unsigned char* buffer, int bufferLen, void* userData, unsigned int timeOut)
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
int handle_async(struct timeval* tv) {
    return libusb_handle_events_timeout(context, tv);
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
int io_write(unsigned char *buf, int len, unsigned int timeout)
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
int send_string(int index, const char *str)
{
    int res;
    res = libusb_control_transfer(handle, 0x40 , ACCESSORY_SEND_STRING, 0, index, (unsigned char*)str, strlen(str) + 1, 2000);
    return(res);
}

// **************************************************************************************************************** //
/**
 * handle_async() - Write async data from accessory device
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
int io_async_write(libusb_transfer_cb_fn callback, unsigned char* buffer, int bufferLen, void* userData, unsigned int timeOut)
{
    libusb_fill_bulk_transfer(inTransfer, handle, outEP, /* EP_RESPONSE */ (uint8_t*)buffer, bufferLen, callback, userData, timeOut);
    inTransfer->type = LIBUSB_TRANSFER_TYPE_BULK;
    int result = libusb_submit_transfer(inTransfer);
    return result;
}
