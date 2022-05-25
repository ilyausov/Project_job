//---------------------------------------------------------------------------
#pragma hdrstop
#include "Dat.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//--���� �������� �������� ������ ��� ��������--//
//---------------------------------------------------------------------------

//-------------------------------//
//--��������� ������ �� �������--//
//-------------------------------//
AnsiString AnswerToAnsiString ( unsigned char Pack[] )
{
    AnsiString
        rezult = "";
    int
        i = 0;
    
	do
	{
		// ���������� ������ ������
		rezult += AnsiString(char(Pack[i]));
		i++;
	}
    while ( Pack[i] != 0 );
    
    return rezult;
}
//------------------------------------//
//--�������� ������� �����/��������--//
//------------------------------------//
void com3_PackageClear ()
{
        for (int i = 0 ; i < PACKAGE_COUNT ; i++)
        {
                com3_PackOut[i] = 0;
                com3_PackIn[i] = 0;
        }
}
//---------------------------------------------------------------------------
//--������� char � HEX--//
//---------------------------------------------------------------------------
BYTE CharToHex ( char Value )
{
        if ( (Value <= 'F') && ( Value >= 'A' ) ) return ( Value - 'A' + 10);
        else                                      return ( Value - '0' );
};
//---------------------------------------------------------------------------
//--������� ���������� ���� � SS-10--//
//---------------------------------------------------------------------------
/*int SS_10( unsigned int SH )
{
	FormMain -> RB_com3_2_prd -> Checked = !SH;
	FormMain -> RB_com3_2_prm -> Checked = SH;
	
    if(SH == 0) // ������� �������
	{
		// ������� ������������ �������

            com3_PackageClear();
            // ������������ �������
            com3_PackOut[0] = 0x02;
            com3_PackOut[1] = '2';
            com3_PackOut[2] = 'S';
            com3_PackOut[3] = '1';
            com3_PackOut[4] = 0x0D;
			FormMain->Edit_com3_prd -> Text = AnswerToAnsiString( com3_PackOut );

            // ������� ������ �����
            Comport3.ResetRB();
            // �������� �������
            Comport3.Write(com3_PackOut,5);
            // ������� �� ��������� ���
            com3_DevState++;
            // ��������� �������
            com3_Dev_Timer = 0;
            return 0;
	}
    else if(SH == 1) // ������� �������
	{
		// ������� ������������ �������

            com3_PackageClear();
            // ������������ �������
            com3_PackOut[0] = 0x02;
            com3_PackOut[1] = '2';
            com3_PackOut[2] = 'S';
            com3_PackOut[3] = '1';
            com3_PackOut[4] = 0x0D;
			FormMain->Edit_com3_prd -> Text = AnswerToAnsiString( com3_PackOut );

            // ������� ������ �����
            Comport3.ResetRB();
            // �������� �������
            Comport3.Write(com3_PackOut,5);
            // ������� �� ��������� ���
            com3_DevState++;
            // ��������� �������
            com3_Dev_Timer = 0;
            return 0;
	}
	else if(SH == 2)
	{
		// ������ ������
		Comport3.Read(com3_PackIn, Comport3.GetRBSize());
		// �������������� ������ � �������
		if((com3_PackIn[0] == 0x02)&&(com3_PackIn[1] == '2')&&(com3_PackIn[6] != 0))
        { // ����� �����
            FormMain->Edit_com3_prm -> Text = AnswerToAnsiString( com3_PackIn );
			float result = 0;
			result = (float)(com3_PackIn[3] - '0') + (float)(com3_PackIn[4] - '0') / 10.0;
			for ( unsigned char i = 0 ; i < ( com3_PackIn[6] - '0' ) ; i++ )
			{
				// ���� ������� ������������� - �������
				if( com3_PackIn[5] == '1' )
					result *= 10.0;
				// ������������� - ������
				else
					result /= 10.0;
			}
			if ( (double)(result/133.322) > 0 )
				D_D2 = (int)(1000.0*(0.5*log10((double)(result/133.322))+8.5));
			// ������� �� ��������� ���
			com3_DevState++;
			// ����� �������� ������ �����
			errSS10 = 0;
			return 0;
		}
		else
		{
            if(com3_Dev_Timer < com3_Dev_TK) return 0; // ���� ������
			// �� ���������
			com3_DevState++; // ��������� � ���������� ����������
			// ���������� �������� ������ ����� � ��������� �� ��������
			if(( errSS10++ ) > maxErrSS10 )
				return 1;
			return 0;
		}
    }
}
//---------------------------------------------------------------------------
//--������� �������� � 925--//
//---------------------------------------------------------------------------
int D925( unsigned int SH )
{
	FormMain -> RB_com3_1_prd -> Checked = !SH;
	FormMain -> RB_com3_1_prm -> Checked = SH;
	
    if(SH == 0) // ������� �������
	{
		// ������� ������������ �������

            com3_PackageClear();
            // ������������ �������
            com3_PackOut[0] = '@';
            com3_PackOut[1] = '0';
            com3_PackOut[2] = '0';
            com3_PackOut[3] = '1';
            com3_PackOut[4] = 'P';
            com3_PackOut[5] = 'R';
            com3_PackOut[6] = '1';
            com3_PackOut[7] = '?';
            com3_PackOut[8] = ';';
            com3_PackOut[9] = 'F';
            com3_PackOut[10]= 'F';
			FormMain->Edit_com3_prd -> Text = AnswerToAnsiString( com3_PackOut );
			
            // ������� ������ �����
            Comport3.ResetRB();
            // �������� �������
            Comport3.Write(com3_PackOut,11);
            // ������� �� ��������� ���
            com3_DevState++;
            // ��������� �������
            com3_Dev_Timer = 0;
            return 0;
	}
	else if(SH == 1)
	{
		// ������ ������
		Comport3.Read(com3_PackIn, Comport3.GetRBSize());
		// �������������� ������ � �������
		if((com3_PackIn[0] == '@')&&(com3_PackIn[14] != 0))
        { // ����� �����
            FormMain->Edit_com3_prm -> Text = AnswerToAnsiString( com3_PackIn );
			float Davl = 0;

			Davl = float ( com3_PackIn[7] - '0' );
			Davl += float ( com3_PackIn[9] - '0' ) / 10;
			Davl += float ( com3_PackIn[10] - '0' ) / 100;
			
			if ( com3_PackIn[12] == '+' )
				for ( int i = 0 ; i < ( com3_PackIn[13] - '0' ) ; i++ ) Davl*=10;
			else
				for ( int i = 0 ; i < ( com3_PackIn[13] - '0' ) ; i++ ) Davl/=10;
			
			if ( ((double)(Davl/133.322)) > 0 )
				D_D1 = 1000.0*(log10((double)(Davl/133.322))+6);
			else
				D_D1 = 0;	
	
			// ������� �� ��������� ���
			com3_DevState++;
			// ����� �������� ������ �����
			errD925 = 0;
			return 0;
		}
		else
		{
            if(com3_Dev_Timer < com3_Dev_TK) return 0; // ���� ������
			// �� ���������
			com3_DevState++; // ��������� � ���������� ����������
			// ���������� �������� ������ ����� � ��������� �� ��������
			if(( errD925++ ) > maxErrD925 )
				return 1;
			return 0;
		}
    }
}        */
//---------------------------------------------------------------------------
//--������� �������� � 972--//
//---------------------------------------------------------------------------
int D972( unsigned int SH )
{
	FormMain -> RB_com3_1_prd -> Checked = !SH;
	FormMain -> RB_com3_1_prm -> Checked = SH;
	
    if(SH == 0) // ������� �������
	{
		// ������� ������������ �������

            com3_PackageClear();
            // ������������ �������
            com3_PackOut[0] = '@';
            com3_PackOut[1] = '0';
            com3_PackOut[2] = '0';
            com3_PackOut[3] = '1';
            com3_PackOut[4] = 'P';
            com3_PackOut[5] = 'R';
            com3_PackOut[6] = '3';
            com3_PackOut[7] = '?';
            com3_PackOut[8] = ';';
            com3_PackOut[9] = 'F';
            com3_PackOut[10]= 'F';
            FormMain->Edit_com3_prd -> Text = AnswerToAnsiString( com3_PackOut );
			
            // ������� ������ �����
            Comport3.ResetRB();
            // �������� �������
            Comport3.Write(com3_PackOut,15);
            // ������� �� ��������� ���
            com3_DevState++;
            // ��������� �������
            com3_Dev_Timer = 0;
            return 0;
	}
	else if(SH == 1)
	{
		// ������ ������
		Comport3.Read(com3_PackIn, Comport3.GetRBSize());
		// �������������� ������ � �������
		if((com3_PackIn[0] == '@')&&(com3_PackIn[14] != 0))
        { // ����� �����
            FormMain->Edit_com3_prm -> Text = AnswerToAnsiString( com3_PackIn );
			float Davl = 0;

			Davl = float ( com3_PackIn[7] - '0' );
			Davl += float ( com3_PackIn[9] - '0' ) / 10;
			Davl += float ( com3_PackIn[10] - '0' ) / 100;
			
			if ( com3_PackIn[12] == '+' )
				for ( int i = 0 ; i < ( com3_PackIn[13] - '0' ) ; i++ ) Davl*=10;
			else
				for ( int i = 0 ; i < ( com3_PackIn[13] - '0' ) ; i++ ) Davl/=10;
			
			if ( ((double)(Davl/133.322)) > 0 )
				D_D4 = int((log10((double)(Davl))+9.0)*1000.0/2.0);	// 972
			else
				D_D4 = 0;	
	
			// ������� �� ��������� ���
			com3_DevState++;
			// ����� �������� ������ �����
			errD972 = 0;
			return 0;
		}
		else
		{
            if(com3_Dev_Timer < com3_Dev_TK) return 0; // ���� ������
			// �� ���������
			com3_DevState++; // ��������� � ���������� ����������
			// ���������� �������� ������ ����� � ��������� �� ��������
			if(( errD972++ ) > maxErrD972 )
				return 1;
			return 0;
		}
    }
}
//---------------------------------------------------------------------------
//--������� �������� � I7017--//
//---------------------------------------------------------------------------
int I7017( unsigned int SH )
{
 /*	FormMain -> RB_com4_2_prd -> Checked = !SH;
	FormMain -> RB_com4_2_prm -> Checked = SH;
	
    if(SH == 0) // ������� �������
	{
		// ������� ������������ �������

            com4_PackageClear();
            // ������������ �������		#02xD
            com4_PackOut[0] = '#';
            com4_PackOut[1] = '0';
            com4_PackOut[2] = '2';
            com4_PackOut[3] = '0';
            com4_PackOut[4] = 0x0D;

			FormMain->Edit_com4_prd -> Text = AnswerToAnsiString( com4_PackOut );
			
            // ������� ������ �����
            Comport4.ResetRB();
            // �������� �������
            Comport4.Write(com4_PackOut,5);
            // ������� �� ��������� ���
            com4_DevState++;
            // ��������� �������
            com4_Dev_Timer = 0;
            return 0;
	}
	else if(SH == 1)
	{
		// ������ ������
		Comport4.Read(com4_PackIn, Comport4.GetRBSize());
		// �������������� ������ � �������
                if(1==1)
		if((com4_PackIn[0] == '>')&&(com4_PackIn[5] != 0))
        { // ����� �����
            FormMain->Edit_com4_prm -> Text = AnswerToAnsiString( com4_PackIn );
			
            unsigned int Value = 0;
            for ( int j = 1 ; j < 5 ; j++ )
            Value = Value * 0x10 + CharToHex( com4_PackIn[j] );

            aik[15] = Value;

            if( !( aik[15] & 0x8000 )) aik[15] = 0;
            else aik[15] = 0xFFFF - aik[15];

			// ������� �� ��������� ���
			com4_DevState++;
			// ����� �������� ������ �����
			err7017 = 0;
			return 0;
		}
		else
		{
            if(com4_Dev_Timer < com4_Dev_TK) return 0; // ���� ������
			// �� ���������
			com4_DevState++; // ��������� � ���������� ����������
			// ���������� �������� ������ ����� � ��������� �� ��������
			if(( err7017++ ) > maxErr7017 )
				return 1;
			return 0;
		}
    }    */
}
//---------------------------------------------------------------------------