// **************************************************************************************************************** //
/**
 * Android USB connection sample.
 */
#include "AOA/AOA.h"

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>


// **************************************************************************************************************** //
/**
 * USB Connector opend.
 */
AOA acc
(
    "Faradid, Inc.",                        // manufacturer
    "DemoKit",                                   // model
    "UsbAccessorySample for ADK",             // description
    "2.0",                                      // version
    "https://github.com/mapyo/UsbAccessorySample", // url
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
    acc.disconnect();
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
	acc.disconnect();
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

	acc.connect(100);


    // Echo back.
    while (1) {
        int len = acc.read(buff, sizeof(buff), 1000000);
        if (len < 0) error((char*)"acc.read",len);
        buff[len+1] = '\0';
        printf("USB>%s\n", buff);
        // for (int i=0; i<len; i++) buff[i] = buff[i] - 0x20;
        acc.write(buff, len, 1000);
    }

}
