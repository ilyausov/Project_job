//---------------------------------------------------------------------------
//--ѕеременные необходимые дл€ обмена по RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// запросы:
	// 1  :aa060173ddddss\cr\lf		- запись уставки 1
	// 2  :aa060573ddddss\cr\lf		- запись уставки 2
	// 3  :aa060973ddddss\cr\lf		- запись уставки 3
	// 4  :aa060D73ddddss\cr\lf		- запись уставки 4
	// 5  :aa061173ddddss\cr\lf		- запись уставки 5
	// 6  :aa061573ddddss\cr\lf		- запись уставки 6
	// 7  :aa061973ddddss\cr\lf		- запись уставки 7
	// 8  :aa061D73ddddss\cr\lf		- запись уставки 8
	// 9  :aa062173ddddss\cr\lf		- запись уставки 9
	// 10 :aa062573ddddss\cr\lf		- запись уставки 10
	// 11 :aa062973ddddss\cr\lf		- запись уставки 11
	// 12 :aa062D73ddddss\cr\lf		- запись уставки 12
	// 13  :aa0300000004ss\cr\lf	- чтение текущих температур

	// ответы:
	// 1
	// ..
	// 12
	// 13  :aa0308ddd1ddd2ddd3ddd4ss\cr\lf
	
#define TRMD_CH_COUNT 12	// количество каналов Термодата

struct STRMD
{
	String adr;		// адрес устойства
		
	unsigned char
		Err,				// кол-во ошибок
		Max_err,			// максимум ошибок
		ACom,				// номер запроса
		Buf_len,			// длина запроса
		diagnS_byte,        // номер байта св€зной диагностики
		diagnS_mask;        // маска байта св€зной диагностики

	unsigned int
		*ZadTemp[TRMD_CH_COUNT],			// уставка 1
		//*ZadTemp2,			// уставка 2
		//*ZadTemp3,			// уставка 3
		//*ZadTemp4,			// уставка 4
		*TekTemp[TRMD_CH_COUNT];			// температура 1
		//*TekTemp2,			// температура 2
		//*TekTemp3,			// температура 3
		//*TekTemp4;			// температура 4
	bool
		*Pr_Sv[TRMD_CH_COUNT];

	SComport *SPort;		// указатель на порт устройства
		
	// элементы ручной страницы
	TTabSheet
		*TS_Pan;            // вкладка дл€ элемента
	String name;            // название дл€ отображени€

	TPanel
		*Pnl_Parent;
	
	TRadioButton			// элементы вывода приема/передачи
		*RB_prd,
		*RB_prm;	
	
	TGroupBox
		*GB_Main;
	
	TLabel
		*Lbl_Adr,
		*Lbl_Uni;			// создание всех надписей

	TEdit
		*Edt_Zad[TRMD_CH_COUNT],			// ячейки вывода заданий и температур
		//*Edt_Zad2,
		//*Edt_Zad3,
		//*Edt_Zad4,
		*Edt_Tek[TRMD_CH_COUNT];
		//*Edt_Tek2,
		//*Edt_Tek3,
		//*Edt_Tek4;

	void TRMD_Gen(); // создание страницы
	bool TRMD_Manage(unsigned int); // функци€ св€зи с датчиком
	void TRMD_FrmZap( );		// формирование запроса
	bool TRMD_ChkRep( );		// обработка ответа
	void __fastcall TRMD_SetZap(TObject *Sender); // ручной запрос
};

#define TRMD_COUNT 2
	
STRMD *TRMD[TRMD_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	
