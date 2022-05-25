#include <vcl.h>
#include "drivers\ISO.h"		// ISO-3232
#include "drivers\ISO813.h"		// ISO-813
#include "drivers\ISODA.h"		// ISO-DA16
#include "drivers\Dll1.H"		// ACL-7250
#include "drivers\Dask.h"		// ACL-7250
#include "drivers\PisoDIO.h"    // PISO/PEX-P32C32/P16R16
#include "drivers\Piso813.h"    // PISO-813
#include "External.h"
//---------------------------------------------------------------------------
//--Файл описания интерфейсов внешних устройств--//
//---------------------------------------------------------------------------
// необъектные функции
int OpenISO_P32C32 (); // попытка связаться с драйвером ISO-P32C32
int OpenACL_7225_1 ();   // попытка связаться с драйвером ACL-7225
int OpenACL_7225_2 ();   // попытка связаться с драйвером ACL-7225
int OpenISO_813 ();    // попытка связаться с драйвером ISO-813
int OpenISO_DA16();    // попытка связаться с драйвером ISO-DA16
int OpenACL_7250 ();   // попытка связаться с драйвером ACL-7250
int OpenPISO_DIO(); // инициализация драйвера PISO-P32C32/P16R16
int OpenPISO_813(); // попытка связаться с драйвером PISO-813
//---------------------------------------------------------------------------
// чтение/запись дискретных сигналов в ACL-7225
unsigned int ACL7225_1 ( unsigned int aclState , unsigned int* value );
unsigned int ACL7225_2 ( unsigned int aclState , unsigned int* value );
// чтение/запись дискретных сигналов в P32C32
unsigned int ISO_P32C32_1 ( unsigned int isoState , unsigned int* value );
unsigned int ISO_P32C32_2 ( unsigned int isoState , unsigned int* value );
// чтение аналоговых сигналов с 813
unsigned int ISO_813 ( unsigned int* value , char valueCount );
// запись аналоговых сигналов в DA16 и чтение уставок
unsigned int ISO_DA16 ( unsigned int isoState, unsigned int value, unsigned int* valueKon, int signNmb );
// чтение/запись дискретных сигналов в ACL-7250
unsigned int ACL7250 ( unsigned int aclState , unsigned int* value );
// чтение/запись дискретных сигналов в PISO-P32C32U
unsigned int PISO_P32C32U_1( unsigned int pisoState , unsigned int* value );
unsigned int PISO_P32C32U_2( unsigned int pisoState , unsigned int* value );
// чтение/запись дискретных сигналов в PEX-P16R16
unsigned int PISO_P16R16U_1( unsigned int pisoState , unsigned int* value );
unsigned int PISO_P16R16U_2( unsigned int pisoState , unsigned int* value );
// чтение сигналов PISO-813
unsigned int PISO_813U( unsigned int pisoState , unsigned int* value );
//---------------------------------------------------------------------------
//--Связаться с драйвером ISO-DA16--//
//---------------------------------------------------------------------------
int OpenISO_DA16 ()
{
    ISODA_DriverInit();
    return isoDA16err = ISODA_CheckBoard( 0 , base_DA16 , 0 );
}
//---------------------------------------------------------------------------
//--Связаться с драйвером ISO-P32C32--//
//---------------------------------------------------------------------------
int OpenISO_P32C32 ()
{
    isoP32C32err1 = ISO_DriverInit();
	return isoP32C32err2 = isoP32C32err1;
}
//---------------------------------------------------------------------------
//--Связаться с драйвером ACL-7225--//
//---------------------------------------------------------------------------
int OpenACL_7225_1 ()
{
    return iso7225err1 = W_7225_Initial( CARD_1 , base_7225_1 );
}
int OpenACL_7225_2 ()
{
    return iso7225err2 = W_7225_Initial( CARD_2 , base_7225_2 );
}
//---------------------------------------------------------------------------
//--Связаться с драйвером ISO-813--//
//---------------------------------------------------------------------------
int OpenISO_813 ()
{
    return iso813err = ISO813_DriverInit();
}
//---------------------------------------------------------------------------
//--Связаться с драйвером ACL-7250--//
//---------------------------------------------------------------------------
int OpenACL_7250 ()
{
    return iso7250err = Register_Card(PCI_7250, 0);
}
//---------------------------------------------------------------------------
//--Связаться с драйвером PISO-P32C32U--//
//---------------------------------------------------------------------------
int OpenPISO_DIO()
{
    PISODIO_DriverInit();
    // поиск плат PISO/PEX-P32C32
    PISODIO_SearchCard(&wTotalBoards,PISO_P32C32);
    //ShowMessage("Количество плат: " + IntToStr(wTotalBoards));
    if(wTotalBoards > 0)
    {
        //ShowMessage("Адрес 1: " + IntToStr(wBaseAddr));  // 0xE000
        PISODIO_GetConfigAddressSpace(Word(StrToInt(0)), &base_PISO_P32C32_1,&wIrqNo, &wSubVendor, &wSubDevice, &wSubAux, &wSlotBus, &wSlotDevice );
        PISODIO_OutputByte(base_PISO_P32C32_1,1);
    }
    if(wTotalBoards > 1)
    {
        //ShowMessage("Адрес 2: " + IntToStr(wBaseAddr)); // 0xD000
        PISODIO_GetConfigAddressSpace(Word(StrToInt(1)), &base_PISO_P32C32_2,&wIrqNo, &wSubVendor, &wSubDevice, &wSubAux, &wSlotBus, &wSlotDevice );
        PISODIO_OutputByte(base_PISO_P32C32_2,1);
    }
    // поиск плат PISO/PEX-P16R16
    PISODIO_SearchCard(&wTotalBoards,PISO_P16R16U);
    //ShowMessage("Количество плат: " + IntToStr(wTotalBoards));
    if(wTotalBoards > 0)
    {
        //ShowMessage("Адрес 1: " + IntToStr(wBaseAddr));  // 0xE000
        PISODIO_GetConfigAddressSpace(Word(StrToInt(0)), &base_PISO_P16R16_1,&wIrqNo, &wSubVendor, &wSubDevice, &wSubAux, &wSlotBus, &wSlotDevice );
        PISODIO_OutputByte(base_PISO_P16R16_1,1);
    }
    if(wTotalBoards > 1)
    {
        //ShowMessage("Адрес 2: " + IntToStr(wBaseAddr)); // 0xD000
        PISODIO_GetConfigAddressSpace(Word(StrToInt(1)), &base_PISO_P16R16_2,&wIrqNo, &wSubVendor, &wSubDevice, &wSubAux, &wSlotBus, &wSlotDevice );
        PISODIO_OutputByte(base_PISO_P16R16_2,1);
    }
}
//---------------------------------------------------------------------------
//--Связаться с драйвером PISO-813U--//
//---------------------------------------------------------------------------
int OpenPISO_813()
{
     PISO813_DriverInit();
     PISO813_SearchCard(&wTotalBoards,PISO_813);
     if(wTotalBoards > 0)
     {
        PISO813_GetConfigAddressSpace( Word(StrToInt(0)), &base_PISO_813,&wIrqNo, &wSubVendor, &wSubDevice, &wSubAux, &wSlotBus, &wSlotDevice);
        PISO813_OutputByte(base_PISO_813, 1);
     }
}
//---------------------------------------------------------------------------
//--ACL7225--//
//---------------------------------------------------------------------------
unsigned int ACL7225_1 ( unsigned int aclState , unsigned int* value )
{
    // если плата неработоспособна
    if ( iso7225err1 != ERR_NoError )
    {
        // пробуем снова ее открыть
        return OpenACL_7225_1();
    }
    // если ошибок нет, считываем данные
    else
    {
        W_7225_Set_Card(CARD_1);
        switch ( aclState )
        {
            // чтение дискретных сигналов
            case 0:
            {
                iso7225err1 = W_7225_DI ( &value[5] );
            }; break;
            // запись дискретных сигналов
            case 1:
            {
                iso7225err1 = W_7225_DO ( value[5] );
            }; break;
        }
    }
    // возвращаем код ошибки
    return iso7225err1;
}
//---------------------------------------------------------------------------
unsigned int ACL7225_2 ( unsigned int aclState , unsigned int* value )
{
    // если плата неработоспособна
    if ( iso7225err2 != ERR_NoError )
    {
        // пробуем снова ее открыть
        return OpenACL_7225_2();
    }
    // если ошибок нет, считываем данные
    else
    {
        W_7225_Set_Card(CARD_2);
        switch ( aclState )
        {
            // чтение дискретных сигналов
            case 0:
            {
                iso7225err2 = W_7225_DI ( &value[3] );
            }; break;
            // запись дискретных сигналов
            case 1:
            {
                iso7225err2 = W_7225_DO ( value[3] );
            }; break;
        }
    }
    // возвращаем код ошибки
    return iso7225err2;
}
//---------------------------------------------------------------------------
//--ISO-P32C32--//
//---------------------------------------------------------------------------
unsigned int ISO_P32C32_1 ( unsigned int isoState , unsigned int* value )
{
    // если плата неработоспособна
    if ( isoP32C32err1 )
    {
        // отрубить связь с платой
        ISO_DriverClose();
        // пробуем снова ее открыть
        return OpenISO_P32C32();
    }
    // если ошибок нет, считываем данные
    else
    {
        switch ( isoState )
        {
            case 0:
            {
                unsigned int
                    t1 =   ( ~ ISO_InputByte ( (Word) (base_P32C32_1 + 0) ) ) & 0xFF,
                    t2 = ( ( ~ ISO_InputByte ( (Word) (base_P32C32_1 + 1) ) ) & 0xFF ) << 8;
                    value[0] = t1 + t2;

                //    t1 =   ( ~ ISO_InputByte ( (Word) (base_P32C32_1 + 2) ) ) & 0xFF;
                //    t2 = ( ( ~ ISO_InputByte ( (Word) (base_P32C32_1 + 3) ) ) & 0xFF ) << 8;
                //    value[3] = t1 + t2;
            }; break;
            case 1:
            {
                unsigned int
                    t1 = value[0] & 0x00FF,
                    t2 = ( value[0] & 0xFF00 ) >> 8;
                ISO_OutputByte( (Word)(base_P32C32_1 + 0) , (BYTE)(t1) );
                ISO_OutputByte( (Word)(base_P32C32_1 + 1) , (BYTE)(t2) );
            //        t1 = value[3] & 0x00FF,
            //       t2 = ( value[3] & 0xFF00 ) >> 8;
            //    ISO_OutputByte( (Word)(base_P32C32_1 + 2) , (BYTE)(t1) );
            //    ISO_OutputByte( (Word)(base_P32C32_1 + 3) , (BYTE)(t2) );
            }; break;
        }
    }
    // возвращаем код ошибки
    return isoP32C32err1;
}
//---------------------------------------------------------------------------
unsigned int ISO_P32C32_2 ( unsigned int isoState , unsigned int* value )
{
    // если плата неработоспособна
    if ( isoP32C32err2 )
    {
        // отрубить связь с платой
        ISO_DriverClose();
        // пробуем снова ее открыть
        return OpenISO_P32C32();
    }
    // если ошибок нет, считываем данные
    else
    {
        switch ( isoState )
        {
            case 0:
            {
                unsigned int
                    t1 =   ( ~ ISO_InputByte ( (Word) (base_P32C32_2 + 0) ) ) & 0xFF,
                    t2 = ( ( ~ ISO_InputByte ( (Word) (base_P32C32_2 + 1) ) ) & 0xFF ) << 8;
                    value[2] = t1 + t2;

                    t1 =   ( ~ ISO_InputByte ( (Word) (base_P32C32_2 + 2) ) ) & 0xFF;
                    t2 = ( ( ~ ISO_InputByte ( (Word) (base_P32C32_2 + 3) ) ) & 0xFF ) << 8;
                    value[3] = t1 + t2;
            }; break;
            case 1:
            {
                unsigned int
                    t1 = value[2] & 0x00FF,
                    t2 = ( value[2] & 0xFF00 ) >> 8;
                ISO_OutputByte( (Word)(base_P32C32_2 + 0) , (BYTE)(t1) );
                ISO_OutputByte( (Word)(base_P32C32_2 + 1) , (BYTE)(t2) );
                    t1 = value[3] & 0x00FF,
                    t2 = ( value[3] & 0xFF00 ) >> 8;
                ISO_OutputByte( (Word)(base_P32C32_2 + 2) , (BYTE)(t1) );
                ISO_OutputByte( (Word)(base_P32C32_2 + 3) , (BYTE)(t2) );
            }; break;
        }
    }
    // возвращаем код ошибки
    return isoP32C32err2;
}
//---------------------------------------------------------------------------
//--Усреднение значения (фильтр)--//
//---------------------------------------------------------------------------
unsigned int DoAverageValue(unsigned char valueNmb, unsigned int value)
{
    unsigned char
        minValueNmb = 0,
        maxValueNmb = 0;
    unsigned int
        sum = 0;
    for ( unsigned char i = 0 ; i < ( VALUE_COUNT - 1 ) ; i++ )
        avrgAikValues[valueNmb][i] = avrgAikValues[valueNmb][i+1];
    avrgAikValues[valueNmb][VALUE_COUNT - 1] = value;
    for ( unsigned char i = 1 ; i < ( VALUE_COUNT - 1 ) ; i++ )
    {
        if ( avrgAikValues[valueNmb][i] < avrgAikValues[valueNmb][minValueNmb] ) minValueNmb = i;
        if ( avrgAikValues[valueNmb][i] > avrgAikValues[valueNmb][maxValueNmb] ) maxValueNmb = i;
    }
    if ( minValueNmb == maxValueNmb ) return avrgAikValues[valueNmb][0];
    else
    {
        for ( unsigned char i = 0 ; i < VALUE_COUNT ; i++ )
        {
            if ( ( i != minValueNmb ) && ( i != maxValueNmb ) )
                sum += avrgAikValues[valueNmb][i];
        }
        return sum / ( VALUE_COUNT - 2 );
    }
}
//---------------------------------------------------------------------------
//--ISO-813--//
//---------------------------------------------------------------------------
unsigned int ISO_813 ( unsigned int* value , int valueCount )
{
    // если плата неработоспособна
    if ( iso813err )
    {
        // отрубить связь с платой
        ISO813_DriverClose();
        // пробуем снова ее открыть
        return OpenISO_813();
    }
    // если ошибок нет, считываем данные
    else
    {
        for ( int i = 0 ; i < valueCount ; i++ )
            value[i] = DoAverageValue(i, ISO813_AD_Hex(base_813,i,0));
    }
    // возвращаем код ошибки
    return iso813err;
}
//---------------------------------------------------------------------------
//--ISO-DA16--//
//---------------------------------------------------------------------------
unsigned int ISO_DA16 ( unsigned int isoState, unsigned int value, unsigned int* valueKon, int signNmb )
{
    switch ( isoState )
    {
        // запись уставки
        case 0:
        {
            // если плата неработоспособна
            if ( isoDA16err )
            {
                // отрубить связь с платой
                ISODA_DriverClose();
                // пробуем снова ее открыть
                return OpenISO_DA16();
            }
            // если ошибок нет, записываем данные
            else
            {
                // убираем функцию записи с дублированием в ПЗУ
                ISODA_AnalogOutput( signNmb , value );
            }
            // возвращаем код ошибки
            return isoDA16err;
        }; break;
        // чтение уставок
        case 1:
        {

            // если плата неработоспособна
            if ( isoDA16err )
            {
                // отрубить связь с платой
                ISODA_DriverClose();
                // пробуем снова ее открыть
                return OpenISO_DA16();
            }
            // если ошибок нет, записываем данные
            else
            {
                DWORD valDWord[1];
                ISODA_ReadPowerOnValue(signNmb , valDWord);
                valueKon[signNmb] = (unsigned int)(valDWord[0]&0xFFFF);
            }
            // возвращаем код ошибки
            return isoDA16err;

        }; break;
    }
}
//---------------------------------------------------------------------------
//--ACL7250--//
//---------------------------------------------------------------------------
unsigned int ACL7250 ( unsigned int aclState , unsigned int* value )
{
  unsigned long zH1 = 0;
  unsigned long zH2 = 0;
  unsigned long zH3 = 0;
  unsigned long zH4 = 0;
  unsigned long  h = 0;
    // если плата неработоспособна
    if ( iso7250err != ERR_NoError )
    {
        // пробуем снова ее открыть
        return OpenACL_7250();
    }
    // если ошибок нет, считываем данные
    else
    {
        switch ( aclState )
        {
            // чтение дискретных сигналов
            case 0:
            {
                DI_ReadPort(0, 0x00, &zH1);
                DI_ReadPort(0, 0x01, &zH2);
                //DI_ReadPort(0, 0x02, &zH3);
                //DI_ReadPort(0, 0x03, &zH4);
                zH2 = (zH2 << 8) & 0xFFFF;
                value[4] = zH1 + zH2;
                //zH4 = (zH4 << 8) & 0xFFFF;
                //value[3] = zH3 + zH4;
            }; break;
            // запись дискретных сигналов
            case 1:
            {
                unsigned short v1 = (unsigned short)value[4] & 0xFF;
                h  = value[4] >> 8;
                unsigned short v2 = (unsigned short)h & 0xFF;
		        //unsigned short v3 = (unsigned short)value[5] & 0xFF;
                //h  = value[3] >> 8;
                //unsigned short v4 = (unsigned short)h & 0xFF;

                DO_WritePort(0, 0x00, v1);
                DO_WritePort(0, 0x01, v2);
                //DO_WritePort(0, 0x02, v3);
                //DO_WritePort(0, 0x03, v4);
            }; break;
        }
    }
    // возвращаем код ошибки
    return iso7250err;
}
//---------------------------------------------------------------------------
//--PISO-P32C32U--//
//---------------------------------------------------------------------------
unsigned int PISO_P32C32U_1( unsigned int pisoState , unsigned int* value )
{
	switch(pisoState)
	{
		case 0:
		{
			unsigned int
			t1 =   ( ~ PISODIO_InputByte ( (Word) (base_PISO_P32C32_1 + 0xC0) ) ) & 0xFF,
			t2 = ( ( ~ PISODIO_InputByte ( (Word) (base_PISO_P32C32_1 + 0xC4) ) ) & 0xFF ) << 8;
			value[0] = t1 + t2;
			t1 =   ( ~ PISODIO_InputByte ( (Word) (base_PISO_P32C32_1 + 0xC8) ) ) & 0xFF,
			t2 = ( ( ~ PISODIO_InputByte ( (Word) (base_PISO_P32C32_1 + 0xCC) ) ) & 0xFF ) << 8;
			value[1] = t1 + t2;
            return 0;
		}; break;
		case 1:
		{
			unsigned int
			t1 = value[0] & 0x00FF,
			t2 = ( value[0] & 0xFF00 ) >> 8;
			PISODIO_OutputByte( (Word)(base_PISO_P32C32_1 + 0xC0) , (BYTE)(t1) );
			PISODIO_OutputByte( (Word)(base_PISO_P32C32_1 + 0xC4) , (BYTE)(t2) );
			t1 = value[1] & 0x00FF,
			t2 = (value[1] & 0xFF00) >> 8;
			PISODIO_OutputByte((Word)(base_PISO_P32C32_1 + 0xC8),(BYTE)(t1));
			PISODIO_OutputByte((Word)(base_PISO_P32C32_1 + 0xCC),(BYTE)(t2));
            return 0;
		}; break;
	}
}
//---------------------------------------------------------------------------
//--PISO-P32C32U--//
//---------------------------------------------------------------------------
unsigned int PISO_P32C32U_2( unsigned int pisoState , unsigned int* value )
{
	switch(pisoState)
	{
		case 0:
		{
			unsigned int
			t1 =   ( ~ PISODIO_InputByte ( (Word) (base_PISO_P32C32_2 + 0xC0) ) ) & 0xFF,
			t2 = ( ( ~ PISODIO_InputByte ( (Word) (base_PISO_P32C32_2 + 0xC4) ) ) & 0xFF ) << 8;
			value[2] = t1 + t2;
			t1 =   ( ~ PISODIO_InputByte ( (Word) (base_PISO_P32C32_2 + 0xC8) ) ) & 0xFF,
			t2 = ( ( ~ PISODIO_InputByte ( (Word) (base_PISO_P32C32_2 + 0xCC) ) ) & 0xFF ) << 8;
			value[3] = t1 + t2;
            return 0;
		}; break;
		case 1:
		{
			unsigned int
			t1 = value[2] & 0x00FF,
			t2 = ( value[2] & 0xFF00 ) >> 8;
			PISODIO_OutputByte( (Word)(base_PISO_P32C32_2 + 0xC0) , (BYTE)(t1) );
			PISODIO_OutputByte( (Word)(base_PISO_P32C32_2 + 0xC4) , (BYTE)(t2) );
			t1 = value[3] & 0x00FF,
			t2 = (value[3] & 0xFF00) >> 8;
			PISODIO_OutputByte((Word)(base_PISO_P32C32_2 + 0xC8),(BYTE)(t1));
			PISODIO_OutputByte((Word)(base_PISO_P32C32_2 + 0xCC),(BYTE)(t2));
            return 0;
		}; break;
	}
}
//---------------------------------------------------------------------------
//--PISO-813U--//
//---------------------------------------------------------------------------
unsigned int PISO_813U( unsigned int* value , int valueCount )
{
    for ( int i = 0 ; i < valueCount ; i++ )
    {
        PISO813_SetChGain(base_PISO_813, i, 0);
        value[i] = DoAverageValue(i,PISO813_AD_Hex(base_PISO_813));
    }
    return 0;
}
//---------------------------------------------------------------------------
//--PISO-P16R16U--//
//---------------------------------------------------------------------------
unsigned int PISO_P16R16U_1( unsigned int pisoState , unsigned int* value )
{
	switch(pisoState)
	{
		case 0:
		{
			unsigned int
			//t1 =   ( ~ PISODIO_InputByte ( (Word) (base_PISO_P16R16_1 + 0xC0) ) ) & 0xFF,
			//t2 = ( ( ~ PISODIO_InputByte ( (Word) (base_PISO_P16R16_1 + 0xC4) ) ) & 0xFF ) << 8;
            t1 =   ( PISODIO_InputByte ( (Word) (base_PISO_P16R16_1 + 0xC0) ) ) & 0xFF,
			t2 = ( ( PISODIO_InputByte ( (Word) (base_PISO_P16R16_1 + 0xC4) ) ) & 0xFF ) << 8;
			value[1] = t1 + t2;
            return 0;
		}; break;
		case 1:
		{
			unsigned int
			t1 = value[1] & 0x00FF,
			t2 = ( value[1] & 0xFF00 ) >> 8;
			PISODIO_OutputByte( (Word)(base_PISO_P16R16_1 + 0xC0) , (BYTE)(t1) );
			PISODIO_OutputByte( (Word)(base_PISO_P16R16_1 + 0xC4) , (BYTE)(t2) );
            return 0;
		}; break;
	}
}
//---------------------------------------------------------------------------
//--PISO-P16R16U--//
//---------------------------------------------------------------------------
unsigned int PISO_P16R16U_2( unsigned int pisoState , unsigned int* value )
{
	switch(pisoState)
	{
		case 0:
		{
			unsigned int
			//t1 =   ( ~ PISODIO_InputByte ( (Word) (base_PISO_P16R16_2 + 0xC0) ) ) & 0xFF,
			//t2 = ( ( ~ PISODIO_InputByte ( (Word) (base_PISO_P16R16_2 + 0xC4) ) ) & 0xFF ) << 8;
            t1 =   ( PISODIO_InputByte ( (Word) (base_PISO_P16R16_2 + 0xC0) ) ) & 0xFF,
			t2 = ( ( PISODIO_InputByte ( (Word) (base_PISO_P16R16_2 + 0xC4) ) ) & 0xFF ) << 8;
			value[2] = t1 + t2;
            return 0;
		}; break;
		case 1:
		{
			unsigned int
			t1 = value[2] & 0x00FF,
			t2 = ( value[2] & 0xFF00 ) >> 8;
			PISODIO_OutputByte( (Word)(base_PISO_P16R16_2 + 0xC0) , (BYTE)(t1) );
			PISODIO_OutputByte( (Word)(base_PISO_P16R16_2 + 0xC4) , (BYTE)(t2) );
            return 0;
		}; break;
	}
}
//---------------------------------------------------------------------------

