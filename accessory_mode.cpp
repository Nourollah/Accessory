// **************************************************************************************************************** //
/**
 * Android USB connection sample.
 */
#include "include/android_open_accessory.h"
#include "include/android_transporter.h"

#ifdef CLANG

#include <cstdlib>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <pthread.h>

#else

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

#endif
// **************************************************************************************************************** //
/**
 * USB Connector opend.
 */
android_open_accessory openAccessory
(
    "Faradid, Inc.",                             // manufacturer
         "DemoKit",                                   // model
      "UsbAccessorySample for ADK",               // description
        "2.0",                                       // version
           "http://www.faradid-co.com", // url
         "000000000000001"
);


// **************************************************************************************************************** //
/**
 * signal_callback_handler() -  Disconnect USB INNTERRUPT aborted.
 *
 * arguments:
 *          signum: provide signal number
 *
 * return:
 *       null
 */
void signal_callback_handler(int signum)
{
    fprintf(stderr, "\ninterrupt %d\n",signum);
    openAccessory.disconnect();
    exit(0);
}


// **************************************************************************************************************** //
/**
 * error() -  Show error content
 *
 * arguments:
 *          msg: Buffer for error content
 *          rc:  Error number
 *
 * return:
 *       null
 */
static void error(char *msg, int rc)
{
	fprintf(stderr,"Error(%d,%s): %s\n",rc,strerror(errno),msg);
	openAccessory.disconnect();
	exit(0);
}


// **************************************************************************************************************** //
/**
 * Main() -  Start Point
 *
 * arguments:
 *
 *
 * return:
 */
int main(int argc, char *argv[])
{
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);

    pthread_t readThread, writeThread;
    int pt_read, pt_write;



	unsigned char buff[1024];
	openAccessory.connect(100);
	openAccessory.r_run();

    // Echo back.
    while (true)
    {
        int len = openAccessory.io_read(buff, sizeof(buff), 2000);
        if (len < 0) error((char*)"openAccessory.read",len);
        buff[len+1] = '\0';
        printf("USB>%s\n", buff);
        // for (int i=0; i<len; i++) buff[i] = buff[i] - 0x20;
        openAccessory.io_write(buff, len, 1000);
    }

}
