#include <vcl.h>
#include "ISO.h"
#include "ISO813.h"
#include "ISODA.h"
#include "Dll1.H"
#include "External.h"
#include "Dask.h"
//---------------------------------------------------------------------------
//--���� �������� ����������� ������� ���������--//
//---------------------------------------------------------------------------
// ����������� �������
int OpenISO_P32C32 (); // ������� ��������� � ��������� ISO-P32C32
int OpenACL_7225_1 ();   // ������� ��������� � ��������� ACL-7225
int OpenACL_7225_2 ();   // ������� ��������� � ��������� ACL-7225
int OpenISO_813 ();    // ������� ��������� � ��������� ISO-813
int OpenISO_DA16();    // ������� ��������� � ��������� ISO-DA16
int OpenACL_7250 ();   // ������� ��������� � ��������� ACL-7250
//---------------------------------------------------------------------------
// ������/������ ���������� �������� � ACL-7225
unsigned int ACL7225_1 ( unsigned int aclState , unsigned int* value );
unsigned int ACL7225_2 ( unsigned int aclState , unsigned int* value );
// ������/������ ���������� �������� � P32C32
unsigned int ISO_P32C32_1 ( unsigned int isoState , unsigned int* value );
unsigned int ISO_P32C32_2 ( unsigned int isoState , unsigned int* value );
// ������ ���������� �������� � 813
unsigned int ISO_813 ( unsigned int* value , char valueCount );
// ������ ���������� �������� � DA16 � ������ �������
unsigned int ISO_DA16 ( unsigned int isoState, unsigned int value, unsigned int* valueKon, int signNmb );
// ������/������ ���������� �������� � ACL-7250
unsigned int ACL7250 ( unsigned int aclState , unsigned int* value );
//---------------------------------------------------------------------------
//--��������� � ��������� ISO-DA16--//
//---------------------------------------------------------------------------
int OpenISO_DA16 ()
{
    ISODA_DriverInit();
    return isoDA16err = ISODA_CheckBoard( 0 , base_DA16 , 0 );
}
//---------------------------------------------------------------------------
//--��������� � ��������� ISO-P32C32--//
//---------------------------------------------------------------------------
int OpenISO_P32C32 ()
{
    isoP32C32err1 = ISO_DriverInit();
	return isoP32C32err2 = isoP32C32err1;
}
//---------------------------------------------------------------------------
//--��������� � ��������� ACL-7225--//
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
//--��������� � ��������� ISO-813--//
//---------------------------------------------------------------------------
int OpenISO_813 ()
{
    return iso813err = ISO813_DriverInit();
}
//---------------------------------------------------------------------------
//--��������� � ��������� ACL-7250--//
//---------------------------------------------------------------------------
int OpenACL_7250 ()
{
    return iso7250err = Register_Card(PCI_7250, 0);
}
//---------------------------------------------------------------------------
//--ACL7225--//
//---------------------------------------------------------------------------
unsigned int ACL7225_1 ( unsigned int aclState , unsigned int* value )
{
    // ���� ����� ����������������
    if ( iso7225err1 != ERR_NoError )
    {
        // ������� ����� �� �������
        return OpenACL_7225_1();
    }
    // ���� ������ ���, ��������� ������
    else
    {
        W_7225_Set_Card(CARD_1);
        switch ( aclState )
        {
            // ������ ���������� ��������
            case 0:
            {
                iso7225err1 = W_7225_DI ( &value[2] );
            }; break;
            // ������ ���������� ��������
            case 1:
            {
                iso7225err1 = W_7225_DO ( value[2] );
            }; break;
        }
    }
    // ���������� ��� ������
    return iso7225err1;
}
//---------------------------------------------------------------------------
unsigned int ACL7225_2 ( unsigned int aclState , unsigned int* value )
{
    // ���� ����� ����������������
    if ( iso7225err2 != ERR_NoError )
    {
        // ������� ����� �� �������
        return OpenACL_7225_2();
    }
    // ���� ������ ���, ��������� ������
    else
    {
        W_7225_Set_Card(CARD_2);
        switch ( aclState )
        {
            // ������ ���������� ��������
            case 0:
            {
                iso7225err2 = W_7225_DI ( &value[3] );
            }; break;
            // ������ ���������� ��������
            case 1:
            {
                iso7225err2 = W_7225_DO ( value[3] );
            }; break;
        }
    }
    // ���������� ��� ������
    return iso7225err2;
}
//---------------------------------------------------------------------------
//--ISO-P32C32--//
//---------------------------------------------------------------------------
unsigned int ISO_P32C32_1 ( unsigned int isoState , unsigned int* value )
{
    // ���� ����� ����������������
    if ( isoP32C32err1 )
    {
        // �������� ����� � ������
        ISO_DriverClose();
        // ������� ����� �� �������
        return OpenISO_P32C32();
    }
    // ���� ������ ���, ��������� ������
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

                    t1 =   ( ~ ISO_InputByte ( (Word) (base_P32C32_1 + 2) ) ) & 0xFF;
                    t2 = ( ( ~ ISO_InputByte ( (Word) (base_P32C32_1 + 3) ) ) & 0xFF ) << 8;
                    value[1] = t1 + t2;
            }; break;
            case 1:
            {
                unsigned int
                    t1 = value[0] & 0x00FF,
                    t2 = ( value[0] & 0xFF00 ) >> 8;
                ISO_OutputByte( (Word)(base_P32C32_1 + 0) , (BYTE)(t1) );
                ISO_OutputByte( (Word)(base_P32C32_1 + 1) , (BYTE)(t2) );
                    t1 = value[1] & 0x00FF,
                    t2 = ( value[1] & 0xFF00 ) >> 8;
                ISO_OutputByte( (Word)(base_P32C32_1 + 2) , (BYTE)(t1) );
                ISO_OutputByte( (Word)(base_P32C32_1 + 3) , (BYTE)(t2) );
            }; break;
        }
    }
    // ���������� ��� ������
    return isoP32C32err1;
}
//---------------------------------------------------------------------------
unsigned int ISO_P32C32_2 ( unsigned int isoState , unsigned int* value )
{
    // ���� ����� ����������������
    if ( isoP32C32err2 )
    {
        // �������� ����� � ������
        ISO_DriverClose();
        // ������� ����� �� �������
        return OpenISO_P32C32();
    }
    // ���� ������ ���, ��������� ������
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
                    //t1 = value[3] & 0x00FF,
                    //t2 = ( value[3] & 0xFF00 ) >> 8;
                //ISO_OutputByte( (Word)(base_P32C32_2 + 2) , (BYTE)(t1) );
                //ISO_OutputByte( (Word)(base_P32C32_2 + 3) , (BYTE)(t2) );
            }; break;
        }
    }
    // ���������� ��� ������
    return isoP32C32err2;
}
//---------------------------------------------------------------------------
//--���������� �������� (������)--//
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
    // ���� ����� ����������������
    if ( iso813err )
    {
        // �������� ����� � ������
        ISO813_DriverClose();
        // ������� ����� �� �������
        return OpenISO_813();
    }
    // ���� ������ ���, ��������� ������
    else
    {
        for ( int i = 0 ; i < valueCount ; i++ )
            value[i] = DoAverageValue(i, ISO813_AD_Hex(base_813,i,0));
    }
    // ���������� ��� ������
    return iso813err;
}
//---------------------------------------------------------------------------
//--ISO-DA16--//
//---------------------------------------------------------------------------
unsigned int ISO_DA16 ( unsigned int isoState, unsigned int value, unsigned int* valueKon, int signNmb )
{
    switch ( isoState )
    {
        // ������ �������
        case 0:
        {
            // ���� ����� ����������������
            if ( isoDA16err )
            {
                // �������� ����� � ������
                ISODA_DriverClose();
                // ������� ����� �� �������
                return OpenISO_DA16();
            }
            // ���� ������ ���, ���������� ������
            else
            {
                // ������� ������� ������ � ������������� � ���
                ISODA_AnalogOutput( signNmb , value );
            }
            // ���������� ��� ������
            return isoDA16err;
        }; break;
        // ������ �������
        case 1:
        {

            // ���� ����� ����������������
            if ( isoDA16err )
            {
                // �������� ����� � ������
                ISODA_DriverClose();
                // ������� ����� �� �������
                return OpenISO_DA16();
            }
            // ���� ������ ���, ���������� ������
            else
            {
                DWORD valDWord[1];
                ISODA_ReadPowerOnValue(signNmb , valDWord);
                valueKon[signNmb] = (unsigned int)(valDWord[0]&0xFFFF);
            }
            // ���������� ��� ������
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
    // ���� ����� ����������������
    if ( iso7250err != ERR_NoError )
    {
        // ������� ����� �� �������
        return OpenACL_7250();
    }
    // ���� ������ ���, ��������� ������
    else
    {
        switch ( aclState )
        {
            // ������ ���������� ��������
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
            // ������ ���������� ��������
            case 1:
            {
                unsigned short v1 = (unsigned short)value[3] & 0xFF;
                h  = value[3] >> 8;
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
    // ���������� ��� ������
    return iso7250err;
}
//---------------------------------------------------------------------------



