#include "Windows.h"

#ifndef EXPORTS
#ifdef  __cplusplus
	#define EXPORTS extern "C" __declspec (dllimport)
#else
	#define EXPORTS __declspec (dllimport)
#endif 
#endif

#ifndef __PISO_DIO_HEADER__
#define __PISO_DIO_HEADER__

// return code
#define PISODIO_NoError                     0
#define PISODIO_DriverOpenError             1
#define PISODIO_DriverNoOpen                2
#define PISODIO_GetDriverVersionError       3
#define PISODIO_InstallIrqError             4
#define PISODIO_ClearIntCountError          5
#define PISODIO_GetIntCountError            6
#define PISODIO_RegisterApcError            7
#define PISODIO_RemoveIrqError              8
#define PISODIO_FindBoardError              9
#define PISODIO_ExceedBoardNumber           10
#define PISODIO_ResetError                  11

// to trigger a interrupt when high -> low
#define PISODIO_ActiveLow                   0
// to trigger a interrupt when low -> high
#define PISODIO_ActiveHigh                  1

// ID
#define PISO_P16R16U                  0x18000000    // for PISO-P16R16U
#define PISO_C64                        0x800800    // for PISO-C64
#define PISO_P64                        0x800810    // for PISO-P64
#define PISO_A64                        0x800850    // for PISO-A64
#define PISO_P32C32                     0x800820    // for PISO-P32C32 
#define PISO_P32A32                     0x800870    // for PISO-P32A32 
#define PISO_P8R8                       0x800830    // for PISO-P8R8
#define PISO_P8SSR8AC                   0x800830    // for PISO-P8SSR8AC
#define PISO_P8SSR8DC                   0x800830    // for PISO-P8SSR8DC
#define PISO_730                        0x800840    // for PISO-730
#define PISO_730A                       0x800880    // for PISO-730A

// Test functions
EXPORTS float  CALLBACK PISODIO_FloatSub(float fA, float fB);
EXPORTS short  CALLBACK PISODIO_ShortSub(short nA, short nB);
EXPORTS WORD   CALLBACK PISODIO_GetDllVersion(void);

// Driver functions
EXPORTS WORD   CALLBACK PISODIO_DriverInit(void);
EXPORTS void   CALLBACK PISODIO_DriverClose(void);
EXPORTS WORD   CALLBACK PISODIO_SearchCard(WORD *wBoards, DWORD dwPIOCardID);
EXPORTS WORD   CALLBACK PISODIO_GetDriverVersion(WORD *wDriverVersion);
EXPORTS WORD   CALLBACK PISODIO_GetConfigAddressSpace(WORD  wBoardNo, DWORD *wAddrBase, WORD *wIrqNo,
                           WORD *wSubVendor, WORD *wSubDevice, WORD *wSubAux,
                           WORD *wSlotBus,   WORD *wSlotDevice);
EXPORTS WORD   CALLBACK PISODIO_ActiveBoard( WORD wBoardNo );
EXPORTS WORD   CALLBACK PISODIO_WhichBoardActive(void);

// DIO functions
EXPORTS void   CALLBACK PISODIO_OutputWord(DWORD wPortAddress, DWORD wOutData);
EXPORTS void   CALLBACK PISODIO_OutputByte(DWORD wPortAddr, WORD bOutputValue);
EXPORTS DWORD  CALLBACK PISODIO_InputWord(DWORD wPortAddress);
EXPORTS WORD   CALLBACK PISODIO_InputByte(DWORD wPortAddr);

// Interrupt functions
EXPORTS WORD   CALLBACK PISODIO_IntInstall(WORD wBoardNo, HANDLE *hEvent, WORD wInterruptSource, WORD wActiveMode);
EXPORTS WORD   CALLBACK PISODIO_IntRemove(void);
EXPORTS WORD   CALLBACK PISODIO_IntResetCount(void);
EXPORTS WORD   CALLBACK PISODIO_IntGetCount(DWORD *dwIntCount);

#endif // end define __PISO_DIO_HEADER__

