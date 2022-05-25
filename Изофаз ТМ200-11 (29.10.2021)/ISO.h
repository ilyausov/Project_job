#ifdef __cplusplus
     #define EXPORTS extern "C" __declspec (dllimport)
#else
     #define EXPORTS
#endif

#define ISO_NoError               0
#define ISO_DriverOpenError       1
#define ISO_DriverNoOpen          2
#define ISO_GetDriverVersionError 3
#define ISO_IntInstallError       4
#define ISO_IntResetCountError    5
#define ISO_IntGetCountError      6
#define ISO_IntRemoveError        7

// Test Functions
EXPORTS short CALLBACK ISO_ShortSub2(short nA, short nB);
EXPORTS float CALLBACK ISO_FloatSub2(float fA, float fB);
EXPORTS WORD  CALLBACK ISO_GetDllVersion(void);
EXPORTS WORD  CALLBACK ISO_GetDriverVersion(WORD *wDriverVersion);

// Digital Input/Output Functions
EXPORTS void  CALLBACK ISO_OutputByte(WORD wPortAddr, UCHAR bOutputVal);
EXPORTS void  CALLBACK ISO_OutputWord(WORD wPortAddr, WORD wOutputVal);
EXPORTS WORD  CALLBACK ISO_InputByte(WORD wPortAddr);
EXPORTS WORD  CALLBACK ISO_InputWord(WORD wPortAddr);

// Driver Functions
EXPORTS WORD  CALLBACK ISO_DriverInit(void);
EXPORTS void  CALLBACK ISO_DriverClose(void);

// Interrupt Functions
EXPORTS WORD  CALLBACK ISO_IntInstall(WORD wBase, WORD wIrq, HANDLE *hEvent);
EXPORTS WORD  CALLBACK ISO_IntRemove();
EXPORTS WORD  CALLBACK ISO_IntGetCount(DWORD *dwIntCount);
EXPORTS WORD  CALLBACK ISO_IntResetCount(void);

