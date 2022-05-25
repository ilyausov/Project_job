/************************************************************************/
/* Copyright (c) 1994-1998, ADLINK Technology Inc.  All rights reserved.*/
/*                                                                                                                                              */
/*      File name :     Dll1.H                                          */
/*      Purpose :       Header File of ACLS-DLL1 for W95 & WNT Libraries*/
/*      Date :          07/08/1998                                      */
/*      Version :       3.1                                             */
/*                                                                      */
/************************************************************************/
#ifndef DLL1_NT_H
#define DLL1_NT_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* Constant  Definitions                                                */
/************************************************************************/



/*------- for card type ---------*/
#define                 T_48DIO             0
#define                 T_7120              1
#define                 T_7122              2
#define                 T_7124              3
#define                 T_7125              4
#define                 T_7130              5
#define                 T_7225              6

#ifndef COMMON_DLL1_DLL2
#define COMMON_DLL1_DLL2
/*------- Error code definition for ACL cards ---------*/
enum {
    ERR_NoError,                        // 0
    ERR_BoardNoInit,                    // 1
    ERR_InvalidBoardNumber,             // 2
    ERR_InitializedBoardNumber,         // 3
    ERR_BaseAddressError,               // 4
    ERR_BaseAddressConflict,            // 5
    ERR_DuplicateBoardSetting,          // 6
    ERR_DuplicateIrqSetting,            // 7
    ERR_PortError,                      // 8
    ERR_ChannelError,                   // 9
    ERR_InvalidADChannel,               //10
    ERR_InvalidDAChannel,               //11
    ERR_InvalidDIChannel,               //12
    ERR_InvalidDOChannel,               //13
    ERR_InvalidDIOChannel,              //14
    ERR_InvalidIRQChannel,              //15
    ERR_InvalidDMAChannel,              //16
    ERR_InvalidChangeValue,             //17
    ERR_InvalidTimerValue,              //18
    ERR_InvalidTimerMode,               //19
    ERR_InvalidCounterValue,            //20
    ERR_InvalidCounterMode,             //21
    ERR_InvalidADMode,                  //22
    ERR_InvalidMode,                    //23
    ERR_NotOutputPort,                  //24
    ERR_NotInputPort,                   //25
    ERR_AD_DMANotSet,                   //26
    ERR_AD_INTNotSet,                   //27
    ERR_AD_AquireTimeOut,               //28
    ERR_AD_InvalidGain,                 //29
    ERR_INTNotSet,                      //30
    ERR_InvalidPortNumber,              //31
    ERR_InvalidTrigSrc,                 //32
    ERR_InvalidINTMode,                 //33
    ERR_TotalErrorCount                 //34
};
/*------- for card number -------*/
#define         CARD_MaxCardNo  8
#define         CARD_1          0     /* possible value of card_number */
#define         CARD_2          1
#define         CARD_3          2
#define         CARD_4          3
#define         CARD_5          4
#define         CARD_6          5
#define         CARD_7          6
#define         CARD_8          7

#define         TIMER_MODE0     0     /* Timer : Terminal Count       */
#define         TIMER_MODE1     1     /* Timer : Programmer One-shot  */
#define         TIMER_MODE2     2     /* Timer : Frq.  Generator      */
#define         TIMER_MODE3     3     /* Timer : Square Wave Generator*/
#define         TIMER_MODE4     4     /* Timer : Counter, Soft Trigger*/
#define         TIMER_MODE5     5     /* Timer : Counter, Hard Trigger*/

#endif //COMMON_DLL1_DLL2

/*------- Port for 48DIO card -------*/
#define         PORTA           0     /* port */
#define         PORTB           1
#define         PORTC           2
#define         PORTC_UPPER     3
#define         PORTC_LOWER     4
/*------- for 8 CHANNELs  DI and DO -------*/
#define         DI_BYTE1                0     /* for chan_no of DI */
#define         DI_BYTE2                1     /* for chan_no of DI */
#define         DI_BYTE3                2     /* for chan_no of DI */
#define         DI_BYTE4                3     /* for chan_no of DI */
#define         DO_BYTE1                0     /* for chan_no of DO */
#define         DO_BYTE2                1     /* for chan_no of DO */
#define         DO_BYTE3                2     /* for chan_no of DO */
#define         DO_BYTE4                3     /* for chan_no of DO */
/*------- for PORT  DI and DO -------*/
#define         DI_PORT1                0     /* for port_no of DI */
#define         DI_PORT2                1     /* for port_no of DI */
#define         DO_PORT1                0     /* for port_no of DO */
#define         DO_PORT2                1     /* for port_no of DO */

#define         CN_No1                  0     /* for port_no of DI */
#define         CN_No2                  2     /* for port_no of DI */
/*------- Channel & Port for DI, DO, and IRQ -------*/
/* Only channel 0 and 1 are available for 48DIO card */
/* Only channel 0 is available for 7124 card */
#define         CH0_PA         (0<<8)+PORTA
#define         CH0_PB         (0<<8)+PORTB
#define         CH0_PC         (0<<8)+PORTC
#define         CH0_PCU        (0<<8)+PORTC_UPPER
#define         CH0_PCL        (0<<8)+PORTC_LOWER
#define         CH1_PA         (1<<8)+PORTA
#define         CH1_PB         (1<<8)+PORTB
#define         CH1_PC         (1<<8)+PORTC
#define         CH1_PCU        (1<<8)+PORTC_UPPER
#define         CH1_PCL        (1<<8)+PORTC_LOWER
/* The following channel&port constants are for 7122*/
#define         CH2_PA         (2<<8)+PORTA
#define         CH2_PB         (2<<8)+PORTB
#define         CH2_PC         (2<<8)+PORTC
#define         CH2_PCU        (2<<8)+PORTC_UPPER
#define         CH2_PCL        (2<<8)+PORTC_LOWER
#define         CH3_PA         (3<<8)+PORTA
#define         CH3_PB         (3<<8)+PORTB
#define         CH3_PC         (3<<8)+PORTC
#define         CH3_PCU        (3<<8)+PORTC_UPPER
#define         CH3_PCL        (3<<8)+PORTC_LOWER
#define         CH4_PA         (4<<8)+PORTA
#define         CH4_PB         (4<<8)+PORTB
#define         CH4_PC         (4<<8)+PORTC
#define         CH4_PCU        (4<<8)+PORTC_UPPER
#define         CH4_PCL        (4<<8)+PORTC_LOWER
#define         CH5_PA         (5<<8)+PORTA
#define         CH5_PB         (5<<8)+PORTB
#define         CH5_PC         (5<<8)+PORTC
#define         CH5_PCU        (5<<8)+PORTC_UPPER
#define         CH5_PCL        (5<<8)+PORTC_LOWER
/*------- IRQ Operation -------*/
#define         NO_OP           0
#define         INPUT_OP        1
#define         OUTPUT_OP       2
/*------- INT status -------*/
#define         INT_STOP        0     /* for INT status */
#define         INT_RUN         1
//for 720
#define         W_720_Initial       W_7120_Initial
#define         W_720_Set_Card      W_7120_Set_Card
#define         W_720_Get_Card      W_7120_Get_Card
#define         W_720_DI_8          W_7120_DI_8
#define         W_720_DI_Channel    W_7120_DI_Channel
#define         W_720_DI_16         W_7120_DI_16
#define         W_720_DO_8          W_7120_DO_8
#define         W_720_DO_Channel    W_7120_DO_Channel
#define         W_720_DO_16         W_7120_DO_16
#define         W_720_Read_Back     W_7120_Read_Back
#define         W_720_Timer_Start   W_7120_Timer_Start
#define         W_720_Timer_Read    W_7120_Timer_Read
#define		W_720_Timer_Stop    W_7120_Timer_Stop
//for 722
#define W_722_Initial           W_7122_Initial
#define W_722_Set_Card          W_7122_Set_Card
#define W_722_Get_Card          W_7122_Get_Card
#define W_722_DI                W_7122_DI
#define W_722_DO                W_7122_DO
#define W_722_Read_Back         W_7122_Read_Back
#define W_722_INT_Start         W_7122_INT_Start
#define W_722_INT_Status        W_7122_INT_Status
#define W_722_INT_Stop          W_7122_INT_Stop
#define W_722_INT_Op            W_7122_INT_Op
#define W_722_INT_Reset         W_7122_INT_Reset
//for 724
#define W_724_Initial           W_7124_Initial
#define W_724_Set_Card          W_7124_Set_Card
#define W_724_Get_Card          W_7124_Get_Card
#define W_724_DI                W_7124_DI
#define W_724_DO                W_7124_DO
#define W_724_Read_Back         W_7124_Read_Back
#define W_724_INT_Start         W_7124_INT_Start
#define W_724_INT_Status        W_7124_INT_Status
#define W_724_INT_Stop          W_7124_INT_Stop
#define W_724_INT_Op            W_7124_INT_Op
#define W_724_INT_Reset         W_7124_INT_Reset
//for 725
#define W_725_Initial           W_7125_Initial
#define W_725_Set_Card          W_7125_Set_Card
#define W_725_Get_Card          W_7125_Get_Card
#define W_725_DI                W_7125_DI
#define W_725_DI_Channel        W_7125_DI_Channel
#define W_725_DO                W_7125_DO
#define W_725_DO_Channel        W_7125_DO_Channel
#define W_725_Read_Back         W_7125_Read_Back
//for 725B
#define W_725B_Initial          W_7225_Initial
#define W_725B_Set_Card         W_7225_Set_Card
#define W_725B_Get_Card         W_7225_Get_Card
#define W_725B_DI               W_7225_DI
#define W_725B_DI_Channel       W_7225_DI_Channel
#define W_725B_DO               W_7225_DO
#define W_725B_DO_Channel       W_7225_DO_Channel
#define W_725B_Read_Back        W_7225_Read_Back

/************************************************************************/
/* Functions Declarations                                               */
/************************************************************************/

/* for 48DIO */
int PASCAL W_48DIO_Initial(int card_number, int base_address, int irq);
int PASCAL W_48DIO_Set_Card(int card_number);
int PASCAL W_48DIO_Get_Card(int *card_number);
int PASCAL W_48DIO_INT_Timer_Start( int c1, int c2);
int PASCAL W_48DIO_INT_Timer_Stop();
int PASCAL W_48DIO_DI(int cn_port, unsigned char *di_data);
int PASCAL W_48DIO_DO(int cn_port, unsigned char do_data);
int PASCAL W_48DIO_INTOP_Start(int count);
int PASCAL W_48DIO_INTOP_Status(int *status, int *count);
int PASCAL W_48DIO_INTOP_Stop(int *count);
int PASCAL W_48DIO_INT_Enable(HANDLE *hEvent);
int PASCAL W_48DIO_INT_Disable();
int PASCAL W_48DIO_INT_Op(int cn_port, int op, unsigned int *buf, int buf_cnt);
int PASCAL W_48DIO_INT_Reset();
int PASCAL W_48DIO_Count_Start(int timer_mode, unsigned int c0);
int PASCAL W_48DIO_Count_Status(unsigned int *counter_value);
int PASCAL W_48DIO_Count_Stop( unsigned int  *counter_value);

/* for 7120 */
int PASCAL W_7120_Initial(int card_number, int base_address, int irq);
int PASCAL W_7120_Set_Card(int card_number);
int PASCAL W_7120_Get_Card(int *card_number);
int PASCAL W_7120_DI_8( int byte_no, unsigned char *di_data);
int PASCAL W_7120_DI_Channel( int channel_no, unsigned int *di_data);
int PASCAL W_7120_DI_16(int port, unsigned int *di_data);
int PASCAL W_7120_DO_8( int byte_no, unsigned char do_data);
int PASCAL W_7120_DO_16( int port, unsigned int do_data);
int PASCAL W_7120_Timer_Stop( int timer_no, long far *cnt_value);
int PASCAL W_7120_Timer_Read( int timer_no, long far *counter_value);
int PASCAL W_7120_Timer_Start( int timer_no, int timer_mode , long c);
int PASCAL W_7120_INT_Timer_Start( unsigned int c1, unsigned int c2);
int PASCAL W_7120_INT_Timer_Stop();
int PASCAL W_7120_INT_Enable(HANDLE *hEvent);
int PASCAL W_7120_INT_Disable();

/* for 7130*/
int PASCAL W_7130_Initial(int card_number, int base_address, int irq1, int irq2);
int PASCAL W_7130_Set_Card(int card_number);
int PASCAL W_7130_Get_Card(int *card_number);
int PASCAL W_7130_DI_8(int byte_no, unsigned char *di_data);
int PASCAL W_7130_DI_Channel(int channel_no, unsigned int *di_data);
int PASCAL W_7130_DI_16(int port, unsigned int *di_data);
int PASCAL W_7130_DO_8(int byte_no, unsigned char do_data);
int PASCAL W_7130_DO_16(int port, unsigned int do_data);
int PASCAL W_7130_Timer_Stop(int timer_no, long far *cnt_value);
int PASCAL W_7130_Timer_Read(int timer_no, long far *counter_value);
int PASCAL W_7130_Timer_Start(int timer_no, int timer_mode , long c);
int PASCAL W_7130_INT_Timer_Start(unsigned int c1, unsigned int c2);
int PASCAL W_7130_INT_Timer_Stop();
int PASCAL W_7130_INT_Enable(int irq_no, HANDLE *hEvent);
int PASCAL W_7130_INT_Disable(int irq_no);

/* for 7122 */
int PASCAL  W_7122_Initial(int card_number, int base_address, int irq);
int PASCAL  W_7122_Set_Card(int card_number);
int PASCAL  W_7122_Get_Card(int *card_number);
int PASCAL  W_7122_DI(int cn_port, unsigned char *di_data);
int PASCAL  W_7122_DO(int cn_port, unsigned char do_data);
int PASCAL  W_7122_INTOP_Start(int count);
int PASCAL  W_7122_INTOP_Status(int *status, int  *count);
int PASCAL  W_7122_INTOP_Stop(int *count);
int PASCAL  W_7122_INT_Op(int cn_port, int op, unsigned int *buf, int buf_cnt);
int PASCAL  W_7122_INT_Reset();
int PASCAL  W_7122_INT_Enable(HANDLE *hEvent);
int PASCAL  W_7122_INT_Disable();

/* for 7124 */
int PASCAL  W_7124_Initial(int card_number, int base_address, int irq);
int PASCAL  W_7124_Set_Card(int card_number);
int PASCAL  W_7124_Get_Card(int *card_number);
int PASCAL  W_7124_DI( int cn_port, unsigned char *di_data);
int PASCAL  W_7124_DO( int cn_port, unsigned char do_data);
int PASCAL  W_7124_INTOP_Start(int count);
int PASCAL  W_7124_INTOP_Status(int *status, int *count);
int PASCAL  W_7124_INTOP_Stop(int *count);
int PASCAL  W_7124_INT_Op(int cn_port, int op, unsigned int *buf, int buf_cnt);
int PASCAL  W_7124_INT_Reset();
int PASCAL  W_7124_INT_Enable(HANDLE *hEvent);
int PASCAL  W_7124_INT_Disable();

/* for 7125 */
int PASCAL  W_7125_Initial(int card_number, int base_address);
int PASCAL  W_7125_Set_Card(int card_number);
int PASCAL  W_7125_Get_Card(int *card_number);
int PASCAL  W_7125_DI(unsigned char *di_data);
int PASCAL  W_7125_DO(unsigned char do_data);
int PASCAL  W_7125_Read_Back(unsigned char *do_data);
int PASCAL  W_7125_DI_Channel(int channel_no, unsigned int *di_data);
int PASCAL  W_7125_DO_Channel(int channel_no, unsigned char do_data);

/* for 7225 */
int PASCAL   W_7225_Initial(int card_number, int base_address);
int PASCAL   W_7225_Set_Card(int card_number);
int PASCAL   W_7225_Get_Card(int *card_number);
int PASCAL   W_7225_DI(unsigned int *di_data);
int PASCAL   W_7225_DO(unsigned int do_data);
int PASCAL   W_7225_Read_Back(long *do_data);
int PASCAL   W_7225_DI_Channel(int channel_no, unsigned int *di_data);
int PASCAL   W_7225_DO_Channel(int channel_no, unsigned char do_data);

#ifdef __cplusplus
}
#endif

#endif //DLL1_NT_H