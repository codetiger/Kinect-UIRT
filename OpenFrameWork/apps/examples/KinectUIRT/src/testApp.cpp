#include "testApp.h"
#include <errno.h>
#include <dlfcn.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#include <stdio.h>

#define DEVICE_PATH "USB-UIRT"

// UUIRT .dll function pointers. These will be assigned when calling LoadDLL()
pfn_UUIRTOpenEx				fnUUIRTOpenEx;
pfn_UUIRTClose				fnUUIRTClose;
pfn_UUIRTGetDrvInfo			fn_UUIRTGetDrvInfo;
pfn_UUIRTGetUUIRTInfo		fn_UUIRTGetUUIRTInfo;
pfn_UUIRTGetUUIRTConfig		fn_UUIRTGetUUIRTConfig;
pfn_UUIRTSetUUIRTConfig		fn_UUIRTSetUUIRTConfig;
pfn_UUIRTSetReceiveCallback	fn_UUIRTSetReceiveCallback;
pfn_UUIRTTransmitIR			fn_UUIRTTransmitIR;
pfn_UUIRTLearnIR			fn_UUIRTLearnIR;

static void *		hinstLib = NULL;
unsigned int	drvVersion;
HUUHANDLE		hDrvHandle;
int		gIRCodeFormat = UUIRTDRV_IRFMT_UUIRT;
char power[2048]= "F41R080A816180B01616161616421616161616161616161616421642161616421642164216421642161616161616164216161616161616161642164216421616164216421642164216";
char nextChannel[2048]= "F41R000069231112111211241124231211121112111210121112111211121112111211121112111211122324111211121112111211";
char prevChannel[2048]= "F41R00046923111211121124112423121112111211121012111211121112111211121112111211121112232411121112111223";
char volumeUp[2048]= "F41R000069231112111211241124231211121112111210121112111211121112111211121112111211121112232411121112111211";
char volumeDown[2048]= "F41R00046923111211121124112423121112111211121012111211121112111211121112111211121112111223241112111223";

void *LoadLibrary(const char* LibName)
{
	char namePath[256];
    //	strcpy(namePath,"./");
    //  strcpy(namePath,"../../../uuirtdrv/build/Debug/lib");
	strcpy(namePath,"lib");
	strcat(namePath, LibName);
    strcat(namePath,".dylib");
	return dlopen(namePath, RTLD_NOW);
};

bool FreeLibrary(void* handle)
{
	return dlclose(handle) == 0;	
};

void *GetProcAddress(void* handle, const char* Name)
{
	return dlsym(handle,Name);	
};

void Sleep(int n)
{
	usleep(1000 * n);   
};

bool kbhit(void)
{
	fd_set rfds;
	struct timeval tv;
	int retval;
    
	// Watch stdin (fd 0) to see when it has input.
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	retval = select(1, &rfds, NULL, NULL, &tv);
	return (retval > 0);
}

char getch(void)
{
	fflush(stdout);
	while (!kbhit())
		Sleep(10);
	return getchar();
}

void clrscr(void)
{
}

#define GetLastError(x) (errno)

void unLoadDLL(void)
{
	if (hinstLib)
		FreeLibrary(hinstLib);
	hinstLib = NULL;
}

BOOL loadDLL(void)
{
    // Get a handle to the DLL module.
    
    hinstLib = LoadLibrary("uuirtdrv"); 
    
    // If the handle is valid, try to get the function address.
    
    if (hinstLib != NULL) 
    { 
        fnUUIRTOpenEx = (pfn_UUIRTOpenEx) GetProcAddress(hinstLib, "UUIRTOpenEx");
        fnUUIRTClose = (pfn_UUIRTClose) GetProcAddress(hinstLib, "UUIRTClose");
		fn_UUIRTGetDrvInfo  = (pfn_UUIRTGetDrvInfo) GetProcAddress(hinstLib, "UUIRTGetDrvInfo");
		fn_UUIRTGetUUIRTInfo = (pfn_UUIRTGetUUIRTInfo) GetProcAddress(hinstLib, "UUIRTGetUUIRTInfo");
		fn_UUIRTGetUUIRTConfig = (pfn_UUIRTGetUUIRTConfig) GetProcAddress(hinstLib, "UUIRTGetUUIRTConfig");
		fn_UUIRTSetUUIRTConfig = (pfn_UUIRTSetUUIRTConfig) GetProcAddress(hinstLib, "UUIRTSetUUIRTConfig");
		fn_UUIRTSetReceiveCallback = (pfn_UUIRTSetReceiveCallback) GetProcAddress(hinstLib, "UUIRTSetReceiveCallback");
		fn_UUIRTTransmitIR = (pfn_UUIRTTransmitIR) GetProcAddress(hinstLib, "UUIRTTransmitIR");
		fn_UUIRTLearnIR = (pfn_UUIRTLearnIR) GetProcAddress(hinstLib, "UUIRTLearnIR");
        
		if (!fnUUIRTOpenEx || 
			!fnUUIRTClose || 
			!fn_UUIRTGetDrvInfo || 
			!fn_UUIRTGetUUIRTInfo || 
			!fn_UUIRTGetUUIRTConfig || 
			!fn_UUIRTSetUUIRTConfig || 
			!fn_UUIRTSetReceiveCallback || 
			!fn_UUIRTTransmitIR || 
			!fn_UUIRTLearnIR)
		{
			unLoadDLL();
			return false;
		}
        
		return true;
	}
	return false;
}

void WINAPI IRReceiveCallback (char *IREventStr, void *userData)
{
	printf("<IR Receive: Code = %s, UserData = %08x!!!\n", IREventStr, userData);
}

void testApp::setup() {
	context.initFromXMLFile();
	depth.setup(&context);
	user.setup(&context, &depth);
	_screenWidth = ofGetScreenWidth();
	_screenHeight = ofGetScreenHeight();
    
    UUINFO UsbUirtInfo;
	UINT32 uConfig;
	char ch;
    
	printf("UUIRTDRV Example Program...\n");
	printf("===========================\n\n");
    
	char devicePath[256];
    
	printf("Using default name: %s\n",DEVICE_PATH);
	strcpy(devicePath, DEVICE_PATH);
    
	if (!loadDLL())
		printf("ERROR: Unable to load uuirtdrv library!\n");
    
	if (!fn_UUIRTGetDrvInfo(&drvVersion))
	{
		printf("ERROR: Unable to retrieve uuirtdrv version!\n");
		unLoadDLL();
	}
    
	if (drvVersion != 0x0100)
	{
		printf("ERROR: Invalid uuirtdrv version!\n");
		unLoadDLL();
	}
    
	hDrvHandle = fnUUIRTOpenEx(devicePath,0,0,0);
	if (hDrvHandle == INVALID_HANDLE_VALUE)
	{
		DWORD err;
        
		err = GetLastError();
        
		if (err == UUIRTDRV_ERR_NO_DLL)
		{
			printf("ERROR: Unable to find USB-UIRT Driver. Please make sure driver is Installed!\n");
		}
		else if (err == UUIRTDRV_ERR_NO_DEVICE)
		{
			printf("ERROR: Unable to connect to USB-UIRT device!  Please ensure device is connected to the computer!\n");
		}
		else if (err == UUIRTDRV_ERR_NO_RESP)
		{
			printf("ERROR: Unable to communicate with USB-UIRT device!  Please check connections and try again.  If you still have problems, try unplugging and reconnecting your USB-UIRT.  If problem persists, contact Technical Support!\n");
		}
		else if (err == UUIRTDRV_ERR_VERSION)
		{
			printf("ERROR: Your USB-UIRT's firmware is not compatible with this API DLL. Please verify you are running the latest API DLL and that you're using the latest version of USB-UIRT firmware!  If problem persists, contact Technical Support!\n");
		}
		else
		{
			printf("ERROR: Unable to initialize USB-UIRT (unknown error)!\n");
		}
        
		unLoadDLL();
        
		printf("Press any key to exit...");
		getch();
		printf("\n");
	}
    
	printf("\n");
    
	// Register a callback function for IR receive...
	fn_UUIRTSetReceiveCallback(hDrvHandle, &IRReceiveCallback, (void *)0xA5A5A5A5);
    
	// Disable tty blocking on Stdio
	struct termios tty;
	struct termios savetty;
	tcgetattr(0, &savetty);
	tcgetattr(0, &tty);
	tty.c_lflag &= ~(ICANON|IEXTEN);
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 0;
	tcsetattr(0, TCSADRAIN, &tty);
    
    if (!fn_UUIRTGetUUIRTInfo(hDrvHandle, &UsbUirtInfo))
    {
        printf("\t*** ERROR Retrieving USB-UIRT Information! ***\n");
    }
    else
    {
        printf("\tUSB-UIRT Info: FirmwareVer=%d.%d ProtVer=%d.%d FirmwareDate=%d/%d/%d\n",
               UsbUirtInfo.fwVersion>>8,
               UsbUirtInfo.fwVersion&0xff,
               UsbUirtInfo.protVersion>>8,
               UsbUirtInfo.protVersion&0xff,
               UsbUirtInfo.fwDateMonth,
               UsbUirtInfo.fwDateDay,
               UsbUirtInfo.fwDateYear+2000);
    }
    if (!fn_UUIRTGetUUIRTConfig(hDrvHandle, &uConfig))
    {
        printf("\t*** ERROR Retrieving USB-UIRT Configuration Info! ***\n");
    }
    
}

//--------------------------------------------------------------
void testApp::update(){
	context.update();
	user.update();
    
	ofxTrackedUser* tracked = user.getTrackedUser(0);
	if(tracked != NULL) {
		int rightHandDistanceX = abs(tracked->right_lower_arm.position[1].X-tracked->right_upper_arm.position[0].X);
		int rightHandDistanceY = abs(tracked->right_lower_arm.position[1].Y-tracked->right_upper_arm.position[0].Y);
		int rightHandDistanceZ = abs(tracked->right_lower_arm.position[1].Z-tracked->right_upper_arm.position[0].Z);
		
		bool rightHandDirectionX = (tracked->right_lower_arm.position[1].X-tracked->right_upper_arm.position[0].X>0)?0:1;
		bool rightHandDirectionY = (tracked->right_lower_arm.position[1].Y-tracked->right_upper_arm.position[0].Y>0)?0:1;
		bool rightHandDirectionZ = (tracked->right_lower_arm.position[1].Z-tracked->right_upper_arm.position[0].Z>0)?0:1;
		
		if(rightHandDistanceX < 40 && rightHandDistanceY < 40 && rightHandDistanceZ < 200)
			gestureRight = RightHandBack;
		else if(rightHandDistanceX < 40 && rightHandDistanceY < 40 && rightHandDistanceZ > 300)
			gestureRight = RightHandFront;
		else if(rightHandDistanceX > 70 && rightHandDistanceY < 40 && rightHandDistanceZ < 200) {
			if(rightHandDirectionX)
				gestureRight = RightHandRight;
			else
				gestureRight = RightHandLeft;
		} else if(rightHandDistanceX < 40 && rightHandDistanceY > 60 && rightHandDistanceZ < 200) {
			if(rightHandDirectionY)
				gestureRight = RightHandUp;
			else
				gestureRight = RightHandDown;
		}
		
        //		printf("Dist: x=%d, y=%d, z=%d\n", rightHandDistanceX, rightHandDistanceY, rightHandDistanceZ);
		if(oldGestureRight != gestureRight && isTracking) {
            if(gestureRight == RightHandFront) {
                if (!fn_UUIRTTransmitIR(hDrvHandle, power, gIRCodeFormat, 10, 0, NULL, NULL, NULL)){
                    printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
                } else {
                    printf("...Returned from call (Done)!\n");
                }
            } else if(gestureRight == RightHandRight) {
                if (!fn_UUIRTTransmitIR(hDrvHandle, nextChannel, gIRCodeFormat, 10, 0, NULL, NULL, NULL)){
                    printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
                } else {
                    printf("...Returned from call (Done)!\n");
                }
            } else if(gestureRight == RightHandLeft) {
                if (!fn_UUIRTTransmitIR(hDrvHandle, prevChannel, gIRCodeFormat, 10, 0, NULL, NULL, NULL)){
                    printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
                } else {
                    printf("...Returned from call (Done)!\n");
                }
            } else if(gestureRight == RightHandUp) {
                if (!fn_UUIRTTransmitIR(hDrvHandle, volumeUp, gIRCodeFormat, 10, 0, NULL, NULL, NULL)){
                    printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
                } else {
                    printf("...Returned from call (Done)!\n");
                }
            } else if(gestureRight == RightHandDown) {
                if (!fn_UUIRTTransmitIR(hDrvHandle, volumeDown, gIRCodeFormat, 10, 0, NULL, NULL, NULL)){
                    printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
                } else {
                    printf("...Returned from call (Done)!\n");
                }
            }
        }
        
		oldGestureRight = gestureRight;
	
        int leftHandDistanceX = abs(tracked->left_lower_arm.position[1].X-tracked->left_upper_arm.position[0].X);
		int leftHandDistanceY = abs(tracked->left_lower_arm.position[1].Y-tracked->left_upper_arm.position[0].Y);
		int leftHandDistanceZ = abs(tracked->left_lower_arm.position[1].Z-tracked->left_upper_arm.position[0].Z);
		
		bool leftHandDirectionX = (tracked->left_lower_arm.position[1].X-tracked->left_upper_arm.position[0].X>0)?0:1;
		bool leftHandDirectionY = (tracked->left_lower_arm.position[1].Y-tracked->left_upper_arm.position[0].Y>0)?0:1;
		bool leftHandDirectionZ = (tracked->left_lower_arm.position[1].Z-tracked->left_upper_arm.position[0].Z>0)?0:1;

        if(leftHandDistanceX < 40 && leftHandDistanceY < 40 && leftHandDistanceZ < 200)
			gestureLeft = LeftHandBack;
		else if(leftHandDistanceX < 40 && leftHandDistanceY < 40 && leftHandDistanceZ > 300)
			gestureLeft = LeftHandFront;

        if(oldGestureLeft != gestureLeft) {
            isTracking = !isTracking;
        }
        oldGestureLeft = gestureLeft;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	//depth.draw(0,0,640,480);
	
	for (int i = 0; i < 2; i++) {
		ofxTrackedUser* tracked = user.getTrackedUser(i);
		if(tracked != NULL) {
			tracked->debugDraw();
            
            if(isTracking) {
                ofSetColor(0, 255, 0);
                ofCircle(tracked->right_lower_arm.position[1].X, tracked->right_lower_arm.position[1].Y, 10);
                ofCircle(tracked->right_upper_arm.position[0].X, tracked->right_upper_arm.position[0].Y, 10);
            }
		}
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

void testApp::PostMouseEvent(CGMouseButton button, CGEventType type, CGPoint point) {
	CGEventRef theEvent = CGEventCreateMouseEvent(NULL, type, point, button);
    CGEventSetType(theEvent, type);
    CGEventPost(kCGHIDEventTap, theEvent);
    CFRelease(theEvent);
}

