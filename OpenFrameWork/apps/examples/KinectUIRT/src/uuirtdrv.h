
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the UUIRTDRV_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// UUIRTDRV_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef UUIRTDRV_EXPORTS
//#define UUIRTDRV_API __declspec(dllexport)
	#define UUIRTDRV_API extern "C"
	#define INVALID_HANDLE_VALUE (void *)0xFFFFFFFF
	#define PASCAL
	#define BOOL bool
	typedef unsigned int DWORD;
	typedef unsigned int *LPDWORD;
	typedef unsigned int UINT32;
	typedef unsigned short UINT16;
	typedef signed short INT16;
	typedef unsigned short WORD;
	typedef UINT32 *PUINT32;
	typedef char *PCHAR;
	typedef void *LPVOID;
	typedef void *PVOID;
	typedef char *LPCTSTR;
	#define INFINITE 0xFFFFFFFF

	#include <pthread.h>
   struct s_event_t {
      pthread_cond_t cond;
      pthread_mutex_t mutex;
//      int   signaled;
   };

	typedef pthread_t STHREAD_HANDLE;
	typedef void* SRETURN_TYPE;
int i;

typedef s_event_t	EVENT_HANDLE;

   #define WAIT_OBJECT_0	1
   #define WAIT_TIMEOUT	   2
   #define WAIT_FAILED	   3

#define HANDLE EVENT_HANDLE*


#else
	#define INVALID_HANDLE_VALUE (void *)0xFFFFFFFF
   struct s_event_t {
      pthread_cond_t cond;
      pthread_mutex_t mutex;
      int   signaled;
   };
typedef s_event_t	EVENT_HANDLE;
#define HANDLE EVENT_HANDLE*
	#define BOOL bool
	typedef unsigned int DWORD;
	typedef unsigned int *LPDWORD;
	typedef void *LPVOID;
	#define PASCAL
	typedef unsigned int UINT32;
	typedef UINT32 *PUINT32;

#define UUIRTDRV_API
#endif

typedef unsigned char BYTE;
#define WINAPI

// This class is exported from the uuirtdrv.dll
//class UUIRTDRV_API CUuirtdrv {
//public:
//	CUuirtdrv(void);
//	// TODO: add your methods here.
//};

typedef struct
{
	unsigned int fwVersion;
	unsigned int protVersion;
	unsigned char fwDateDay;
	unsigned char fwDateMonth;
	unsigned char fwDateYear;
} UUINFO, *PUUINFO;

typedef struct UUGPIO
{
	BYTE	irCode[6];
	BYTE	action;
	BYTE	duration;
} *PUUGPIO;

typedef void *HUUHANDLE;

typedef void (WINAPI *PUUCALLBACKPROC) (char *IREventStr, void *userData);
#if 0
typedef void (WINAPI *PLEARNCALLBACKPROC) (unsigned short progress, unsigned short sigQuality, unsigned int carrierFreq, void *userData);
#else
typedef void (WINAPI *PLEARNCALLBACKPROC) (unsigned int progress, unsigned int sigQuality, unsigned long carrierFreq, void *userData);
#endif

//typedef void (__stdcall *PUUCALLBACKPROC)(BSTR pbstr);

//extern UUIRTDRV_API int nUuirtdrv;
UUIRTDRV_API int try1(void);

UUIRTDRV_API HUUHANDLE PASCAL UUIRTOpen(void);

UUIRTDRV_API HUUHANDLE PASCAL UUIRTOpenEx(char *devName, DWORD dwFlagsAndAttributes, void *reserved0, void *reserved1);

#define UUIRTDRV_OPENEX_ATTRIBUTE_EXCLUSIVE	0x0001

UUIRTDRV_API BOOL PASCAL UUIRTClose(HUUHANDLE hHandle);

UUIRTDRV_API BOOL PASCAL UUIRTGetDrvInfo(unsigned int *puAPIVersion);
//UUIRTDRV_API BOOL PASCAL UUIRTGetDrvInfo2(LPDWORD puDrvVersion);

UUIRTDRV_API BOOL PASCAL UUIRTGetDrvVersion(unsigned int *puDLLVersion);

UUIRTDRV_API BOOL PASCAL UUIRTGetUUIRTInfo(HUUHANDLE hHandle, PUUINFO puuInfo);

UUIRTDRV_API BOOL PASCAL UUIRTGetUUIRTConfig(HUUHANDLE hHandle, PUINT32 puConfig);

UUIRTDRV_API BOOL PASCAL UUIRTSetUUIRTConfig(HUUHANDLE hHandle, UINT32 uConfig);

UUIRTDRV_API BOOL PASCAL UUIRTTransmitIR(HUUHANDLE hHandle, char *IRCode, int codeFormat, int repeatCount, int inactivityWaitTime, HANDLE hEvent, void *reserved0, void *reserved1);

UUIRTDRV_API BOOL PASCAL UUIRTLearnIR(HUUHANDLE hHandle, int codeFormat, char *IRCode, PLEARNCALLBACKPROC progressProc, void *userData, signed int *pAbort, unsigned int param1, HANDLE hEvent, void *reserved0);

UUIRTDRV_API BOOL PASCAL UUIRTSetReceiveCallback(HUUHANDLE hHandle, PUUCALLBACKPROC receiveProc, void *userData);

UUIRTDRV_API BOOL PASCAL UUIRTGetUUIRTGPIOCfg(HUUHANDLE hHandle, int *pNumSlots, UINT32 *pdwPortPins, PUUGPIO pGPIOStruct);

UUIRTDRV_API BOOL PASCAL UUIRTSetUUIRTGPIOCfg(HUUHANDLE hHandle, int index, PUUGPIO pGpioSt);

#if 0
UUIRTDRV_API BOOL PASCAL UUIRTEERead(HUUHANDLE hHandle, FT_PROGRAM_DATA *ftData);

UUIRTDRV_API BOOL PASCAL UUIRTEEProgram(HUUHANDLE hHandle, FT_PROGRAM_DATA *ftData);
#endif

#define UUIRTDRV_ERR_NO_DEVICE 0x20000001
#define UUIRTDRV_ERR_NO_RESP   0x20000002
#define UUIRTDRV_ERR_NO_DLL    0x20000003
#define UUIRTDRV_ERR_VERSION   0x20000004
#define UUIRTDRV_ERR_IN_USE    0x20000005

#define ERROR_OPERATION_ABORTED	0x200000FC
#define ERROR_IO_PENDING	0x200000FD
#define ERROR_INVALID_PARAMETER    0x200000FE
#define ERROR_INVALID_HANDLE    0x200000FF


typedef HUUHANDLE	(WINAPI *pfn_UUIRTOpen)		(void);
typedef HUUHANDLE	(WINAPI *pfn_UUIRTOpenEx)		(char *devName, DWORD dwFlagsAndAttributes, void *reserved0, void *reserved1);
typedef BOOL		(WINAPI *pfn_UUIRTClose)	(HUUHANDLE hHandle);
typedef BOOL		(WINAPI *pfn_UUIRTGetDrvInfo)(unsigned int *puDrvVersion);
typedef	BOOL		(WINAPI *pfn_UUIRTGetDrvVersion)(unsigned int *puDLLVersion);
typedef BOOL		(WINAPI *pfn_UUIRTGetUUIRTInfo)(HUUHANDLE hHandle, PUUINFO puuInfo);
typedef BOOL		(WINAPI *pfn_UUIRTGetUUIRTConfig)(HUUHANDLE hHandle, PUINT32 puConfig);
typedef BOOL		(WINAPI *pfn_UUIRTSetUUIRTConfig)(HUUHANDLE hHandle, UINT32 uConfig);
typedef BOOL		(WINAPI *pfn_UUIRTSetReceiveCallback)(HUUHANDLE hHandle, PUUCALLBACKPROC receiveProc, void *userData);
typedef	BOOL		(WINAPI *pfn_UUIRTTransmitIR)(HUUHANDLE hHandle, char *IRCode, int codeFormat, int repeatCount, int inactivityWaitTime, HANDLE hEvent, void *reserved0, void *reserved1);
typedef	BOOL		(WINAPI *pfn_UUIRTLearnIR)(HUUHANDLE hHandle, int codeFormat, char *IRCode, PLEARNCALLBACKPROC progressProc, void *userData, signed int *pAbort, unsigned int param1, void *reserved0, void *reserved1);
typedef	BOOL		(WINAPI *pfn_UUIRTGetUUIRTGPIOCfg)(HUUHANDLE hHandle, int *pNumSlots, UINT32 *pdwPortPins, PUUGPIO pGPIOStruct);
typedef	BOOL		(WINAPI *pfn_UUIRTSetUUIRTGPIOCfg)(HUUHANDLE hHandle, int index, PUUGPIO pGpioSt);
#if 0
typedef	BOOL		(WINAPI *pfn_UUIRTEERead)(HUUHANDLE hHandle, FT_PROGRAM_DATA *ftData);
typedef	BOOL		(WINAPI *pfn_UUIRTEEProgram)(HUUHANDLE hHandle, FT_PROGRAM_DATA *ftData);
#endif

#define UUIRTDRV_CFG_LEDRX		0x0001
#define UUIRTDRV_CFG_LEDTX		0x0002
#define UUIRTDRV_CFG_LEGACYRX	0x0004

#define UUIRTDRV_IRFMT_UUIRT	0x0000
#define UUIRTDRV_IRFMT_PRONTO	0x0010

#define UUIRTDRV_IRFMT_LEARN_FORCERAW	0x0100
#define UUIRTDRV_IRFMT_LEARN_FORCESTRUC	0x0200
#define UUIRTDRV_IRFMT_LEARN_FORCEFREQ	0x0400
#define UUIRTDRV_IRFMT_LEARN_FREQDETECT	0x0800
#define UUIRTDRV_IRFMT_LEARN_UIR		0x4000
#define UUIRTDRV_IRFMT_LEARN_DEBUG		0x8000

#define UUIRTDRV_IRFMT_TRANSMIT_DC		0x0080
