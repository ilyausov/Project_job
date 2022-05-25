#ifdef __cplusplus
     #define EXPORTS extern "C" __declspec (dllimport)
#else
     #define EXPORTS
#endif

/****** define the gain mode ********/
#define ISO813_BI_1			0x00
#define ISO813_BI_2			0x01
#define ISO813_BI_4			0x02
#define ISO813_BI_8			0x03
#define ISO813_BI_16		0x04

#define ISO813_UNI_1		0x00
#define ISO813_UNI_2		0x01
#define ISO813_UNI_4		0x02
#define ISO813_UNI_8		0x03
#define ISO813_UNI_16		0x04

/****** define the error number *******/
#define ISO813_NoError 					0
#define ISO813_CheckBoardError			1
#define ISO813_DriverOpenError			2
#define ISO813_DriverNoOpen				3
#define ISO813_AdError					4
#define ISO813_AdError2					-100.0
#define ISO813_OtherError				5
#define ISO813_GetDriverVersionError	6
#define ISO813_TimeOutError				0xffff


/***************************************************/
/**************** Function of Test *****************/
EXPORTS short CALLBACK ISO813_SHORT_SUB_2(short nA, short nB);
EXPORTS float CALLBACK ISO813_FLOAT_SUB_2(float fA, float fB);
EXPORTS WORD  CALLBACK ISO813_Get_DLL_Version(void);
EXPORTS WORD  CALLBACK ISO813_GetDriverVersion(WORD *wDriverVersion);

/*************** Function of Driver *****************/
EXPORTS WORD  CALLBACK ISO813_DriverInit(void);
EXPORTS void  CALLBACK ISO813_DriverClose(void);
EXPORTS WORD  CALLBACK ISO813_Check_Address(WORD wBase);

/***************** Function of AD *******************/
EXPORTS WORD  CALLBACK ISO813_AD_Hex(   WORD  wBase,     WORD  wChannel, WORD wGainCode );
EXPORTS WORD  CALLBACK ISO813_ADs_Hex(  WORD  wBase,     WORD  wChannel, WORD wGainCode,
									    WORD  *wBuffer,  DWORD dwDataNo  );
EXPORTS float CALLBACK ISO813_AD_Float( WORD  wBase,     WORD  wChannel, WORD wGainCode, 
					                    WORD  wBipolar,  WORD  wJump10v  );
EXPORTS WORD  CALLBACK ISO813_ADs_Float(WORD  wBase,     WORD  wChannel, WORD wGainCode, 
					                    WORD  wBipolar,  WORD  wJump10v,
					                    float *fBuffer,  DWORD dwDataNo  ); 
EXPORTS float CALLBACK ISO813_AD2F(WORD whex, WORD wGainCode, WORD wBipolar,  WORD wJump10v);
EXPORTS void  CALLBACK ISO813_AD_SetReadyTicks(WORD wTicks);

/********** Declare 8253 Timer Interface ************/
EXPORTS WORD  CALLBACK ISO813_TimerRead(WORD *wTicks);
EXPORTS void  CALLBACK ISO813_TimerDelay(DWORD dwTicks);

/***************** Function of DIO*******************/
EXPORTS void  CALLBACK ISO813_OutputByte(WORD wPortAddr, UCHAR bOutputVal);
EXPORTS void  CALLBACK ISO813_OutputWord(WORD wPortAddr, WORD wOutputVal);
EXPORTS WORD  CALLBACK ISO813_InputByte(WORD wPortAddr);
EXPORTS WORD  CALLBACK ISO813_InputWord(WORD wPortAddr);


