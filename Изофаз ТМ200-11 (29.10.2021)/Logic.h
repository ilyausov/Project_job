//---------------------------------------------------------------------------
#ifndef LogicH
#define LogicH
//---------------------------------------------------------------------------
#include "Math.h"
//---------------------------------------------------------------------------
//--?????????????? ??????????--//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--????? ? ???????????--//
//---------------------------------------------------------------------------
#define DIAGN_COUNT   36
#define DIAGN_S_COUNT  3
#define ZIN_COUNT      5
#define OUT_COUNT      5
#define AIK_COUNT      3
#define A_OUT_COUNT    3
#define SHR_COUNT      85
#define NASMOD_COUNT   19
#define PAR_TRANS     6

unsigned char
	norma = 0,			// (???) ?????
	qkk = 0,			// (??M) ??? ???????
	diagn[DIAGN_COUNT],             // (???) ??????????? ?????, ????????? ????? main
	diagnOld[DIAGN_COUNT],          // ??????????? ????????
	diagnS[DIAGN_S_COUNT],          // (???) ??????????? ?????? ? ???????????? ?? RS
	diagnSOld[DIAGN_S_COUNT];       // ??????????? ?????? ? ???????????? ????????

//---------------------------------------------------------------------------
//--?????????? ?????-??????--//
//---------------------------------------------------------------------------
unsigned int
    out[OUT_COUNT]={0,0,0,0,0},
    zin[ZIN_COUNT]={0,0,0,0,0},	    // (???) ?????????? ?????, 0 - ?????????
    aik[AIK_COUNT * 8],         // (???) ?????????? ?????
    out_Z[ZIN_COUNT]={0,0,0,0}, // ??????????? ??? ???????????
    zin_Z[ZIN_COUNT]={0,0,0,0}, // ??????????? ??? ???????????
    aout[A_OUT_COUNT * 4] = {8192},
    aoutKon[A_OUT_COUNT * 4] = {8192},
//---------------------------------------------------------------------------
//--?????? ???????--//
//---------------------------------------------------------------------------
	// ??????? ??????? ??????? (?1,?2,?3,?4)
	D_D1 = 0, D_D2 = 0, D_D3 = 0, D_D4 = 0,
    UVAK_KAM = 4056,          // 8?? ??????? ?????? (972)
    POROG_DAVL = 6800,      // 100?? ????? ?????????? ???????? ? ?????? (MTM9D)
    UVAKV_KAM = 6142,       // 8?? ?? (MTM9D)
    UVAKN_KAM = 6439,       // 25?? ?? (MTM9D)
    UVAK_SHL_MO = 6574,     // 500?? ?? (???)
    UVAK_SHL_MN = 6574,     // 500?? ?? (???)
    UVAK_ZTMN = 5477,       // 40?? ?? (???)
    UATM_D1 = 8866,         // ????????? ?? (???)
    UATM_D4 = 8572;         // ????????? ?? (MTM9D)
    
//---------------------------------------------------------------------------
//--??????????? ? ??????????? ???????--//
//---------------------------------------------------------------------------
unsigned int	nasmod[NASMOD_COUNT] = {0};
/////////////////////////////////////////////////////////////////////////////////////////////////
#define PAR_ROW		52  // ???????????? ?????????? ????? ? ???????  0 ? 51 ?? ????????????!
#define PAR_COUNT 	21
#define PAR_NAGR	24
/////////////////////////////////////////////////////////////////////////////////////////////////
// ?????? ??????? ??????????
unsigned int par[PAR_ROW][PAR_COUNT] = {0};
// ?????? ?????????? ????? ??????????
unsigned int par_temp[PAR_ROW][PAR_COUNT] = {0};
// ?????? ?????????? ??? ???????
unsigned int par_n[PAR_NAGR] = {0};
// ?????? ??????????? ??? ?????????? ??????????????
long par_t[PAR_TRANS]            = {0};
//	??? ? ???? - par_t[1]
//	??? ? ??? - par_t[2]
//	??? ? ??? - par_t[3]
//	???? ?? ???(h) - par_t[4]
//	???? ?? ??? ???????.(h1) - par_t[5]
//---------------------------------------------------------------------------
//--??????????? ??????? ???????????--//
//---------------------------------------------------------------------------
// ??????? ? ???????? ???? ? ????????, ???? ??? ???????? ????????????
unsigned int

	// ??????? ????? ??? ?????????
	T_VHG			= 5,   // ?.????? ??????????? ?/? ???
	T_ZAD_DVS		= 25,  // (???)(0.5???.) ???????? ?? ???.??????
	T_PROC			= 0,   // ????? ???????????????? ????????
	T_KTMN_RAZGON	        = 180, // ?.????? ??????? ???
	T_KKAM_V		= 300, // 300 ???. ?????. ????? ?? ??????? ??????
	T_VODA			= 5,   // ???. ??????? ?? ?????????? ????
	T_STOP			= 25,  // (???) ????? ?? ???? ?????????? ??
	T_DVIJ			= 25,  // (???) ????? ?? ???????????? ???????. ?????????? ??
	T_KDVIJ_SU		= 3000,  // (???)(60 ???) ??????????? ????? ???????? ?? ????????????
	T_KSUT			= 30,  // (20?? ???.) 0,6 ???. ???????? ?? ?? ???????????? ????????
	T_KKAM			= 1200,
	T_KTMN			= 900,
	T_KSHL			= 150, // ?.????? ?????????? ?????
	T_KNAP			= 120, // ?.????? ??????? ? ????
	T_NAPUSK		= 5,
	 T_KSHL_MO       = 180,
	// ??????????? ???????? ???????
	CT_VHG = 0,
	CT_VODA_IP = 0,		        // ??????????? ???? ? ?????
	CT_PER = 0,
    CT_POV = 0,
    CT_KAS = 0,
	CT_TEMP = 0,		        // ??????? ??????? 

	CT_DVIJ_GIR_g = 0,		// ??????? ????????? ?? ????????(?????)
	CT_DVIJ_GIR_t = 0,		// ??????? ????????? ?? ????????(?????)
	CT_SUT_g = 0,			// ??????? ?? ???????????? ????.(?????)
	CT_SUT_t = 0,			// ??????? ?? ???????????? ????.(?????)
		
	// ???????? ??????
    CT_T1, 				// 1??
    CT_T20,     			// 20 ??
	
	// ??????????? ???????? ???????, ???
	CT_1,
	CT_2,
	CT_3,
	CT_4,
	CT_5,
	CT_6,
	CT_7,
	CT_9,
	CT_17,
	CT17K1,
	CT_29,
	CT29K1,
	CT_30T;

//---------------------------------------------------------------------------

//--??????--//
unsigned char
	otvet;	  // ?????????? ????? ?????????
	
//---------------------------------------------------------------------------
//--???? ? ??????????? ????????? ???????????--//
//---------------------------------------------------------------------------
unsigned char
	sh_   = 0,         		// ????? ?????????? ???????? ????
	// (???) ????????? ??? ?????? ?????????? ????
	shr[SHR_COUNT+1],
	// ????????? ??? ?????? ?????????? ???????
	sh[SHR_COUNT+1];
unsigned char
        zshr3;
unsigned char
		PR_TRTEST = 0,			// ??????? ????????? ????????????? ?????
		PR_OTK = 0,
		PR_FK_KAM = 0,
		PR_NASOS = 0,
		PR_NALADKA = 0,
		N_PL = 0,
		N_ST_MAX = 50,
		N_ST = 0,
        ZN_ST = 0;
unsigned int
        N_ZICL = 0;
//int     h = 792; //79167;
bool	PR_MAN = 0,				// ??????? ?????????? ???? ? ???????
        PR_KLASTER=1;
//---------------------------------------------------------------------------
//--??????????? ????????--//
//---------------------------------------------------------------------------
bool
	PR_DZASL = 0;		    		// ??????? ??????? ?????????????? ?????? ????????
unsigned char
	OTVET_DZASL = 0;
unsigned int
	CT_DZASL = 0,
	DAVL_DZASL = 0,
	DATA_DZASL = 0,		    		// ??????? ??????? ???????? (? ???)
	PAR_DZASL  = 0,	            	// ???????? ????????
	ZPAR_DZASL = 0;               	// ???. ????????? ???????? ????????
	int X_TDZASL;                 	// ??????? ???????? ????????
	unsigned char VRDZASL = 0;    	// 1 - ??????? ?????? ?? ????? ????????
	int E_TDZASL;		    		// ??????? ??????
	int DELDZASL;		    		// ?????? ??????? ??????
	unsigned int LIM1DZASL;	    	// ?????? ?????? ????????????? (???????)
	unsigned int LIM2DZASL;	    	// ?????? ?????? ????????????? (?????)
	unsigned int T_VRDZASL=10;   	// ??????????? ????? ?????? ?? ????? (???.)
	unsigned int T_KDZASL = 2;    	// ??????????? ????? ??????????? ??????(???.)
	int DOPDZASL = 10;	    		// ?????????? ??????????? 10%
	unsigned char KOM_DZASL = 0; 	//??? ??????? ????????
    int TEK_DAVL_DZASL;
    unsigned int TEK_POZ_DZASL;
//---------------------------------------------------------------------------
//--?? ? ??--//
//---------------------------------------------------------------------------
unsigned char
    prDvijGir_g = 0,              // ??????? ???????? ?? ????????: 0 - ??? ????????, 1 - ???? ????????
    prDvijGir_t = 0;
unsigned int
	DOP_SU = 5,                 // ?????? ???????? ? ?????
    T_SM_NAPR = 30,             //(???)(0.6???.)???????? ?? ????? ???????????
    DOP_DV_IP = 205;            // ?????? ???????? (???????) ?? 0,5?

bool
    klGir_gV = 0,             // ??????? ??????? ??????? "??????" ????????(?????)
    klGir_gN = 0,             // ??????? ??????? ??????? "?????"  ????????(?????)
    klGir_tV = 0,             // ??????? ??????? ??????? "??????" ????????(?????)
    klGir_tN = 0;             // ??????? ??????? ??????? "?????"  ????????(?????)
//----------------------------------------------------------------------------	
//--??? ????????--//
//---------------------------------------------------------------------------
    unsigned char
	    VRGIR = 0,          // 1 - ??????? ?????? ?? ????? ??? ????????
	    K_SOGL_GIR=67,      // ???????? ????-? ???????????? ??? ???????? (~15%)
	    NAPRS_GIR= 0; 	    // ??????????? ???????? ???????????? ??? ?????
                            // 0-??????(>); 1-????????(<)
    int
        X_TGIR,             // ??????? ???????? ???????? ????????
	    E_TGIR,		        // ??????? ??????
	    DELGIR,		        // ?????? ??????? ??????
        DOPGIR = 5,		    // ?????????? ???????????
	    PAR_GIR = 0;	    // ???????? ??? ?????
    unsigned int
	    N_TEK_GIR,    	    // ??????? ????-? ???????????? ???.??? ????????
        LIM1GIR,	        // ?????? ?????? ????????????? (???????)
	    LIM2GIR,	        // ?????? ?????? ????????????? (?????)
	    T_VRGIR=10,         // ??????????? ????? ?????? ?? ????? (???.)
	    T_KGIR = 2,         // ??????????? ????? ??????????? ??????(???.)
	    N_PRED_GIR;	        // ??????????? (??????????) ????-?
                            // ???????????? ?????????? ??? ????????
//---------------------------------------------------------------------------
//--????? ????? ??????????? ? ???????--//
//---------------------------------------------------------------------------
unsigned int
	OTVET_MOD,
	KOM_MOD;
//---------------------------------------------------------------------------
//--???????? ???????????--//
//---------------------------------------------------------------------------
	unsigned char
	KOM_PER = 0,	// ???????
	OTVET_PER = 0,	// ?????
	V_PER = 0,		// ????????	
	TYPE_PER = 0;	// ??? ????????
	
	bool
	PR_PER = 0,		// ??????? ?????????
	HOME_PER = 0;	// ??????? ???????? ? Home

	int
	PUT_PER = 0,	// ?????????? ????
	TEK_ABS_PER = 0,
	TEK_OTN_PER = 0;
//---------------------------------------------------------------------------
//--???????? ????????--//
//---------------------------------------------------------------------------
	unsigned char
	KOM_POV = 0,	// ???????
	OTVET_POV = 0,	// ?????
	V_POV = 0,		// ????????
	TYPE_POV = 0;	// ??? ????????
	
	bool
	PR_POV = 0,		// ??????? ?????????
	HOME_POV = 0;	// ??????? ???????? ? Home

	int
	PUT_POV = 0,	// ?????????? ????
	TEK_ABS_POV = 0,
	TEK_OTN_POV = 0;
//---------------------------------------------------------------------------
//--???????? ???????--//
//---------------------------------------------------------------------------
	unsigned char
	KOM_KAS = 0,	// ???????
	OTVET_KAS = 0,	// ?????
	V_KAS = 0,		// ????????
	TYPE_KAS = 0;	// ??? ????????
	
	bool
	PR_KAS = 0,		// ??????? ?????????
	HOME_KAS = 0;	// ??????? ???????? ? Home

	int
	PUT_KAS = 0,	// ?????????? ????
	TEK_ABS_KAS = 0,
	TEK_OTN_KAS = 0;
//------------------------------------------------------------------------------
//----------------?????????---------------------------------------------------
//------------------------------------------------------------------------------
void R_1();                  // ?????? 1 "??????? ??????"
void R_2();                  // ?????? 2 "??????? ?????"
void R_3();                  // ?????? 3 "??????? ????"
void R_4();                  // ?????? 4 "??????????????? ???????"
void R_5();                  // ?????? 5 "????? ??"
void R_6();                  // ?????? 6 "???? ???????"
void R_7();                  // ?????? 7 "?????????? ?????????"
void R_8();                  // ?????? 8 "????????? ?????????? ?????????"
void R_9();                  // ?????? 9 "???????????? ????"
void R_10();                 // ?????? 10 "??????? ??"
void R_11();                 // ?????? 11 "??????? ??"
void R_12();                 // ?????? 12 "???. ?????. ? ???"
void R_13();                 // ?????? 13 "???. ?????. ??????/?????"
void R_14();                 // ?????? 14 "???. ???. ? ???"
void R_15();                 // ?????? 15 "???. ???. ??????/?????"
void R_17();                 // ?????? 17 "??????????????? ??"
void R_18();                 // ?????? 18 "??????? ??"
void R_19();                 // ?????? 19 "??????? ??"
void R_20();                 // ?????? 20 "???1"
void R_21();                 // ?????? 21 "???2"
void R_22();                 // ?????? 22 "???3"
void SBROSR_27();            // ????? ??? ?/?
void R_28();                 // ?????? 28 "???. ??? ?????(????)"
void R_29();                 // ?????? 29 "???. ??? ??"
void SBROSR_29();            // ????? ??? ??
void R_23();                 // ?????? 23 "???4"
void R_24();                 // ?????? 24 "???5"
void R_25();                 // ?????? 25 "???6"
void R_26();                 // ?????? 26 "???7"
void R_29();                 // ?????? 29 "???. ??? ??"
void R_30();                 // ?????? 30 "????????. ??? ??(?????)"
void R_31();                 // ?????? 31 "????. ? ????? ??? ??(?????)"
void R_32();                 // ?????? 32 "????. ? ????? ??? ??(?????)"
void R_33();                 // ?????? 33 "???. ??????? ?????????"
void R_34();                 // ?????? 34 "????. ??????? ?????????"
void R_37();                 // ?????? 37 "???. ???. ? ???"
void R_38();                 // ?????? 38 "???. ???. ?????/????"
void R_40();				 // ?????? 40 "???. ?????? ?????? 1"
void R_41();				 // ?????? 41 "????. ??????? ?????? 1"
void R_42();				 // ?????? 42 "???. ?????? ?????? 2"
void R_43();				 // ?????? 43 "????. ?????? ?????? 2"
void R_44();				 // ?????? 44 "???. ?????? ?????? 3"
void R_45();				 // ?????? 45 "????. ?????? ?????? 3"
void R_46();				 // ?????? 46 "???. ?????? ????? 4"
void R_47();				 // ?????? 47 "????. ??????? ?????? 4"
void R_48();				 // ?????? 48 "???. ??????? ?????? 6"
void R_49();				 // ?????? 49 "????. ??????? ?????? 6"
void R_50();				 // ?????? 50 "???. ??????? ?????? 7"
void R_51();				 // ?????? 51 "????. ??????? ?????? 7"
void R_52();				 // ?????? 52 "???. ??????? ?????? 8"
void R_53();				 // ?????? 53 "????. ??????? ?????? 8"
void R_54();				 // ?????? 54 "???. ??????? ?????? 9"
void R_55();				 // ?????? 55 "????. ??????? ?????? 9"
void R_56();				 // ?????? 56 "???. ??????? ?????? 10"
void R_57();				 // ?????? 57 "????. ??????? ?????? 10"
void R_58();				 // ?????? 58 "???. ??????? ?????? 11"
void R_59();				 // ?????? 59 "????. ??????? ?????? 11"
void R_60();				 // ?????? 60 "???. ??????? ?????? 12"
void R_61();				 // ?????? 61 "????. ??????? ?????? 12"
void R_62();				 // ?????? 62 "???. ??????? ?????? 13"	
void R_63();				 // ?????? 63 "????. ??????? ?????? 13"
void R_64();				 // ?????? 64 "???. ??????? ?????? 14"
void R_65();				 // ?????? 65 "????. ??????? ?????? 14"
void R_66();				 // ?????? 66 "???. ??????? ?????? 15"
void R_67();				 // ?????? 67 "????. ??????? ?????? 15"
void R_68();				 // ?????? 68 "???. ??????? ?????? 16"
void R_69();				 // ?????? 69 "????. ??????? ?????? 16"
void R_70();				 // ?????? 70 "???. ??????? ?????? 17"
void R_71();				 // ?????? 71 "????. ??????? ?????? 17"
void R_72();				 // ?????? 72 "???. ??????? ?????? 18"
void R_73();				 // ?????? 73 "????. ??????? ?????? 18"
void R_74();				 // ?????? 74 "???. ??????? ?????? 19"
void R_75();				 // ?????? 75 "????. ??????? ?????? 19"
void R_76();				 // ?????? 76 "???. ??????? ?????? 20"
void R_77();				 // ?????? 77 "????. ??????? ????? 20"
void R_78();				 // ?????? 78 "???. ??????? ?????? 21"
void R_79();				 // ?????? 79 "????. ??????? ?????? 21"
void R_80();                 // ?????? 80 "???. ??????? ?????? 23"
void R_81();				 // ?????? 81 "????. ??????? ?????? 23"
void R_82();                 // ?????? 82 "???. ??????? ?????? 24"
void R_83();				 // ?????? 83 "????. ??????? ?????? 24"
void R_84();                 // ?????? 84 "???. ??????? ?????? 5"
void R_85();				 // ?????? 85 "????. ??????? ?????? 5"
//---------------------------------------------------------------------------
//--??????? ??????--//
//---------------------------------------------------------------------------
void KOLCO();   		// ???????????? ?????? (?????., ??????? ?? ??????)

void OSBROS();			// ????? ?????
void TIME ();			// ???????
void POST ();   		// ????? ? ?????????? ??????

void DIAGN_KOLCO();              	// ??????????? ? ??????
void AVAR_VODA_IP();   			 	// ??????? ?? ?????????? ???? ? ?/????
void AVAR_DAVL();					// "??????? ?? ??????? ???????? ? ?????? ??? ?????? ???"
void UPR_AVAR_OTKL();				// ?????????? ????????? ???????????
void UPR_R_30();					// ?????????? ??????? ????????.
void VID_DIAGN_GIR();            	// ?????? ??????????? ??? ???????????? ??? ??
void OKNSGIR_g();					// ????????? ?????? ????? ??? ??
void OKNSGIR_t();					// ????????? ?????? ????? ??? ??
void ZashDD();                   	// ???????????? ?????? ???????
void OpenFK_TMN_CloseFK_SHL();		// ????????????: ?????????????? ???????? ??-??? ? ???????? ??-??
void MEH_AVAR_CHK();				// ???????? ?? ??????? ??????????
void MEH_AVAR_SBROS();
void Vkl_FK_TMN();
void UPR();						// ??????/????????? ?????? (??????.)
void OPROS_SOST();				// "????? ? ?????? ?????????"
void PUSK_TP();					//???? ??
//---------------------------------------------------------------------------
//--??????? ?????????--//
//---------------------------------------------------------------------------
void VIBPAR_DZASL();		    // ???????????? "????? ????????? ??? ????????"
void PDDZASL();			    // ?????????? ?????????? ?????? ????????
void RLIMDZASL();	        	// ?????? LIM ??? ????????

void VIBPAR_GIR();	        // ???????????? "????? ????????? ??? ??? ??"
void RLIMGIR();		        // ???????????? "?????? LIM ??? ??? ??"
void PDGIR();			    // ?????????? ?????????? ?????? ??? ??
void RKOEF_GIR();           // ?????? ????????????

void VM_GIR();	// ???????????? "???. ???. ???. ???"
void OM_GIR();	// ???????????? "???. ???. ???. ???"

void UPR_Klapan(unsigned int);

void MP_V();				// ???????????? "?????? ?????"
void MP_N();				// ???????????? "?????? ????"
void VRR_GIR ();	        // ???????????? "????????? ????. ?????? ??? ??"
void ORR_GIR ();	        // ???????????? "????? ????. ?????? ??? ??"
bool KasVPaze();			// ???????????? ???????? ?????????? ??????? ????? ??????
//---------------------------------------------------------------------------
//--RS-???????--//
//---------------------------------------------------------------------------
void VIDK_DZASL(int,int,int,int);
void VIDK_PER(unsigned char,unsigned char,int,bool,unsigned int);
void VIDK_POV(unsigned char,unsigned char,int,bool,unsigned int);
void VIDK_KAS(unsigned char,unsigned char,int,bool,unsigned int);
//---------------------------------------------------------------------------
//--???????????? ???????--//
//---------------------------------------------------------------------------
void SBROS_MEH();							// ????? ????????
//int abs(int); // ???????????????? ??????? ??????
#define PER 0
#define POV 1
#define KAS 1
#define ABS 1
#define OTN 2
extern
    void A_OUT(unsigned int Nmb, unsigned int Value);   // ??????? ??????????? ??????
extern
    void SetOut(bool, unsigned char, unsigned int);     // ?????????? ?????????? ????????	
#endif

