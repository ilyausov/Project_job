//---------------------------------------------------------------------------
//--Переменные необходимые для обмена по RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// запросы: KOM_TMN
	// 1 	0x01 0x05 0x02 0x01	0x00 0x01 0xCC 0xCC - старт
	// 2 	0x01 0x05 0x02 0x01	0x00 0x10 0xCC 0xCC	- стоп
	// 3 	0x01 0x04 0x01 0x00	0x00 0x07 0xCC 0xCC	- опрос состояния 1
    // 4 	0x01 0x04 0x01 0x0D	0x00 0x01 0xCC 0xCC	- опрос состояния 2
	
	// ответы: OTVET_TMN (для VIDK)
	// 1  - старт
	// 2  - стоп
	// 3  - опрос состояния 1
    // 4  - опрос состояния 2

unsigned char KNOmsk_Req_Buf[5][6] =
{   0x01,0x02,0x03,0x04,0x05,0x06,
    0x01,0x05,0x02,0x01,0x00,0x01,
    0x01,0x05,0x02,0x01,0x00,0x10,
    0x01,0x04,0x01,0x00,0x00,0x07,
    0x01,0x04,0x01,0x0D,0x00,0x01};

struct S_KNOmsk
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
        pr_sost1,           // признаки ответов на отдельные запросы
        pr_sost2;           // признак связи выставляется только после ответа на оба

    bool
        *Pr_KNOmsk;         // признак полож. ответа
    unsigned char
        *Com_KNOmsk,        // команда задания
        *Otv_KNOmsk;     // ответ
    unsigned int
        *OtvM_KNOmsk[7];     // ответ состояний

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
		*Lbl_Uni;
		
	TEdit
		*Edt_Otv1,
        *Edt_Otv2,
		*Edt_Otv3,
        *Edt_Otv4;

    TCheckBox
		*CB_alarm1,
        *CB_alarm2,
        *CB_alarm3,
        *CB_alarm4,
        *CB_alarm5,
        *CB_alarm6,
        *CB_alarm7,
		*CB_alarm8;
		
	TButton
		*Btn_Zap1,
		*Btn_Zap2;

	void KNOmsk_Gen(); // создание страницы
	bool KNOmsk_Manage(unsigned int,bool); // функция связи с датчиком
	void KNOmsk_FrmZap(bool);		// формирование запроса
	bool KNOmsk_ChkRep(bool);		// обработка ответа
    unsigned short KNOmsk_GenCC(char*,unsigned int); // рассчет контр. суммы
    // bool KNOmsk_ChkCC(*char,*char); // проверка контрольной суммы
	void __fastcall KNOmsk_SetZap(TObject *Sender); // ручной запрос
};

#define KNOmsk_COUNT 1
	
S_KNOmsk *KNOmsk[KNOmsk_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	
