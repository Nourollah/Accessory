// **************************************************************************************************************** //


#include "include/android_open_accessory.h"
#include "include/android_transporter.h"

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

// **************************************************************************************************************** //

/*
android_open_accessory("Faradid, Inc.", "DemoKit", "UsbAccessorySample for ADK", "2.0", "http://www.faradid-co.com", "000000000000001");
*/


// **************************************************************************************************************** //
/**
 * signal_callback_handler() -  Disconnect USB INNTERRUPT aborted.
 *
 * arguments:
 *          signum: provide signal number
 *
 * return:
 *       null
 *
 */


void signal_callback_handler(int signum)
{
    fprintf(stderr, "\ninterrupt %d\n",signum);
    disconnect();
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
    disconnect();
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

	unsigned char buff[1024];
	connect(100);
	r_run();

    // Echo back.
    while (true)
    {
        int len = io_read(buff, sizeof(buff), 2000);
        if (len < 0) error((char*)"openAccessory.read",len);
        buff[len+1] = '\0';
        printf("USB>%s\n", buff);
        // for (int i=0; i<len; i++) buff[i] = buff[i] - 0x20;
        io_write(buff, len, 1000);
    }

}
