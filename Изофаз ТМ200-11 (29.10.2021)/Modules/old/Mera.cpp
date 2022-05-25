//---------------------------------------------------------------------------
#pragma hdrstop
#include "Mera.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//--Файл описания обменных команд для RS-485--//
//---------------------------------------------------------------------------

//-------------------------------//
//--Выделение ответа из посылки--//
//-------------------------------//
AnsiString FormStrMERA ( unsigned char Pack[],unsigned char len )
{
    AnsiString
        rezult = "";
    for(int i=0;i<len;i++)
	{
		// составляем строку ответа
		rezult += AnsiString(char(Pack[i]));
	}
    
    return rezult;
}
//------------------------------------//
//--Очищение буферов приёма/передачи--//
//------------------------------------//
void com2_PackageClear ()
{
        for (int i = 0 ; i < PACKAGE_COUNT ; i++)
        {
                com2_PackOut[i] = 0;
                com2_PackIn[i] = 0;
        }
}
//---------------------------------------------------------------------------
//--Расшифровка данных--//
//---------------------------------------------------------------------------
unsigned int raspakTRMD1()
{
    char tmp1[5] = {0};
	unsigned char i=0,j=0,k=0;

    for(i=4;(i<PACKAGE_COUNT)&&(k<12);i++)
    {
        if((com2_PackIn[i]!='_')&&(j<5))
        {
            tmp1[j] = com2_PackIn[i];
            j++;
        }
        else
        {
            ObjBPN[k]->tekBPN = int(10.0*atof(tmp1));
            k++;
            j = 0;
            tmp1[0] = 0;
            tmp1[1] = 0;
            tmp1[2] = 0;
            tmp1[3] = 0;
            tmp1[4] = 0;
        }
    }
}
//---------------------------------------------------------------------------
unsigned int raspakTRMD2()
{
    char tmp2[5] = {0};
	unsigned char i=0,j=0,k=0;

    for(i=4;(i<PACKAGE_COUNT)&&(k<10);i++)
    {
        if((com2_PackIn[i]!='_')&&(j<5))
        {
            tmp2[j] = com2_PackIn[i];
            j++;
        }
        else
        {
            ObjBPN[12+k]->tekBPN = int(10.0*atof(tmp2));
            k++;
            j = 0;
            tmp2[0] = 0;
            tmp2[1] = 0;
            tmp2[2] = 0;
            tmp2[3] = 0;
            tmp2[4] = 0;
        }
    }

    /*
    for(k=0;k<=9;k++)
    {
        j = 0;
        for(i=0;i<5;i++)
        {
			tmp[j] = com2_PackIn[4+i+5*k];
			j++;
        }

        ObjBPN[12+k]->tekBPN = int(10*atof(tmp));
    }*/
}
//---------------------------------------------------------------------------
//--Расшифровка давления--//
//---------------------------------------------------------------------------
double raspakMRD()
{
	char strB1[] = {0,0,0};
	char strB2[] = {0,0,0};
	char strB3[] = {0,0,0};
	char strB4[] = {0,0,0};

	strB1[0] =  com2_PackIn[11];
	strB1[1] =  com2_PackIn[12];
	strB2[0] =  com2_PackIn[13];
	strB2[1] =  com2_PackIn[14];
	strB3[0] =  com2_PackIn[7];
	strB3[1] =  com2_PackIn[8];
	strB4[0] =  com2_PackIn[9];
	strB4[1] =  com2_PackIn[10];

	int b1,b2,b3,b4;
	double f = 0;
	long Pa;
	int st;
	int m1 = 0,m2 = 0,m3 = 0;
	double p = 0,p0 = 0,p1 = 0,p2 = 0,p3 = 0,p4 = 0,p5 = 0,p6 = 0,p7 = 0;
	double       p8 = 0,p9 = 0,p10 = 0,p11 = 0,p12 = 0,p13 = 0,p14 = 0,p15 = 0;
	double       p16 = 0,p17 = 0,p18 = 0,p19 = 0,p20 = 0,p21 = 0,p22 = 0;
	char *string = "0000",*endptr;
	strcpy(string,strB1);
	b1=strtol(string,&endptr,16);

	strcpy(string,strB2);
	b2=strtol(string,&endptr,16);

	strcpy(string,strB3);
	b3=strtol(string,&endptr,16);

	strcpy(string,strB4);
	b4=strtol(string,&endptr,16);

	Pa = (b1*256) + b2;

	if(Pa)
	{
		Pa = Pa << 1;
		st = Pa/256;
		st = st - 127;

		m1 = (b2*256) + b3;
		m1 = m1 << 1;
   
		p = pow(2,st);
   
		if(m1 & 0x8000)
			{p22 = pow(2,st-1);}
		if(m1 & 0x4000)
			{p21 = pow(2,st-2);}
		if(m1 & 0x2000)
			{p20 = pow(2,st-3);}
		if(m1 & 0x1000)
			{p19 = pow(2,st-4);}
		if(m1 & 0x0800)
			{p18 = pow(2,st-5);}
		if(m1 & 0x0400)
			{p17 = pow(2,st-6);}
		if(m1 & 0x0200)
			{p16 = pow(2,st-7);}
		if(m1 & 0x0100)
			{p15 = pow(2,st-8);}
   
    	if(m2 & 0x0080)
			{p14 = pow(2,st-9);}
		if(m2 & 0x0040)
			{p13 = pow(2,st-10);}
		if(m2 & 0x0020)
			{p12 = pow(2,st-11);}
		if(m2 & 0x0010)
			{p11 = pow(2,st-12);}
		if(m2 & 0x0008)
			{p10 = pow(2,st-13);}
		if(m2 & 0x0004)
			{p9 = pow(2,st-14);}
		if(m2 & 0x0002)
			{p8 = pow(2,st-15);}
		if(m2 & 0x0001)
			{p7 = pow(2,st-16);}

		f = p + p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7+
		p8 + p9 + p10 + p11 + p12 + p13 + p14 + p15 + p16+
		p17 + p18 + p19 + p20 + p21 + p22;
	}
     	return(f);
}
//---------------------------------------------------------------------------
//-- Обмен с Термодатом --//
//---------------------------------------------------------------------------
int TRMD1( unsigned int SH , unsigned int Command, unsigned char NCh )
{
    FormMain -> RB_com2_4_prd -> Checked = !SH;
	FormMain -> RB_com2_4_prm -> Checked = SH;

    switch ( Command )
    {
        // запись уставки
        case 0:
        {
            switch ( SH )
            {
                case 0:
                {
                    // очистка массивов обмена
                    com2_PackageClear();
                    // формирование посылки "&04D00000\r"
                    com2_PackOut[0] = '&';
                    com2_PackOut[1] = '0';
                    com2_PackOut[2] = '4';
                    com2_PackOut[3] = 'D';
                    if(NCh < 10)
                        com2_PackOut[4] = NCh + 48; // 0 - 9
                    else
                        com2_PackOut[4] = NCh + 55; // A - F

                    for(int i=5;i<=9;i++ ) // очистка
					com2_PackOut[i]='0';

					int len;
					char str0[5];

					itoa(float(ObjBPN[NCh]->zadBPN)/10.0,str0,10);	//целая часть
					len=strlen(str0);				

					if((len>0)&&(len<=4))
					{

						for(int i=0;i<=len-1;i++)
						com2_PackOut[8-i] = str0[len-1-i];						
					}

                    com2_PackOut[9] = 0x0D;

					FormMain->Edit_com2_prd -> Text = FormStrMERA( com2_PackOut,10);

                    // очистка буфера приёма
                    Comport2.ResetRB();
                    // отправка посылки
                    Comport2.Write(com2_PackOut,10);
                    // переход на следующий шаг
                    com2_DevState++;
                    // обнуление времени
                    com2_Dev_Timer = 0;
                    return 0;
                }; break;
                
				case 1:
                {
                    // чтение ответа
                    Comport2.Read(com2_PackIn,Comport2.GetRBSize());
                    // преобразование ответа в сигналы
                    if ((	com2_PackIn[0] == '>' ) &&
						( 	com2_PackIn[1] == '0' ) &&
						( 	com2_PackIn[2] == '4' ))
                    {	
						// ответ верен
						FormMain->Edit_com2_prm -> Text = FormStrMERA( com2_PackIn,20 );
						
						ObjBPN[NCh]->prBPN = 1;    //задание отработано
						// переход к следующей задаче
						com2_DevState++;
						// сброс счетчика ошибок связи
						err_TRMD1 = 0;
                        return 0;
                    }
					else
					{
						if( com2_Dev_Timer < com2_Dev_TK) return 0; // ждем ответа
						// не дождались
						com2_DevState++; // переходим к следующему устройству
						// увеличение счетчика ошибок связи и проверяем на максимум
						if(( err_TRMD1++ ) > maxErr_TRMD1 )
							return 1;
						return 0;
					}
                }; break;
            }
        }; break;

		// Опрос температур
        case 1:
        {
            switch ( SH )
            {
                case 0:
                {
                    // очистка массивов обмена
                    com2_PackageClear();
                    // формирование посылки "&011\r"
                    com2_PackOut[0] = '&';
                    com2_PackOut[1] = '0';
                    com2_PackOut[2] = '4';
                    com2_PackOut[3] = '1';
                    com2_PackOut[4] = 0x0D;

					FormMain->Edit_com2_prd -> Text = FormStrMERA( com2_PackOut,5 );
					
                    // очистка буфера приёма
                    Comport2.ResetRB();
                    // отправка посылки
                    Comport2.Write(com2_PackOut,5);
                    // переход на следующий шаг
                    com2_DevState++;
                    // обнуление времени
                    com2_Dev_Timer = 0;
                    return 0;
                }; break;

                case 1:
                {
                    // чтение ответа
                    Comport2.Read(com2_PackIn,Comport2.GetRBSize());
                    // преобразование ответа в сигналы
                    if	(
						( com2_PackIn[0] == '>' ) &&
						( com2_PackIn[1] == '0' ) &&
						( com2_PackIn[2] == '4' )
                        )
                    {	// ответ верен
						FormMain->Edit_com2_prm -> Text = FormStrMERA( com2_PackIn,70 );

						raspakTRMD1();	// распаковка ТЕРМОДАТА

						// переход на следующий шаг
						com2_DevState++;
						// сброс счетчика ошибок связи
						err_TRMD1 = 0;
                        return 0;
                    }
					else
					{
						if(com2_Dev_Timer < com2_Dev_TK) return 0; // ждем ответа
						// не дождались
						com2_DevState++; // переходим к следующему устройству
						// увеличение счетчика ошибок связи и проверяем на максимум
						if(( err_TRMD1++ ) > maxErr_TRMD1 )
							return 1;
						return 0;
					}
                }; break;
            }
        }; break;
    }
}
//---------------------------------------------------------------------------
int TRMD2( unsigned int SH , unsigned int Command, unsigned char NCh )
{
    FormMain -> RB_com2_5_prd -> Checked = !SH;
	FormMain -> RB_com2_5_prm -> Checked = SH;

    switch ( Command )
    {
        // запись уставки
        case 0:
        {
            switch ( SH )
            {
                case 0:
                {
                    // очистка массивов обмена
                    com2_PackageClear();
                    // формирование посылки "&05D00000\r"
                    com2_PackOut[0] = '&';
                    com2_PackOut[1] = '0';
                    com2_PackOut[2] = '5';
                    com2_PackOut[3] = 'D';
                    if(NCh < 10)
                        com2_PackOut[4] = NCh + 48; // 0 - 9
                    else
                        com2_PackOut[4] = NCh + 55; // A - F

                    for(int i=5;i<=9;i++ ) // очистка
					com2_PackOut[i]='0';

					int len;
					char str0[5];

					itoa(float(ObjBPN[NCh+12]->zadBPN)/10.0,str0,10);	//целая часть
					len=strlen(str0);				

					if((len>0)&&(len<=4))
					{

						for(int i=0;i<=len-1;i++)
						com2_PackOut[8-i] = str0[len-1-i];						
					}

                    com2_PackOut[9] = 0x0D;

					FormMain->Edit_com2_prd -> Text = FormStrMERA( com2_PackOut,10);

                    // очистка буфера приёма
                    Comport2.ResetRB();
                    // отправка посылки
                    Comport2.Write(com2_PackOut,10);
                    // переход на следующий шаг
                    com2_DevState++;
                    // обнуление времени
                    com2_Dev_Timer = 0;
                    return 0;
                }; break;
                
				case 1:
                {
                    // чтение ответа
                    Comport2.Read(com2_PackIn,Comport2.GetRBSize());
                    // преобразование ответа в сигналы
                    if ((	com2_PackIn[0] == '>' ) &&
						( 	com2_PackIn[1] == '0' ) &&
						( 	com2_PackIn[2] == '5' ))
                    {	
						// ответ верен
						FormMain->Edit_com2_prm -> Text = FormStrMERA( com2_PackIn,20 );
						
						ObjBPN[NCh+12]->prBPN = 1;    //задание отработано
						// переход к следующей задаче
						com2_DevState++;
						// сброс счетчика ошибок связи
						err_TRMD2 = 0;
                        return 0;
                    }
					else
					{
						if( com2_Dev_Timer < com2_Dev_TK) return 0; // ждем ответа
						// не дождались
						com2_DevState++; // переходим к следующему устройству
						// увеличение счетчика ошибок связи и проверяем на максимум
						if(( err_TRMD2++ ) > maxErr_TRMD2 )
							return 1;
						return 0;
					}
                }; break;
            }
        }; break;

		// Опрос температур
        case 1:
        {
            switch ( SH )
            {
                case 0:
                {
                    // очистка массивов обмена
                    com2_PackageClear();
                    // формирование посылки "&051\r"
                    com2_PackOut[0] = '&';
                    com2_PackOut[1] = '0';
                    com2_PackOut[2] = '5';
                    com2_PackOut[3] = '1';
                    com2_PackOut[4] = 0x0D;

					FormMain->Edit_com2_prd -> Text = FormStrMERA( com2_PackOut,5 );
					
                    // очистка буфера приёма
                    Comport2.ResetRB();
                    // отправка посылки
                    Comport2.Write(com2_PackOut,5);
                    // переход на следующий шаг
                    com2_DevState++;
                    // обнуление времени
                    com2_Dev_Timer = 0;
                    return 0;
                }; break;

                case 1:
                {
                    // чтение ответа
                    Comport2.Read(com2_PackIn,Comport2.GetRBSize());
                    // преобразование ответа в сигналы
                    if	(
						( com2_PackIn[0] == '>' ) &&
						( com2_PackIn[1] == '0' ) &&
						( com2_PackIn[2] == '5' )
                        )
                    {	// ответ верен
						FormMain->Edit_com2_prm -> Text = FormStrMERA( com2_PackIn,70 );

						raspakTRMD2();	// распаковка ТЕРМОДАТА

						// переход на следующий шаг
						com2_DevState++;
						// сброс счетчика ошибок связи
						err_TRMD2 = 0;
                        return 0;
                    }
					else
					{
						if(com2_Dev_Timer < com2_Dev_TK) return 0; // ждем ответа
						// не дождались
						com2_DevState++; // переходим к следующему устройству
						// увеличение счетчика ошибок связи и проверяем на максимум
						if(( err_TRMD2++ ) > maxErr_TRMD2 )
							return 1;
						return 0;
					}
                }; break;
            }
        }; break;
    }
}
//---------------------------------------------------------------------------
//--Считать давление с Мерадата 1--//
//---------------------------------------------------------------------------
int Mera1 ( unsigned int SH , unsigned int Command )
{
	FormMain -> RB_com2_1_prd -> Checked = !SH;
	FormMain -> RB_com2_1_prm -> Checked = SH;
	
    switch ( Command )
    {
        // считать давление
        case 0:
        {
            switch ( SH )
            {
                case 0:
                {
                    // очистка массивов обмена
                    com2_PackageClear();
                    // формирование посылки ":01 03 0000 0002 fa \r\n"
                    com2_PackOut[0] = ':';
                    com2_PackOut[1] = '0';
                    com2_PackOut[2] = '1';
                    com2_PackOut[3] = '0';
                    com2_PackOut[4] = '3';
                    com2_PackOut[5] = '0';
					com2_PackOut[6] = '0';
					com2_PackOut[7] = '0';
					com2_PackOut[8] = '0';
					com2_PackOut[9] = '0';
					com2_PackOut[10] = '0';
					com2_PackOut[11] = '0';
					com2_PackOut[12] = '2';
					com2_PackOut[13] = 'f';
					com2_PackOut[14] = 'a';
					com2_PackOut[15] = 0x0D;
					com2_PackOut[16] = 0x0A;

					FormMain->Edit_com2_prd -> Text = FormStrMERA(com2_PackOut,15);
					
                    // очистка буфера приёма
                    Comport2.ResetRB();
                    // отправка посылки
                    Comport2.Write(com2_PackOut,17);
                    // переход на следующий шаг
                    com2_DevState++;
                    // обнуление времени
                    com2_Dev_Timer = 0;
                    return 0;
                }; break;
				
                case 1:
                {
                    // чтение ответа
                    Comport2.Read(com2_PackIn,Comport2.GetRBSize());
                    // преобразование ответа в сигналы
                    if	(
						( com2_PackIn[0] == ':' ) &&
						( com2_PackIn[1] == '0' ) &&
						( com2_PackIn[2] == '1' ) &&
						( com2_PackIn[15] )
                        )
                    {	
						// ответ верен
						FormMain->Edit_com2_prm -> Text = FormStrMERA( com2_PackIn,20 );

						// преобразование показаний в паскалях в милливольты
						double mera_result = raspakMRD();
						if(mera_result > 0.0)
							D_D1=int(1000*(log10(mera_result/133.3)+6.0));	//925
						
						// переход на следующий шаг
						com2_DevState++;
						// сброс счетчика ошибок связи
						err_MERA1 = 0;
                        return 0;
                    }
					else
					{
						if(com2_Dev_Timer < com2_Dev_TK) return 0; // ждем ответа
						// не дождались
						com2_DevState++; // переходим к следующему устройству
						// увеличение счетчика ошибок связи и проверяем на максимум
						if(( err_MERA1++ ) > maxErr_MERA1 )
							return 1;
						return 0;
					}
                }; break;
            }
        }; break;
    }

    return 0;
}
//---------------------------------------------------------------------------
//--Считать давление с Мерадата 2--//
//---------------------------------------------------------------------------
int Mera2 ( unsigned int SH , unsigned int Command )
{
	FormMain -> RB_com2_2_prd -> Checked = !SH;
	FormMain -> RB_com2_2_prm -> Checked = SH;
	
    switch ( Command )
    {
        // считать давление
        case 0:
        {
            switch ( SH )
            {
                case 0:
                {
                    // очистка массивов обмена
                    com2_PackageClear();
                    // формирование посылки ":02 03 0000 0002 f9 \r\n"
                    com2_PackOut[0] = ':';
                    com2_PackOut[1] = '0';
                    com2_PackOut[2] = '2';
                    com2_PackOut[3] = '0';
                    com2_PackOut[4] = '3';
                    com2_PackOut[5] = '0';
					com2_PackOut[6] = '0';
					com2_PackOut[7] = '0';
					com2_PackOut[8] = '0';
					com2_PackOut[9] = '0';
					com2_PackOut[10] = '0';
					com2_PackOut[11] = '0';
					com2_PackOut[12] = '2';
					com2_PackOut[13] = 'f';
					com2_PackOut[14] = '9';
					com2_PackOut[15] = 0x0D;
					com2_PackOut[16] = 0x0A;

					FormMain->Edit_com2_prd -> Text = FormStrMERA(com2_PackOut,15);
					
                    // очистка буфера приёма
                    Comport2.ResetRB();
                    // отправка посылки
                    Comport2.Write(com2_PackOut,17);
                    // переход на следующий шаг
                    com2_DevState++;
                    // обнуление времени
                    com2_Dev_Timer = 0;
                    return 0;
                }; break;
				
                case 1:
                {
                    // чтение ответа
                    Comport2.Read(com2_PackIn,Comport2.GetRBSize());
                    // преобразование ответа в сигналы
                    if	(
						( com2_PackIn[0] == ':' ) &&
						( com2_PackIn[1] == '0' ) &&
						( com2_PackIn[2] == '2' ) &&
						( com2_PackIn[15] )

                        )
                    {	
						// ответ верен
						FormMain->Edit_com2_prm -> Text = FormStrMERA( com2_PackIn,20 );

						// преобразование показаний в паскалях в милливольты
						double mera_result = raspakMRD();
						if(mera_result > 0.0)
							D_D2=int(1000*(log10(mera_result/133.3)+6.0));	//925
						
						// переход на следующий шаг
						com2_DevState++;
						// сброс счетчика ошибок связи
						err_MERA2 = 0;
                        return 0;
                    }
					else
					{
						if(com2_Dev_Timer < com2_Dev_TK) return 0; // ждем ответа
						// не дождались
						com2_DevState++; // переходим к следующему устройству
						// увеличение счетчика ошибок связи и проверяем на максимум
						if(( err_MERA2++ ) > maxErr_MERA2 )
							return 1;
						return 0;
					}
                }; break;
            }
        }; break;
    }

    return 0;
}
//---------------------------------------------------------------------------
//--Считать давление с Мерадата 3--//
//---------------------------------------------------------------------------
int Mera3 ( unsigned int SH , unsigned int Command )
{   /*
	FormMain -> RB_com2_3_prd -> Checked = !SH;
	FormMain -> RB_com2_3_prm -> Checked = SH;
	
    switch ( Command )
    {
        // считать давление
        case 0:
        {
            switch ( SH )
            {
                case 0:
                {
                    // очистка массивов обмена
                    com2_PackageClear();
                    // формирование посылки ":03 03 0000 0002 f8 \r\n"
                    com2_PackOut[0] = ':';
                    com2_PackOut[1] = '0';
                    com2_PackOut[2] = '3';
                    com2_PackOut[3] = '0';
                    com2_PackOut[4] = '3';
                    com2_PackOut[5] = '0';
					com2_PackOut[6] = '0';
					com2_PackOut[7] = '0';
					com2_PackOut[8] = '0';
					com2_PackOut[9] = '0';
					com2_PackOut[10] = '0';
					com2_PackOut[11] = '0';
					com2_PackOut[12] = '2';
					com2_PackOut[13] = 'f';
					com2_PackOut[14] = '8';
					com2_PackOut[15] = 0x0D;
					com2_PackOut[16] = 0x0A;

					FormMain->Edit_com2_prd -> Text = FormStrMERA(com2_PackOut,15);
					
                    // очистка буфера приёма
                    Comport2.ResetRB();
                    // отправка посылки
                    Comport2.Write(com2_PackOut,17);
                    // переход на следующий шаг
                    com2_DevState++;
                    // обнуление времени
                    com2_Dev_Timer = 0;
                    return 0;
                }; break;
				
                case 1:
                {
                    // чтение ответа
                    Comport2.Read(com2_PackIn,Comport2.GetRBSize());
                    // преобразование ответа в сигналы
                    if	(
						( com2_PackIn[0] == ':' ) &&
						( com2_PackIn[1] == '0' ) &&
						( com2_PackIn[2] == '3' ) &&
						( com2_PackIn[15] )

                        )
                    {	
						// ответ верен
						FormMain->Edit_com2_prm -> Text = FormStrMERA( com2_PackIn,20 );

						// преобразование показаний в паскалях в милливольты
						double mera_result = raspakMRD();
						if(mera_result > 0.0)
							D_D2=int(1000*(log10(mera_result/133.3)+6.0));	//925
						
						// переход на следующий шаг
						com2_DevState++;
						// сброс счетчика ошибок связи
						err_MERA3 = 0;
                        return 0;
                    }
					else
					{
						if(com2_Dev_Timer < com2_Dev_TK) return 0; // ждем ответа
						// не дождались
						com2_DevState++; // переходим к следующему устройству
						// увеличение счетчика ошибок связи и проверяем на максимум
						if(( err_MERA3++ ) > maxErr_MERA3 )
							return 1;
						return 0;
					}
                }; break;
            }
        }; break;
    }

    return 0;      */
}
//---------------------------------------------------------------------------