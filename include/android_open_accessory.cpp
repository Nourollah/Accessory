#include "android_open_accessory.h"

#ifdef CLANG

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <pthread.h>

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#endif



android_open_accessory::android_open_accessory
(const char *manufacturer, const char *model, const char *description, const char *version, const char *uri, const char *serial):
 manufacturer(manufacturer), model(model), description(description), version(version), uri(uri), serial(serial), handle(NULL), contex(NULL)
{
    inTransfer = libusb_alloc_transfer(0);
}


android_open_accessory::~android_open_accessory()
{
    this->disconnect();
    if( inTransfer != NULL )
    {
       libusb_free_transfer(inTransfer);
    }
}


void android_open_accessory::disconnect()
{
    if(handle != NULL ){
        libusb_release_interface (handle, 0);
        libusb_close(handle);
	handle = NULL;
    }
    if(contex != NULL ){
       libusb_exit(contex);
        contex = NULL;
    }
}

// **************************************************************************************************************** //
/**
 * connect() - connect to android_open_accessory device
 *
 * argument:
 *         retry: Retry times for connect to new PID
 *
 * return:
 *       0 : connection success
 *       -1 : connection failed.(android_open_accessory device not found)
 *       -2 : connection failed.(usb_accessory mode switch failed)
 *       -3 : connection failed.(libusb_claim_interface failed)
 */
int android_open_accessory::connect(int retry)
{
    if(libusb_init(&contex) < 0){
       printf("libusb_init failed\n");
       return 1;
    }
    unsigned char ioBuffer[2];
    int protocol;
    int res;
    int tries = retry;
    uint16_t idVendor, idProduct;

    // Search for android_open_accessory support device in the all USB devices
    if ((protocol= search_device(contex, &idVendor, &idProduct)) < 0) {
        printf("android_open_accessory device not found.\n");
        return -1;
    }

    // Already in accessory mode ?
    if( protocol == 0 ) {
       handle = libusb_open_device_with_vid_pid(contex, idVendor, idProduct);
       libusb_claim_interface(handle, 0);
       return 0;
    }
    verProtocol = protocol;

    handle = libusb_open_device_with_vid_pid(contex, idVendor, idProduct);
    libusb_claim_interface(handle, 0);
    usleep(2000);//sometimes hangs on the next transfer :(

    // register accessory
    res = register_accessory(ioBuffer);
    if ( res < 0 )
    {
        return -2;
    }

    // Wait a moment
    usleep(1000);

    printf("connect to new PID...\n");
    // Attempt to connect to new PID, if that doesn't work try ACCESSORY_PID_ALT
    for(;;){
        tries--;
        if(search_device(contex, &idVendor, &idProduct) != 0 ){
            sleep(1);
            continue;
        }
        if((handle = libusb_open_device_with_vid_pid(contex, idVendor, idProduct)) == NULL){
            if(tries < 0){
                return -1;
            }
        }else{
            break;
        }
        sleep(1);
    }

    res = libusb_claim_interface(handle, 0);
    if(res < 0){
        return -3;
    }

    printf("Established android_open_accessory connection.\n");
    return 0;
}


// **************************************************************************************************************** //
/**
 * get_protocol() - retrieve android_open_accessory protocol version
 *
 * argument:
 *         none
 *
 * return:
 *       -1 : Error (libusb_bulk_transfer error code)
 *       >0 : Protocol version
 */
int android_open_accessory::get_protocol()
{
    unsigned short protocol;
    unsigned char buf[2];
    int res;

    res = libusb_control_transfer(handle, 0xc0, ACCESSORY_GET_PROTOCOL, 0, 0, buf, 2, 0);
    if(res < 0){
        return -1;
    }
    protocol = buf[1] << 8 | buf[0];
    return((int)protocol);
}


// **************************************************************************************************************** //
/**
 * search_device() -  Search for android_open_accessory support device in the all USB devices
 *
 * argument:
 *         contex      : libusb_context
 *         idVendor : return buffer for USB Vendor ID
 *         idProduc : return buffer for USB Product ID
 *
 * return:
 *       -1 : android_open_accessory device not found
 *       0> : android_open_accessory Version
 */
int android_open_accessory::search_device(libusb_context *ctx, uint16_t *idVendor, uint16_t *idProduct)
{
    int res;
    int i;
    libusb_device **devs;
    libusb_device *dev;
    struct libusb_device_descriptor desc;
    ssize_t devcount;

    *idVendor = *idProduct = 0;
    res = -1;
    devcount = libusb_get_device_list(ctx, &devs);
    if(devcount <= 0){
       printf("Get device error.\n");
       return -1;
    }

    //enumerate USB deivces
    for(i=0; i<devcount; i++){
        dev = devs[i];
        libusb_get_device_descriptor(dev, &desc);
#ifdef DEBUG
        printf("VID:%04X, PID:%04X Class:%02X\n", desc.idVendor, desc.idProduct, desc.bDeviceClass);
#endif
        //Ignore non target device
        if( desc.bDeviceClass != 0 ){
            continue;
        }

        //Already android_open_accessory mode ?
        if(desc.idVendor == USB_ACCESSORY_VENDOR_ID &&
            (desc.idProduct >= USB_ACCESSORY_PRODUCT_ID &&
             desc.idProduct <= USB_ACCESSORY_AUDIO_ADB_PRODUCT_ID)
        ){
#ifdef DEBUG
            printf("already in accessory mode.\n");
#endif
            res = 0;
            break;
        }

        //Checking the android_open_accessory capability.
        if((handle = libusb_open_device_with_vid_pid(ctx, desc.idVendor,  desc.idProduct)) == NULL) {
               printf("Device open error.\n");
        } else {
                libusb_claim_interface(handle, 0);
                res = get_protocol();
                libusb_release_interface (handle, 0);
                libusb_close(handle);
                handle = NULL;
                if( res != -1 ){
#ifdef DEBUG
    printf("android_open_accessory protocol version: %d\n", verProtocol);
#endif
                    break; //android_open_accessory found.
                }
        }
    }

    // find end point number
    if(find_end_Point(dev) < 0 ){
        printf("Endpoint not found.\n");
        res = -1;
    }
    *idVendor = desc.idVendor;
    *idProduct = desc.idProduct;
#ifdef DEBUG
    printf("VID:%04X, PID:%04X\n", *idVendor, *idProduct);
#endif
    return res;
}


// **************************************************************************************************************** //
/**
 * find_end_Point() -  find end point number
 *
 * argument:
 *         dev : libusb_device
 *
 * return:
 *       0 : Success
 *       -1 : Valid end point not found
 */
int android_open_accessory::find_end_Point(libusb_device *dev)
{
    struct libusb_config_descriptor *config;
    libusb_get_config_descriptor (dev, 0, &config);

    //initialize end point number
    inEP = outEP = 0;

    //Evaluate first interface and endpoint descriptor
#ifdef DEBUG
    printf("bNumInterfaces: %d\n", config->bNumInterfaces);
#endif
    const struct libusb_interface *itf = &config->interface[0];
    struct libusb_interface_descriptor ifd = itf->altsetting[0];
#ifdef DEBUG
    printf("bNumEndpoints: %d\n", ifd.bNumEndpoints);
#endif
    for(int i=0; i<ifd.bNumEndpoints; i++){
        struct libusb_endpoint_descriptor epd;
        epd = ifd.endpoint[i];
        if( epd.bmAttributes == 2 ) { //Bulk Transfer ?
            if( epd.bEndpointAddress & 0x80){ //IN
                if( inEP == 0 )
                    inEP = epd.bEndpointAddress;
            }else{                            //OUT
                if( outEP == 0 )
                    outEP = epd.bEndpointAddress;
            }
        }
#ifdef DEBUG
    printf(" bEndpointAddress: %02X, bmAttributes:%02X\n", epd.bEndpointAddress, epd.bmAttributes);
#endif
    }
    if( outEP == 0 || inEP == 0) {
        return -1;
    }else{
        return 0;
    }
}


// **************************************************************************************************************** //
/**
 * register_accessory() - register to USB Accessory Mode.
 *
 * argument:
 *         none
 *
 * return:
 *       <0 : Error occur when switched
 *       >0 : Switched to Accessory mode success
 */
int android_open_accessory::register_accessory(unsigned char ioBuffer[2])
{
    int response;
    // Create register
    libusb_control_transfer(handle,0xC0,51,0,0,ioBuffer,2,2000);

    // Send accessory identifications
    send_string(ACCESSORY_STRING_MANUFACTURER, (char *) manufacturer);
    send_string(ACCESSORY_STRING_MODEL, (char *) model);
    send_string(ACCESSORY_STRING_DESCRIPTION, (char *) description);
    send_string(ACCESSORY_STRING_VERSION, (char *) version);
    send_string(ACCESSORY_STRING_URI, (char *) uri);
    send_string(ACCESSORY_STRING_SERIAL, (char *) serial);

    // Switch to accessory mode
    response = libusb_control_transfer(handle,0x40,ACCESSORY_START,0,0,ioBuffer,0,2000);
    if(response < 0){
        libusb_close(handle);
        printf("bad request");
        handle = NULL;
        return -1;
    }
    sleep(2);
    if(handle != NULL){
        libusb_close(handle);
        handle = NULL;
    }

    return response;
}


// **************************************************************************************************************** //
/**
 * unregister_accessory() - retrieve android_open_accessory protocol version
 *
 * argument:
 *         handle: libusb_device_handle struct
 *
 * return:
 *       null
 *
 */
void android_open_accessory::unregister_accessory(libusb_device_handle *handle)
{
    libusb_release_interface(handle,0);
    sleep(1);
}


// **************************************************************************************************************** //
/**
 * hid_register() - connect to android_open_accessory device
 *
 * argument:
 *         ss
 *
 * return:
 *       0
 */
int android_open_accessory::hid_register(unsigned char data[WEIGH_REPORT_SIZE])
{
    libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS /* | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_CAP_HAS_HID_ACCESS*/ , WEIGH_REPORT_SIZE, 0x0100,\
    0x00, data, WEIGH_REPORT_SIZE, 2000);
    return 0;
}


// **************************************************************************************************************** //
/**
 * r_quit() -  Change r_mQuiting value to true for give ending to r_loop or thread;
 */
void android_open_accessory::r_quit()
{
    r_mQuiting = true;
}


// **************************************************************************************************************** //


/**
 * r_run() -  Read Async data from USB Accessory.
 */
void android_open_accessory::r_run()
{
    pthread_t runner;
    int pthreadRun;
    void *unused;

    // Separate read data thread from main thread
    pthreadRun = pthread_create(&runner, NULL , &r_loop, NULL);

    (pthreadRun>0)? pthread_join(runner, NULL): printf("\nCreate read thread unsuccessfully\n");
    pthread_exit(NULL);

    // Release stream interface
    unregister_accessory(handle);
}


// **************************************************************************************************************** //
/**
 * w_run() -  Write Async data over USB Accessory.
 */
void android_open_accessory::w_run()
{
    pthread_t runner;
    int pthreadRun;

    // Separate read data thread from main thread
    pthreadRun = pthread_create(&runner, NULL , &w_loop(), NULL);

    (pthreadRun>0)? pthread_join(runner, NULL): printf("\nCreate read thread unsuccessfully\n");
    pthread_exit(NULL);

    // Release stream interface
    unregister_accessory(handle);
}


// **************************************************************************************************************** //
/**
 * r_loop() -  Read Async data from USB Accessory
 */
void * r_loop(void *)
{
    unsigned char *buffer;
    int length;
    void* userData;
    void *unused;
    while (!r_mQuiting)
    {
        if (buffer == NULL)
        {
            buffer = (unsigned char*)malloc(length);
        }
        else
        {
            free(buffer);
            buffer = (unsigned char*)malloc(length);
        }

        try
        {
            io_aread(cbTransfer, buffer, length, userData, 2000);
        }
        catch (int error)
        {
            printf("\nAsync Read buffer unsuccessfully. %d", error);
        }
    }

}


// **************************************************************************************************************** //
/**
 * w_loop() -  Write Async string data over USB Accessory
 */
void w_loop()
{
    volatile bool flag;

    libusb_transfer_cb_fn cbTransfer;
    unsigned char *buffer;
    int length;
    void* userData;

    while (!flag)
    {
        try {
            if (buffer == NULL)
            {
                buffer = (unsigned char*)malloc(length);
            }
            else
            {
                free(buffer);
                buffer = (unsigned char*)malloc(length);
            }
        }
        catch (int error)
        {
            printf("Can't allocate correct buffer. error: %d",error);
            flag = true;
        }

        try
        {
            io_awrite(cbTransfer, buffer, length, userData, 2000);
        }
        catch (int error)
        {
            printf("\nAsync Write buffer unsuccessfully. %d", error);
            flag = true;
        }
    }

    if ();
}
