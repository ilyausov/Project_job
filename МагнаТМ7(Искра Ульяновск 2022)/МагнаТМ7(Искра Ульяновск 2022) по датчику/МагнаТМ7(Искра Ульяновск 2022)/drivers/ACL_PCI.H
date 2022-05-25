#ifndef ACL_PCI_H
    #define ACL_PCI_H
/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Copyright (c) 1995-1997, ADLink Technology Inc.  All rights reserved.  ;;
;;                                                                         ;;
;;      File Name   :   ACL_PCI.H                                          ;;
;;      Purpose     :   Header file for NuDAQ PCI cards                    ;;
;;      Date        :   07/28/1997                                         ;;
;;      Revision    :   1.05                                               ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/*      Typedef  Definitions                                                */
/****************************************************************************/

typedef unsigned char   U8;
typedef short           I16;
typedef unsigned short  U16;
typedef long            I32;
typedef unsigned long   U32;
typedef float           F32;
typedef double          F64;
typedef char            Boolean;

enum {
    PCI_CARD1,
    PCI_CARD2,
    PCI_CARD3,
    PCI_CARD4,
    PCI_CARD5,
    PCI_CARD6,
    PCI_CARD7,
    PCI_CARD8,
    PCI_CARD9,
    PCI_CARD10,
    PCI_CARD11,
    PCI_CARD12,
    MAX_PCI_CARDS
};

typedef struct {
    U16 baseAddr[MAX_PCI_CARDS];
    U16 irqNo[MAX_PCI_CARDS];
} PCI_INFO;


/****************************************************************************/
/*      Constant Definitions                                                */
/****************************************************************************/

/*;;;;;;;;;;;;;;;;;;;;;;;;;  for PCI-7250  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
#define         DO_PORT0       0x00 /* For PCI-7250 Board              */
#define         DO_PORT1       0x01 /* For PCI-7251 Expansion Board #1 */
#define         DO_PORT2       0x02 /* For PCI-7251 Expansion Board #2 */
#define         DO_PORT3       0x03 /* For PCI-7251 Expansion Board #3 */

#define         DI_PORT0       0x00 /* For PCI-7250 Board              */
#define         DI_PORT1       0x01 /* For PCI-7251 Expansion Board #1 */
#define         DI_PORT2       0x02 /* For PCI-7251 Expansion Board #2 */
#define         DI_PORT3       0x03 /* For PCI-7251 Expansion Board #3 */

#define         PCI_7251_EX1   0x01 /* For PCI-7251 Board #1           */
#define         PCI_7251_EX2   0x02 /* For PCI-7251 Board #2           */
#define         PCI_7251_EX3   0x03 /* For PCI-7251 Board #3           */

#define         PCI_7251_EXIST      1
#define         PCI_7251_NOT_EXIST  0


/*;;;;;;;;;;;;;;;;;;;;;;;;;  for PCI-7248, PCI-7296 ;;;;;;;;;;;;;;;;;;;;;;;;*/
/*----- Parameters used for Interrupt Control ------------------------------*/
#define         INT1_OFF        0x00
#define         INT1_P1C0       0x01
#define         INT1_P1C3C0     0x02
#define         INT1_EVENT_IRQ  0x03

#define         INT2_OFF        0x00
#define         INT2_P2C0       0x04
#define         INT2_P2C3C0     0x08
#define         INT2_TIMER_IRQ  0x0C

/*----- Parameters used for Event Counter Edge Control ---------------------*/
#define         EDGE_L2H        0x00
#define         EDGE_H2L        0x01

/*---------------------------------------------- for Mode of Timer #0-------*/
#define         TIMER_MODE0     0       /* Timer : Terminal Count       */
#define         TIMER_MODE1     1       /* Timer : Programmer One-shot  */
#define         TIMER_MODE2     2       /* Timer : Frq.  Generator      */
#define         TIMER_MODE3     3       /* Timer : Square Wave Generator*/
#define         TIMER_MODE4     4       /* Timer : Counter, Soft Trigger*/
#define         TIMER_MODE5     5       /* Timer : Counter, Hard Trigger*/

//DIO Port Direction
#define         INPUT_PORT      1
#define         OUTPUT_PORT     2

/*----- PCI-7248, PCI-7296 Channel Number Definition ----------------------*/
#define         PCI_CH0         0x00 /* Channel 0                          */
#define         PCI_CH1         0x01 /* Channel 1                          */
#define         PCI_CH2         0x02 /* Channel 2   only PCI-7296          */
#define         PCI_CH3         0x03 /* Channel 3   only PCI-7296          */

/*  PORT direction definition           PORT_A   PORT_CU   PORT_B  PORT_CL */
#define         PORT_OOOO       0x00 /*  OUT       OUT      OUT      OUT   */
#define         PORT_OOOI       0x01 /*  OUT       OUT      OUT      IN    */
#define         PORT_OOIO       0x02 /*  OUT       OUT      IN       OUT   */
#define         PORT_OOII       0x03 /*  OUT       OUT      IN       IN    */
#define         PORT_OIOO       0x04 /*  OUT       IN       OUT      OUT   */
#define         PORT_OIOI       0x05 /*  OUT       IN       OUT      IN    */
#define         PORT_OIIO       0x06 /*  OUT       IN       IN       OUT   */
#define         PORT_OIII       0x07 /*  OUT       IN       IN       IN    */

#define         PORT_IOOO       0x08 /*  IN        OUT      OUT      OUT   */
#define         PORT_IOOI       0x09 /*  IN        OUT      OUT      IN    */
#define         PORT_IOIO       0x0A /*  IN        OUT      IN       OUT   */
#define         PORT_IOII       0x0B /*  IN        OUT      IN       IN    */
#define         PORT_IIOO       0x0C /*  IN        IN       OUT      OUT   */
#define         PORT_IIOI       0x0D /*  IN        IN       OUT      IN    */
#define         PORT_IIIO       0x0E /*  IN        IN       IN       OUT   */
#define         PORT_IIII       0x0F /*  IN        IN       IN       IN    */


/*---- Channel PORT  definition -------------------------------------------*/
#define         PCI_PORTA       0x00 /* PORT-A                  */
#define         PCI_PORTB       0x01 /* PORT-A                  */
#define         PCI_PORTC       0x02 /* PORT-A                  */
#define         PCI_PORTCU      0x03 /* PORT-A                  */
#define         PCI_PORTCL      0x04 /* PORT-A                  */


#define         PCI_CH0_PA      0x00 /* Channel 0, PORT-A       */
#define         PCI_CH0_PB      0x01 /* Channel 0, PORT-B       */
#define         PCI_CH0_PC      0x02 /* Channel 0, PORT-C       */
#define         PCI_CH0_PCU     0x03 /* Channel 0, PORT-C Upper */
#define         PCI_CH0_PCL     0x04 /* Channel 0, PORT-A Lower */

#define         PCI_CH1_PA      0x05 /* Channel 1, PORT-A       */
#define         PCI_CH1_PB      0x06 /* Channel 1, PORT-B       */
#define         PCI_CH1_PC      0x07 /* Channel 1, PORT-C       */
#define         PCI_CH1_PCU     0x08 /* Channel 1, PORT-C Upper */
#define         PCI_CH1_PCL     0x09 /* Channel 1, PORT-A Lower */

#define         PCI_CH2_PA      0x0A /* Channel 2, PORT-A       */
#define         PCI_CH2_PB      0x0B /* Channel 2, PORT-B       */
#define         PCI_CH2_PC      0x0C /* Channel 2, PORT-C       */
#define         PCI_CH2_PCU     0x0D /* Channel 2, PORT-C Upper */
#define         PCI_CH2_PCL     0x0E /* Channel 2, PORT-A Lower */

#define         PCI_CH3_PA      0x0F /* Channel 3, PORT-A       */
#define         PCI_CH3_PB      0x10 /* Channel 3, PORT-B       */
#define         PCI_CH3_PC      0x11 /* Channel 3, PORT-C       */
#define         PCI_CH3_PCU     0x12 /* Channel 3, PORT-C Upper */
#define         PCI_CH3_PCL     0x13 /* Channel 3, PORT-A Lower */


/****************************************************************************/
/*      Functions Declerations                                              */
/****************************************************************************/

extern U16  _7250_Initial(U16 *existCards, PCI_INFO *pciInfo);
extern U16  _7250_DO(U16 cardNo, U16 doChannelNo, U16 doData);
extern U16  _7250_DO_Read_Back(U16 cardNo, U16 doChannelNo,U16 *doReadBackData);
extern U16  _7250_DI(U16 cardNo, U16 diChannelNo, U16 *diData);
extern U16  _7251_Check_Exist(U16 cardNo, U16 extensionBoardNo);

extern void _7250_Set_INT_Control(U16 cardNo, U16 int1Flag, U16 int2Flag);
extern void _7250_Get_IRQ_Status(U16 cardNo, U16 *int1Status, U16 *int2Status);
extern void _7250_CLR_IRQ(U16 cardNo);
extern void _7250_Software_Reset(U16 cardNo);
/*---------------------------------------------------------------------------*/

extern U16  _7248_Initial(U16 *existCards, PCI_INFO *pciInfo);
extern U16  _7248_DI(U16 cardNo, U16 channelPort, U16 *inData);
extern U16  _7248_DO(U16 cardNo, U16 channelPort, U16 outData);
extern U16  _7248_Config_Channel(U16 cardNo, U16 channelNo, U16 ctrlValue);
extern U16  _7248_Config_Port(U16 cardNo, U16 channelPort, U16 direction);
extern void _7248_Set_INT_Control(U16 cardNo, U16 ctrlValue);
extern void _7248_Timer_Start(U16 cardNo, U16 timer0Mode, U16 c0);
extern void _7248_Timer_Read(U16 cardNo, U16 *counterValue);
extern void _7248_Timer_Stop(U16 cardNo, U16 *counterValue);
extern void _7248_Cascaded_Timer(U16 cardNo, U16 c1, U16 c2);
extern void _7248_Get_IRQ_Status(U16 cardNo, U16 *int1Status, U16 *int2Status);
extern void _7248_CLR_IRQ(U16 cardNo);
extern void _7248_Software_Reset(U16 cardNo);
/*---------------------------------------------------------------------------*/

extern U16  _7296_Initial(U16 *existCards, PCI_INFO *pciInfo);
extern U16  _7296_DI(U16 cardNo, U16 channelPort, U16 *inData);
extern U16  _7296_DO(U16 cardNo, U16 channelPort, U16 outData);
extern U16  _7296_Config_Channel(U16 cardNo, U16 channelNo, U16 ctrlValue);
extern U16  _7296_Config_Port(U16 cardNo, U16 channelPort, U16 direction);
extern void _7296_Set_INT_Control(U16 cardNo, U16 ctrlValue);
extern void _7296_Timer_Start(U16 cardNo, U16 timer0Mode, U16 c0);
extern void _7296_Timer_Read(U16 cardNo, U16 *counterValue);
extern void _7296_Timer_Stop(U16 cardNo, U16 *counterValue);
extern void _7296_Cascaded_Timer(U16 cardNo, U16 c1, U16 c2);
extern void _7296_Get_IRQ_Status(U16 cardNo, U16 *int1Status, U16 *int2Status);
extern void _7296_CLR_IRQ(U16 cardNo);
extern void _7296_Software_Reset(U16 cardNo);
/*---------------------------------------------------------------------------*/

extern U16  _7230_Initial(U16 *existCards, PCI_INFO *pciInfo);
extern U16  _7230_DO( U16 cardNo, U16  doData);
extern U16  _7230_DI( U16 cardNo, U16 *diData);

extern void _7230_Set_INT_Control(U16 cardNo, U16 int1Flag, U16 int2Flag);
extern void _7230_Set_INT_Polarity( U16 cardNo, U16 int1pol, U16 int2pol );
extern void _7230_Get_IRQ_Status(U16 cardNo,U16 *int1Status, U16 *int2Status);
extern void _7230_Get_IRQ_Channel(U16 cardNo, U16 *irq_no );
extern void _7230_CLR_IRQ( U16 cardNo );
extern void _7230_Software_Reset(U16 cardNo);
/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif
