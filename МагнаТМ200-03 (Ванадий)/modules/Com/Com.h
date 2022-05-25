//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	#define PACKAGE_COUNT 128
	
	struct SComport
	{
		MSerial Port;		// �������� ����
		bool State,
            port_err,       // ������� ������ �� �����������
            port_ct,        // ������� ����� �� ���������������
            Pr_nal;
        String PortName;
        DWORD B_Rate;
		unsigned char P_Rate;	// ��������
		unsigned int
			PortTask, 		//
			DevState,  		//
			DevErr;			// 0 - ������ ���

		unsigned int
                Timer_Int,      // �������� ������ �������
			Dev_TK;			// ������������ ����� �������� ������
		unsigned int
			Dev_Timer;		// ������ �������� ������

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

	
	
	

	
	
	
	
	
	

	
	
	
