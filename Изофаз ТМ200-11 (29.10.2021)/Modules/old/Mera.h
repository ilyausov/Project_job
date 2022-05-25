//---------------------------------------------------------------------------
//--Переменные необходимые для обмена по RS-485--//
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
	extern TFormMain *FormMain;

	unsigned char
	// количество ошибок связи подряд и предельного количества ошибок связи
	err_TRMD1 = 0, maxErr_TRMD1 = 5,
    err_TRMD2 = 0, maxErr_TRMD2 = 5,
	err_MERA1 = 0, maxErr_MERA1 = 5,
	err_MERA2 = 0, maxErr_MERA2 = 5,
	err_MERA3 = 0, maxErr_MERA3 = 5;
	
	// необъектные функции
	unsigned int raspakTRMD1();
    unsigned int raspakTRMD2();
	double raspakMRD();
	int TRMD1( unsigned int SH , unsigned int Command , unsigned char NCh); // обмен с Термодатом1
    int TRMD2( unsigned int SH , unsigned int Command , unsigned char NCh); // обмен с Термодатом2
	int Mera1( unsigned int SH , unsigned int Command ); // чтение давления с Мерадата 1
	int Mera2( unsigned int SH , unsigned int Command ); // чтение давления с Мерадата 2
	int Mera3( unsigned int SH , unsigned int Command ); // чтение давления с Мерадата 3
	
	// вспомогательные
	void com2_PackageClear();		// очистка массивов обмена
    AnsiString FormStrMERA( unsigned char,unsigned char); //формирование строки для отображение
