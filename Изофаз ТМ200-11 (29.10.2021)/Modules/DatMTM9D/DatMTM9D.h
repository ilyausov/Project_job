//---------------------------------------------------------------------------
//--Переменные необходимые для обмена по RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// запросы:
	// 1  001M^/cr         - запрос давления

	// ответы:
	// 1  001M260014K/cr	- 1000 mBar

struct SMTM9D_dat
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
		*Pres_DMTM9D;			// давление

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

	void DatMTM9D_Gen(); // создание страницы
	bool DatMTM9D_Manage(unsigned int,bool); // функция связи с датчиком
	void DatMTM9D_FrmZap(bool);		// формирование запроса
	bool DatMTM9D_ChkRep(bool);		// обработка ответа
	void __fastcall DatMTM9D_SetZap(TObject *Sender); // ручной запрос
};

#define DAT_MTM9D_COUNT 1
	
SMTM9D_dat *Dat_MTM9D[DAT_MTM9D_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	
