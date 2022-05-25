#ifdef __cplusplus
     #define EXPORTS extern "C" __declspec (dllimport)
#else
     #define EXPORTS
#endif

/*** #define WORD int ***/

#define ISODA_NoError           0
#define ISODA_SysNoOpen         1
#define ISODA_DriverNoOpen      1
#define ISODA_ActiveError       2
#define ISODA_TimeOut           3
#define ISODA_SysOpenError      4
#define ISODA_CheckError        5
#define ISODA_BoardError        6
#define ISODA_ChannelError      7
#define ISODA_IrqError          8
#define ISODA_ReadRomError      9
#define ISODA_InstallIrqError  10
#define ISODA_GetIntCountError 11
#define ISODA_IrqResetError    12
#define ISODA_DriverOpenError  13


// Test functions 
EXPORTS DWORD  CALLBACK ISODA_DwordSub2(DWORD dwA, DWORD dwB);
EXPORTS double CALLBACK ISODA_DoubleSub2(double dfA, double dfB);
EXPORTS DWORD  CALLBACK ISODA_GetDllVersion(void);

// Driver functions
EXPORTS DWORD  CALLBACK ISODA_SysOpen(void);
EXPORTS void   CALLBACK ISODA_SysClose(void);
EXPORTS DWORD  CALLBACK ISODA_DriverInit(void);
EXPORTS void   CALLBACK ISODA_DriverClose(void);
EXPORTS DWORD  CALLBACK ISODA_CheckBoard(
        DWORD dwBoard, DWORD dwBase, DWORD dwIrq);
EXPORTS DWORD  CALLBACK ISODA_ActiveBoard(DWORD dwBoard);
EXPORTS DWORD  CALLBACK ISODA_ReadActive(
        DWORD *dwBoard, DWORD *dwBase, DWORD *dwIrq);

// DI/DO functions
EXPORTS DWORD  CALLBACK ISODA_Do(DWORD dwDo);
EXPORTS DWORD  CALLBACK ISODA_Di(DWORD *dwDi);
EXPORTS void   CALLBACK ISODA_OutputByte(WORD wPortAddr, UCHAR bOutputVal);
EXPORTS void   CALLBACK ISODA_OutputWord(WORD wPortAddr, WORD wOutputVal);
EXPORTS DWORD  CALLBACK ISODA_InputByte(WORD wPortAddr);
EXPORTS DWORD  CALLBACK ISODA_InputWord(WORD wPortAddr);

// EEPROM functions
EXPORTS DWORD  CALLBACK ISODA_ReadPowerOnValue(DWORD dwChannel, DWORD *dwValue);
EXPORTS DWORD  CALLBACK ISODA_SetPowerOnValue(DWORD dwChannel, DWORD dwValue);
EXPORTS DWORD  CALLBACK ISODA_ReadRom(DWORD dwAddress, DWORD *dwValue);
EXPORTS DWORD  CALLBACK ISODA_WriteRom(DWORD dwAddress, DWORD dwValue);

// DA functions
EXPORTS DWORD  CALLBACK ISODA_AnalogOutput(DWORD dwChannel, DWORD dwValue);

// Timer functions
EXPORTS DWORD  CALLBACK ISODA_SetTimer(double timeslice);
EXPORTS DWORD  CALLBACK ISODA_ReadTimer(double *timeslice);
EXPORTS DWORD  CALLBACK ISODA_StartTimer(void);
EXPORTS DWORD  CALLBACK ISODA_StopTimer(void);
EXPORTS DWORD  CALLBACK ISODA_DelayMs(DWORD dwMs);

//// ******************** Reserved *******************
//// DA functions
// EXPORTS DWORD  CALLBACK ISODA_Bi10v_DA(DWORD dwChannel,   float fValue);
// EXPORTS DWORD  CALLBACK ISODA_Uni20mA_DA(DWORD dwChannel, float fValue);
//// Interrupt functions
// EXPORTS DWORD  CALLBACK ISODA_Int_Install(
//        WORD wBase, WORD wIrq, HANDLE *hEvent);
// EXPORTS DWORD  CALLBACK ISODA_Int_Remove(HANDLE hEvent);
// EXPORTS DWORD  CALLBACK ISODA_Int_GetCount(DWORD *dwVal);
// EXPORTS DWORD  CALLBACK ISODA_Int_Reset(void);


