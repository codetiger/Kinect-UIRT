#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

/*
#include "debug.h"
#include "ribsu-util.h"
#include "usb.h"
#include "tty.h"
#include "uirt-raw.h"
#include "uirt-sm.h"
#include "ribsu.h"

#define MODULE_NAME main
DBG_MODULE_DEFINE();

ribsu_ctx ribsu;

static void ribsu_read_callback(void *ctx0, buffer *buf);
void stdin_read_callback(CFSocketRef s, 
                         CFSocketCallBackType callbackType, 
                         CFDataRef address, 
                         const void *data, 
                         void *info);
void signal_handler(int sigraised);
void usage(void);
*/

//========================================================================
int main() {

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,768, OF_WINDOW);			// <-------- setup the GL context

	testApp *myapp = new testApp();
	ofRunApp(myapp);
	
//ribsu_write(ribsu, );
}
/*
void ribsu_read_callback(void *ctx0, buffer *buf) {
    buffer *hex;
    UInt32 i;
    hex = buf_alloc(1025);
    if (!hex)
    {
        ERR("Failed to allocate buffer\n");
        return;
    }
	
    u_buf2hex(buf, hex);
    i = 0;
    while (hex->buf[i])
    {
        printf("%c", hex->buf[i]);
        if ((i & 3) == 3)
        {
            printf(" ");
        }
        i++;
    }
    printf("\n");
    
    buf_free(hex);
}

void stdin_read_callback(CFSocketRef s, CFSocketCallBackType callbackType, CFDataRef address, const void *data, void *info) {
	printf("STD callback\n");
}

void signal_handler(int sigraised) {
    CFRunLoopStop(CFRunLoopGetCurrent());
}

void usage(void)
{
    USG("ribsu [-u] [-v VID] [-p PID] | [-t <device>] [-d]\n"
        "\t-u try direct USB using IOKit\n"
        "\t-t try TTY device specified, - to auto-detect device name (requires FTDI driver, version 2.0 or better)\n"
        "\t-v use USB VID\n"
        "\t-p use USB PID\n"
        "\t-d increment debug level\n");   
}
*/