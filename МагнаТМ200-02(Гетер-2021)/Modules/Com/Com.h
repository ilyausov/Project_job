//---------------------------------------------------------------------------
//--Переменные необходимые для обмена по RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	#define PACKAGE_COUNT 128
	
	struct SComport
	{
		MSerial Port;		// открытый порт
		bool State,
            port_err,       // наличие ошибки по исключениям
            port_ct,        // счётчик паузы на переподключение
            Pr_nal;
        String PortName;
        DWORD B_Rate;
		unsigned char P_Rate;	// четность
		unsigned int
			PortTask, 		//
			DevState,  		//
			DevErr;			// 0 - ошибок нет

		unsigned int
                Timer_Int,      // интервал работы таймера
			Dev_TK;			// максимальное время ожидания ответа
		unsigned int
			Dev_Timer;		// таймер ожидания ответа

		unsigned char
			PackOut[PACKAGE_COUNT],
			PackIn[PACKAGE_COUNT];

		TLabel
			*LBL_name;		//
		TEdit
			*Edt_prd,		//
			*Edt_prm;		
		TRadioButton		//
			*RB_prd,
			*RB_prm;
        TCheckBox
			*CB_status,		//
			*CB_nal;		//
		TButton
			*BTN_reset,		//
			*BTN_nal;		//
		TTimer
			*Timer_Com;		//
		TLabel
			*LBL_otl;		//
		TPageControl
			*PC_Com;        //

		TPanel *PNL_parent; //

		void PackageClear();
        void ComTimers(); // инкремент счётчиков Com при ошибках
        //void VisPackIVE(bool,unsigned char);
        void VisPackRTU(bool,unsigned char);
        void VisPackASCII(bool);
		void ComPanGen(unsigned char port_num);
		void __fastcall Reser_Port(TObject *Sender);
        void __fastcall Com_Timer(TObject *Sender);
	};

	#define PORT_COUNT 6
	SComport *Comport[PORT_COUNT];
    MSerial MSCom[PORT_COUNT];

	
	
	

	
	
	
	
	
	

	
	
	
