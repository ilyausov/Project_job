//---------------------------------------------------------------------------
#pragma hdrstop
#include "DZVat.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//--���� �������� �������� ������ ��� ��������--//
//---------------------------------------------------------------------------

//-------------------------------//
//--��������� ������ �� �������--//
//-------------------------------//
/*AnsiString AnswerToAnsiString ( unsigned char Pack[] )
{
    AnsiString rezult = "";
    int i = 0;
	
	do
	{
		// ���������� ������ ������
		rezult += AnsiString(char(Pack[i]));
		i++;
	}
    while ( Pack[i] != 0 );
    
    return rezult;
}  */
//------------------------------------//
//--�������� ������� �����/��������--//
//------------------------------------//
void com1_PackageClear ()
{
        for (int i = 0 ; i < PACKAGE_COUNT ; i++)
        {
                com1_PackOut[i] = 0;
                com1_PackIn[i] = 0;
        }
}
//-------------------------------------//
//----- ������������ ������� � �� -----//
//-------------------------------------//
void FrmZapDZVat( bool com )
{
    if(!KOM_ZASL) KOM_ZASL = 5;
	AcomDZVat = KOM_ZASL;
	
	AnsiString 	temp_buf = "",
				temp_date = "";
	unsigned int lenFull = 0,
				 lenDate = 0;
	
	if(com) // ������ ������
	{
		temp_buf = Req_Buf[RcomDZVat];		// ����� �������������� ������ �� �������
		lenFull = temp_buf.Length();		// ���������� � �����
		if((RcomDZVat == 1)||(RcomDZVat == 2))	// ���� ����� �������� ����
		{
			temp_date = IntToStr(DATA_ZASL);// ������������ ������ �� ����
			lenDate = temp_date.Length();	// ���������� � �����
			for(int i=1; i<=lenDate; i++)	// ��������� � ����� ������
				temp_buf[lenFull-lenDate+i] = temp_date[i];
		}
		for(int i=0; i<lenFull; i++)		// ���������� ������ � ������ char
			com1_PackOut[i] = temp_buf[i+1];
		
		com1_PackOut[lenFull] = 0x0D;		// ��������� ���������
		com1_PackOut[lenFull + 1] = 0x0A;
	}
	else // �������������� ������
	{
		temp_buf = Req_Buf[AcomDZVat];		// ����� �������������� ������ �� �������
		lenFull = temp_buf.Length();		// ���������� � �����
		if((AcomDZVat == 1)||(AcomDZVat == 2))	// ���� ����� �������� ����
		{
			temp_date = IntToStr(DATA_ZASL);// ������������ ������ �� ����
			lenDate = temp_date.Length();	// ���������� � �����
			for(int i=1; i<=lenDate; i++)	// ��������� � ����� ������
				temp_buf[lenFull-lenDate+i] = temp_date[i];
		}
		for(int i=0; i<lenFull; i++)		// ���������� ������ � ������ char
			com1_PackOut[i] = temp_buf[i+1];
		
		com1_PackOut[lenFull] = 0x0D;
		com1_PackOut[lenFull + 1] = 0x0A;	// ��������� ���������
	}
}
//---------------------------------------------------------------------------
//-- ��������� ������ �� �� --//
//---------------------------------------------------------------------------
void ChkRepDZVat( bool com )
{
	// ���������������� ����� �����
	char temp_str[6] = {0};
	
	for(int i=0; i<=5; i++) temp_str[i]=com1_PackIn[4+i];
	    TEK_POZ_ZASL=atoi(temp_str);
	for(int i=0; i<=6; i++) temp_str[i]=com1_PackIn[11+i];
	    TEK_DAVL_ZASL=atoi(temp_str);
    if(com1_PackIn[10] != '0')
        TEK_DAVL_ZASL = (-1)*TEK_DAVL_ZASL;

    if(TEK_DAVL_ZASL < 0) D_D3 = 0;
	else if(TEK_DAVL_ZASL > 10000) D_D3 = 10000;
    else D_D3 = TEK_DAVL_ZASL;
	
	FormMain->Edit_DZVat_TekPos->Text = IntToStr(TEK_POZ_ZASL);
	FormMain->Edit_DZVat_TekDavl->Text = IntToStr(TEK_DAVL_ZASL);
}
//---------------------------------------------------------------------------
//--��������� ������� � ��--//
//---------------------------------------------------------------------------
int DZVat( unsigned int SH , bool com)
{
	FormMain -> RB_com1_1_prd -> Checked = !SH;
	FormMain -> RB_com1_1_prm -> Checked = SH;
	
    if(SH == 0) // ������� �������
	{
		com1_PackageClear();
		// ������� ������������ �������
		FrmZapDZVat( com );

		FormMain->Edit_com1_prd -> Text = AnswerToAnsiString( com1_PackOut );

		// ������� ������ �����
		Comport1.ResetRB();
		// �������� �������
		Comport1.Write(com1_PackOut,strlen(com1_PackOut));
		// ������� �� ��������� ���
		com1_DevState++;
		// ��������� �������
		com1_Dev_Timer = 0;
		return 0;
	}
	else if(SH == 1)
	{
		// ������ ������
		Comport1.Read(com1_PackIn, Comport1.GetRBSize());
		// �������� ������
		if(	(com1_PackIn[2] == 0x0D) ||
			(((com1_PackIn[21] == 0x0D))&&(AcomDZVat == 5)) ||
			(((com1_PackIn[21] == 0x0D))&&(RcomDZVat == 5))	)
		{
			// ����� �����
            OTVET_ZASL = AcomDZVat;
            
            FormMain->Edit_com1_prm -> Text = AnswerToAnsiString( com1_PackIn );
			
			if(com) // ������ �����
			{			
				if(RcomDZVat == 5) // ����� �� ������ ���������
					ChkRepDZVat( com );
			}
			else // ��������������
			{	
				if(AcomDZVat == 5) // ����� �� ������ ���������
				{
					ChkRepDZVat( com );
					PR_ZASL = 1;
				}
				else
				{
					KOM_ZASL = 5;	// ����� ��������� ������� ������������ � ������ ������
					PR_ZASL = 1;
				}
			}
			
			// ������� �� ��������� ���
			com1_DevState++;
			// ����� �������� ������ �����
			errDZVat = 0;
			return 0;
		}
		else
		{
            if(com1_Dev_Timer < com1_Dev_TK) return 0; // ���� ������
			// �� ���������
			com1_DevState++; // ��������� � ���������� ����������
			// ���������� �������� ������ ����� � ��������� �� ��������
			if(( errDZVat++ ) > maxDZVat )
				return 1;
			return 0;
		}
    }
}
//---------------------------------------------------------------------------