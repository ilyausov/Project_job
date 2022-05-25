//---------------------------------------------------------------------------
//--Переменные необходимые для обмена по RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// запросы:
	// 1  R:xxxxxx\cr\lf		- контроль позиции
	// 2  S:0xxxxxxx\cr\lf		- контроль давления
	// 3  O:\cr\lf				- открыть заслонку
	// 4  C:\cr\lf				- закрыть заслонку
	// 5  H:\cr\lf				- удержание
	// 6  L:00010000\cr\lf		- обучение
	// 7  i:76\cr\lf            - запрос состояния

	// ответы:
	// 1  R:\cr\lf				- контроль позиции
	// 2  S:\cr\lf				- контроль давления
	// 3  O:\cr\lf				- открыть заслонку
	// 4  C:\cr\lf				- закрыть заслонку
	// 5  H:\cr\lf				- удержание
	// 6  L:\cr\lf				- обучение
	// 7  i:76xxxxxxsyyyyyyyabc\cr\lf	- запрос состояния

struct SDZaslVAT
{

	unsigned char
		Err,				// кол-во ошибок
		Max_err,			// максимум ошибок
		ACom,				// текущий автоматический запрос
		RCom,				// текущий ручной апрос
		Buf_len,			// длина запроса
		diagnS_byte,        // номер байта связной диагностики
		diagnS_mask;        // маска байта связной диагностики

    bool
        *Pr_DZaslVAT;       // признак ответа

    unsigned char
        *ZadCom_DZaslVAT,   // команда задания
        *Otvet_DZaslVAT;    // ответ

	unsigned int
		*ZadData_DZaslVAT,  // задание
		*TekPos_DZaslVAT,   // текущая позиция
		*TekDat_DZaslVAT;   // показания датчика
	int
		*TekDavl_DZaslVAT;	// текущее давление

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
		*Lbl_Zap1,
		*Lbl_Zap2,
		*Lbl_Zap3,
		*Lbl_Zap4,
		*Lbl_Zap5,
		*Lbl_Zap6;

	TMaskEdit
		*Edt_Zap1,
		*Edt_Zap2;

	TEdit
		*Edt_Zap7_1,
		*Edt_Zap7_2;
		
	TButton
		*Btn_Zap1,
		*Btn_Zap2,
		*Btn_Zap3,
		*Btn_Zap4,
		*Btn_Zap5,
		*Btn_Zap6,
		*Btn_Zap7;

	void DZaslVAT_Gen(); // создание страницы
	bool DZaslVAT_Manage(unsigned int,bool); // функция связи с датчиком
	void DZaslVAT_FrmZap(bool);		// формирование запроса
	bool DZaslVAT_ChkRep(bool);		// обработка ответа
	void __fastcall DZaslVAT_SetZap(TObject *Sender); // ручной запрос
};

#define DZaslVAT_COUNT 1
	
SDZaslVAT *DZaslVAT[DZaslVAT_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	
