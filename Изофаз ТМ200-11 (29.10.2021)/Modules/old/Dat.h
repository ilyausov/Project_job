//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
	extern TFormMain *FormMain;

	unsigned char
	// ���������� ������ ����� ������ � ����������� ���������� ������ �����
	errD925 = 0, maxErrD925 = 5,
	errD972 = 0, maxErrD972 = 5,
	errSS10 = 0, maxErrSS10  = 5,
	err7017 = 0, maxErr7017  = 5;
	
	// ����������� �������
	int D925 ( unsigned int SH ); // ������� ����� � 925
	int D972 ( unsigned int SH ); // ������� ����� � 972
	int SS_10 ( unsigned int SH ); // ������� ����� � ��-10
	int I7017 ( unsigned int SH ); // ������� ����� � I7017
	// ���������������
	void com3_PackageClear ();		// ������� �������� ������
	AnsiString AnswerToAnsiString ( unsigned char ); // ��������� ������ �� �������