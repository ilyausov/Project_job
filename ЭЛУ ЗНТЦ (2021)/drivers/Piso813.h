#ifdef __cplusplus
     #define EXPORTS extern "C" __declspec (dllimport)
#else
     #define EXPORTS
#endif

/************ define PISO813 relative address **********************/
#define	PISO813_AD_LO		0xd0    // Analog to Digital, Low Byte
#define	PISO813_AD_HI		0xd4    // Analog to Digital, High Byte
#define PISO813_SET_CH   	0xE0    // channel selecting
#define PISO813_SET_GAIN	0xE4    // PGA gain code
#define PISO813_SOFT_TRIG	0xF0    // A/D trigger control register

/****** define the gain mode ********/
#define PISO813_BI_1		0x00
#define PISO813_BI_2		0x01
#define PISO813_BI_4		0x02
#define PISO813_BI_8		0x03
#define PISO813_BI_16		0x04

#define PISO813_UNI_1		0x00
#define PISO813_UNI_2		0x01
#define PISO813_UNI_4		0x02
#define PISO813_UNI_8		0x03
#define PISO813_UNI_16		0x04

/*************    return code    *************/
#define PISO813_NoError                     0
#define PISO813_DriverOpenError             1
#define PISO813_DriverNoOpen                2
#define PISO813_GetDriverVersionError       3
#define PISO813_CallDriverError             4
#define PISO813_FindBoardError              5
#define PISO813_ExceedBoardNumber           6
#define PISO813_TimeOutError                0xffff
#define PISO813_AdError2                    -100.0


// ID
#define PISO_813                        0x800A00    

// Test functions
EXPORTS float  CALLBACK PISO813_FloatSub(float fA, float fB);
EXPORTS short  CALLBACK PISO813_ShortSub(short nA, short nB);
EXPORTS WORD   CALLBACK PISO813_GetDllVersion(void);

// Driver functions
EXPORTS WORD   CALLBACK PISO813_DriverInit(void);
EXPORTS void   CALLBACK PISO813_DriverClose(void);
EXPORTS WORD   CALLBACK PISO813_SearchCard(WORD *wBoards, DWORD dwPIOCardID);
EXPORTS WORD   CALLBACK PISO813_GetDriverVersion(WORD *wDriverVersion);
EXPORTS WORD   CALLBACK PISO813_GetConfigAddressSpace(WORD  wBoardNo, DWORD *wAddrBase, WORD *wIrqNo,
                           WORD *wSubVendor, WORD *wSubDevice, WORD *wSubAux,
                           WORD *wSlotBus,   WORD *wSlotDevice);
//EXPORTS WORD   CALLBACK PISO813_ActiveBoard( WORD wBoardNo );
//EXPORTS WORD   CALLBACK PISO813_WhichBoardActive(void);

// DIO functions
EXPORTS void   CALLBACK PISO813_OutputWord(DWORD wPortAddress, DWORD wOutData);
EXPORTS void   CALLBACK PISO813_OutputByte(DWORD wPortAddr, WORD bOutputValue);
EXPORTS DWORD  CALLBACK PISO813_InputWord(DWORD wPortAddress);
EXPORTS WORD   CALLBACK PISO813_InputByte(DWORD wPortAddr);

// AD functions
EXPORTS WORD   CALLBACK PISO813_SetChGain(DWORD wAddrBase, WORD wChannel , WORD wGainCode);
EXPORTS WORD   CALLBACK PISO813_AD_Hex(DWORD wAddrBase);
EXPORTS WORD   CALLBACK PISO813_ADs_Hex(DWORD wAddrBase, WORD  *wBuffer, DWORD dwDataNo);
EXPORTS float  CALLBACK PISO813_AD_Float(DWORD wAddrBase, WORD wJump20v, WORD wBipolar);
EXPORTS float  CALLBACK PISO813_ADs_Float(DWORD wAddrBase, WORD wJump20v, WORD wBipolar, float *fBuffer, DWORD dwDataNo);
EXPORTS float  CALLBACK PISO813_AD2F(WORD whex, WORD wGainCode,  WORD wJump20v,  WORD wBipolar);
