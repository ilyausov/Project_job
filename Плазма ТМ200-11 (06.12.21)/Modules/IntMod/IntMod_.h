//---------------------------------------------------------------------------
//--Переменные необходимые для обмена по RS-485--//
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
extern TForm1 *Form1;
	
	// команды
	// ">" "adr" "D" "D" "D" "D" "/cr" "/lf"

	// ответы
	// "<" "adr" "D" "D" "D" "D" "/cr" "/lf"
	
#define IMBits_COUNT 8
	
struct SIntMod
{
	unsigned char adr;		// адрес устойства

	unsigned char
		Err,				// кол-во ошибок
		Max_err,			// максимум ошибок
		Buf_len,			// длина запроса
		diagnS_byte,        // номер байта связной диагностики
		diagnS_mask;        // маска байта связной диагностики
		
	bool
		Type_Im;			// тип: 1-интерф./0 - модуль
			
	unsigned int			// переменные
		*Kom_IM,			// команда
		*Otv_IM;			// ответ       

	SComport *SPort;		// указатель на порт устройства
		
	// элементы ручной страницы
	TTabSheet
		*TS_Pan;            // вкладка для элемента
		
	TPanel
		*Pnl_Parent;
		
	TLabel
		*Lbl_Uni;
		
	TRadioButton			// элементы вывода приема/передачи
		*RB_prd,
		*RB_prm;
			
	TImage
		*Img_KomIM[IMBits_COUNT],		//
		*Img_OtvIM[IMBits_COUNT];		//

	unsigned char IM_manage( unsigned int );		// функция связи
	void IM_FrmZap( );								// формирование запроса
	bool IM_ChkRep( );								// обработка ответа
	void __fastcall IMSetKom(TObject *Sender); 		// контроль вводимых параметров
	void __fastcall IMSetOtv(TObject *Sender); 		// контроль вводимых параметров
};

    String IM_temp_str;
	char IM_char_tmp[6];
	unsigned int IM_KS = 0;
	String IM_tmp;
	char IM_str[2];

	void IM_Gen(unsigned char);						// создание страницы	
	void Visual_IM();
	void get_summ_IM(char*);
	bool chk_summ_IM(char*,unsigned char);

#define IntMod_COUNT 1
	
SIntMod *IntMod[IntMod_COUNT];
//------------------------------------------------------------------------------



	
	
	
	
