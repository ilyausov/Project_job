//---------------------------------------------------------------------------
//--Переменные необходимые для обмена по RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// запросы:
	// 1  001|00|740|02|=?|106|cr         - запрос давления

	// ответы:
	// 1  001|10|740|06|100023|025|cr	- 1000 mBar

struct SPPT200_dat
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
		*Pres_PPT200;			// давление

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

	void DatPPT200_Gen(); // создание страницы
	bool DatPPT200_Manage(unsigned int,bool); // функция связи с датчиком
	void DatPPT200_FrmZap(bool);		// формирование запроса
	bool DatPPT200_ChkRep(bool);		// обработка ответа
	void __fastcall DatPPT200_SetZap(TObject *Sender); // ручной запрос
};

#define DAT_PPT200_COUNT 2
	
SPPT200_dat *Dat_PPT200[DAT_PPT200_COUNT];