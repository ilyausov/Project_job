//---------------------------------------------------------------------------
//--Переменные необходимые для обмена по RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// запросы:
	// 1  001|00|740|02|=?|106|cr         - запрос давления

	// ответы:
	// 1  001|10|740|06|100023|025|cr	- 1000 mBar

struct SMPT200_dat
{
	String adr;		// адрес устойства
		
	unsigned char
		Err,				// кол-во ошибок
		Max_err,			// максимум ошибок
		ACom,				// текущий автоматический запрос
		RCom,				// текущий ручной апрос
		Buf_len,			// длина запроса
		diagnS_byte,        // номер байта связной диагностики
		diagnS_mask;        // маска байта связной диагностики

	unsigned int
		*Pres_MPT200;			// давление

	SComport *SPort;		// указатель на порт устройства
		
	// элементы ручной страницы
	TTabSheet
		*TS_Pan;            // вкладка для элемента
	String name;            // название для отображения

	TPanel
		*Pnl_Parent;
	
	TRadioButton			// элементы вывода приема/передачи
		*RB_prd,
		*RB_prm;	
	
	TGroupBox
		*GB_Main;
	
	TLabel
		*Lbl_Adr,
		*Lbl_Zap1;

	TEdit
		*Edt_Zap1;		

	void DatMPT200_Gen(); // создание страницы
	bool DatMPT200_Manage(unsigned int,bool); // функция связи с датчиком
	void DatMPT200_FrmZap(bool);		// формирование запроса
	bool DatMPT200_ChkRep(bool);		// обработка ответа
	void __fastcall DatMPT200_SetZap(TObject *Sender); // ручной запрос
};

#define DAT_MPT200_COUNT 1
	
SMPT200_dat *Dat_MPT200[DAT_MPT200_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	
