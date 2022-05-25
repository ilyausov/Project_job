#ifndef ACL7225_H
#define ACL7225_H
/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Copyright (c) 1994-1995, ADLINK Technology Inc.  All rights reserved.  ;;
;;									   ;;
;;	File Name   :	7225.H						   ;;
;;	Purpose     :	Header File of ACL-7225 Library 		   ;;
;;	Date	    :	8/8/1995					   ;;
;;	Revision    :	1.1						   ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;			Constant Definitions				   ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
/*------------- Card Number -------------*/
#define 	CARD_MaxCardNo	   8
#define 	CARD_1		       0  /* possible value of card_number */
#define 	CARD_2		       1
#define 	CARD_3		       2
#define 	CARD_4		       3
#define 	CARD_5		       4
#define 	CARD_6		       5
#define 	CARD_7		       6
#define 	CARD_8		       7
/*------------- 16 Channel DI and DO ---------*/
#define 	DO_BYTE0		0	/* DO bit 0 - 7      */
#define 	DO_BYTE1		1	/* DO bit 8 - 15     */
#define 	DI_BYTE0		2	/* DI bit 0 - 7      */
#define 	DI_BYTE1		3	/* DI bit 8 - 15     */

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;			Function Definitions				   ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
#define _725B_Initial	     _7225_Initial
#define _725B_Set_Card	     _7225_Set_Card
#define _725B_Get_Card	     _7225_Get_Card
#define _725B_DI_16	         _7225_DI_16
#define _725B_DI_Channel     _7225_DI_Channel
#define _725B_DO_16	         _7225_DO_16
#define _725B_DO_Channel     _7225_DO_Channel
#define _725B_Read_Back      _7225_Read_Back

extern int _7225_Initial(int cardNo, int baseAddress);
extern int _7225_Set_Card(int cardNo);
extern int _7225_Get_Card(int *cardNo);
extern int _7225_DI(unsigned short *data);
extern int _7225_DI_Channel(int channelNo, unsigned char *data);
extern int _7225_DO(unsigned short data);
extern int _7225_DO_Channel(int channelNo , unsigned char data);
extern int _7225_Read_Back(unsigned short *data );

#endif	/* end of ifndef ACL7225_H  */
