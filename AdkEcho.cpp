/**
Android USB connection sample.

@Author kotemru.org
@Licence apache/2.0
*/

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "AOA/AOA.h"

// USB Connector opend.
AOA acc("Google, Inc.",   // manufacturer
        "DemoKit",        // model
        "UsbAccessorySample for ADK", // description
        "2.0",            // version
        "https://github.com/mapyo/UsbAccessorySample", // url
        "000000000000001") ;

/**
 * Disconnect USB INNTERRUPT aborted.
 */
void signal_callback_handler(int signum)
{
    fprintf(stderr, "\ninterrupt %d\n",signum);
    acc.disconnect();
    exit(0);
}

static void error(char *msg, int rc) {
	fprintf(stderr,"Error(%d,%s): %s\n",rc,strerror(errno),msg);
	acc.disconnect();
	exit(0);
}


/**
 * Main function.
 */
int main(int argc, char *argv[])
{
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);

	unsigned char buff[1024];

	acc.connect(100);
    printf("\ngo next step: send audio\n");

    // send auido over usb
    /*FILE *audio;
    audio = fopen("$HOME/Downloads/audio.mp3","r");
    unsigned char* audio_buf = (unsigned char*)malloc(sizeof(audio));
    fread(audio_buf, sizeof(char), sizeof(*audio), audio);
    acc.write(audio_buf,sizeof(audio_buf),1000);
    printf("\nAudio sended\n");*/


    // Echo back.
    while (1) {
        int len = acc.read(buff, sizeof(buff), 1000000);
        if (len < 0) error((char*)"acc.read",len);
        buff[len+1] = '\0';
        printf("USB>%s\n", buff);
        // for (int i=0; i<len; i++) buff[i] = buff[i] - 0x20;
        acc.write(buff, len, 1000);
    }

    acc.disconnect();


    return 0;

}
