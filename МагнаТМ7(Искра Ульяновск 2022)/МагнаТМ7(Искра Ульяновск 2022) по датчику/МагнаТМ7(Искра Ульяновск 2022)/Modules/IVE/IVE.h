#ifndef IVEH
#define IVEH
//---------------------------------------------------------------------------
//--ѕеременные необходимые дл€ обмена по RS-485--//
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// запросы:
	// 1 - запись команд в 0x15 регистр 
	// 2 - запись характеристик в 0x01-0x04 регистры 
	// 3 - чтение регистров 0x01-0x16

	// ответы:


struct S_IVE
{
	unsigned char
		adr,				// адрес блока
		Err,				// кол-во ошибок
		Max_err,			// максимум ошибок
		ACom,				// текущий автоматический запрос
		RCom,				// текущий ручной апрос
		Buf_len,			// длина запроса
		diagnS_byte,        // номер байта св€зной диагностики
		diagnS_mask;        // маска байта св€зной диагностики
		
	unsigned char
		type_BU_IVE;       // тип блока 0-Ѕѕ»», 1-Ѕѕћ ...

    bool
		pr_zap[3],			// признаки успешности запросов
        *Pr_BU_IVE;       	// признак ответа

	unsigned int
		*Kom_BU_IVE[5],   	// задание на запись
		*Otv_BU_IVE[10];   	// считанные регистры

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
		*GB_Main,
        *GB_Err,
        *GB_Par;
	
	TLabel
		*Lbl_Uni;

	TEdit
		*Edt_Ent_Char[4],	// €чейки ввода
		*Edt_Zad_Char[4],	// €чейки дл€ вывода опорных
		*Edt_Tek_Char[4];	// €чейки дл€ вывода текущих
		
	TCheckBox
		*CB_Zad[9],
		*CB_Ent[9],
        *CB_Err[5];

	TButton
		*Btn_Zap1,
		*Btn_Zap2,
		*Btn_Zap3;

	void BU_IVE_Gen(); // создание страницы
	bool BU_IVE_Manage(unsigned int,bool); // функци€ св€зи
	void BU_IVE_FrmZap(bool);		// формирование запроса
	bool BU_IVE_ChkRep(bool);		// обработка ответа
	void __fastcall BU_IVE_SetZap(TObject *Sender); // ручной запрос
};

#define IVE_COUNT 2
	
S_IVE *BU_IVE[IVE_COUNT];
#endif

	
	
	
	
	
	
	
	
	
	

	
	
	
