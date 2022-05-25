#ifndef DatMTP4DH
#define DatMTP4DH
//---------------------------------------------------------------------------
//--Переменные необходимые для обмена по RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// запросы:
	// 1  001M^/cr         - запрос давления

	// ответы:
	// 1  001M100023K/cr	- 

struct SMTP4D_dat
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
		*Pres_DMTP4D;			// давление

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

	void DatMTP4D_Gen(); // создание страницы
	bool DatMTP4D_Manage(unsigned int,bool); // функция связи с датчиком
	void DatMTP4D_FrmZap(bool);		// формирование запроса
	bool DatMTP4D_ChkRep(bool);		// обработка ответа
	void __fastcall DatMTP4D_SetZap(TObject *Sender); // ручной запрос
};

#define DAT_MTP4D_COUNT 1
	
SMTP4D_dat *Dat_MTP4D[DAT_MTP4D_COUNT];
#endif















