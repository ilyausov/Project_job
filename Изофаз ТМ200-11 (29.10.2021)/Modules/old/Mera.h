//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
	extern TFormMain *FormMain;

	unsigned char
	// ���������� ������ ����� ������ � ����������� ���������� ������ �����
	err_TRMD1 = 0, maxErr_TRMD1 = 5,
    err_TRMD2 = 0, maxErr_TRMD2 = 5,
	err_MERA1 = 0, maxErr_MERA1 = 5,
	err_MERA2 = 0, maxErr_MERA2 = 5,
	err_MERA3 = 0, maxErr_MERA3 = 5;
	
	// ����������� �������
	unsigned int raspakTRMD1();
    unsigned int raspakTRMD2();
	double raspakMRD();
	int TRMD1( unsigned int SH , unsigned int Command , unsigned char NCh); // ����� � ����������1
    int TRMD2( unsigned int SH , unsigned int Command , unsigned char NCh); // ����� � ����������2
	int Mera1( unsigned int SH , unsigned int Command ); // ������ �������� � �������� 1
	int Mera2( unsigned int SH , unsigned int Command ); // ������ �������� � �������� 2
	int Mera3( unsigned int SH , unsigned int Command ); // ������ �������� � �������� 3
	
	// ���������������
	void com2_PackageClear();		// ������� �������� ������
    AnsiString FormStrMERA( unsigned char,unsigned char); //������������ ������ ��� �����������
