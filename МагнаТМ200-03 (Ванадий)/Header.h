// ������������� ���������� ����������� ����������� ��������� �������

        // �������� �������� � ������ Zin-��
        TLabel    *Title_Zin[ZIN_COUNT][16];
        TImage *Check_Zin[ZIN_COUNT][16];

        // �������� �������� � ������ Out
        TLabel    *Title_Out[OUT_COUNT][16];
        TButton   *Btn_Out[OUT_COUNT][16];
        TImage *Check_Out[OUT_COUNT][16];

        // �������� �������� � ������ Aik
        TLabel  *Title_Ain[AIK_COUNT][8];
        TEdit   *Dec_Ain[AIK_COUNT][8];
        TEdit   *UV_Ain[AIK_COUNT][8];
        TCGauge *CG_Ain[AIK_COUNT][8];

        // �������� �������� � ������ Aout
        TLabel     *Title_Aout[AOUT_COUNT][4];
        TEdit      *Dec_Aout[AOUT_COUNT][4];
        TEdit      *UV_Aout[AOUT_COUNT][4];
        TCGauge    *CG_Aout[AOUT_COUNT][4];
        TEdit      *Dec_Aout_zad[AOUT_COUNT][4];
        TEdit      *UV_Aout_zad[AOUT_COUNT][4];
        TCGauge    *CG_Aout_zad[AOUT_COUNT][4];
        TButton    *Zero_Aout[AOUT_COUNT][4];
        TButton    *Ent_Aout[AOUT_COUNT][4];
        TScrollBar *Zad_Aout[AOUT_COUNT][4];

    // ���������� ���������� ���������� ��������� (���� ISO)
unsigned int
    externalTask = 0x0000,  // 0x0001 - ������� ���������� ������� ������� � ISO-P32C32
                            // 0x0002 - ������� ���������� ������� ������� � ISO-ACL7225
                            // 0x0004 - �������� ���������� �������� ������� � ISO-P32C32
                            // 0x0008 - �������� ���������� �������� ������� � ISO-ACL7225
                            // 0x0010 - ������� ���������� ������� ������� � ISO-813
                            // 0x0020 - �������� ���������� �������� ������� � ISO-DA16
                            // 0x0040 - ������� ���������� ������� � ISO-DA16
    externalError = 0;      // ��� ������ ��� ������ ���������� ��������� (���� ISO)

//------------------------------------------------------------------------------
const unsigned int
    porogDavlVodi = 4095 * ( 0.27 * 0.4 + 1 ) / 10; //����� 0.27���(�������� ������)

bool pr_otl = 0;                    // ������� ������ �������

//int PR_KLASTER = 0; 

float
    magnRes1,               // ������ 1 ���������� �������
    magnRes2,               // ������ 2 ���������� ������� 
    RRG1_MAX = 9.0,           // ������������ ������ ���1
    RRG1_MIN = 0.0,           // ����������� ������ ���1
    RRG2_MAX = 0.9,           // ������������ ������ ���2
    RRG2_MIN = 0.0,           // ����������� ������ ���2
    RRG3_MAX = 3.6,           // ������������ ������ ���3
    RRG3_MIN = 0.0,           // ����������� ������ ���3
    RRG4_MAX = 3.6,           // ������������ ������ ���4
    RRG4_MIN = 0.0,           // ����������� ������ ���4
    RRG5_MAX = 3.6,           // ������������ ������ ���5
    RRG5_MIN = 0.0,           // ����������� ������ ���5
    DAVL_MAX = 10.0,         // ������������ �������� 13.33
    DAVL_MAX_USER = 10.0,
    DAVL_MIN = 0.1,           // ����������� ��������
    CESAR_MAX_IP = 3000.0,    // ������������ �������� �������� CESAR ��
    CESAR_MIN_IP = 0.0,       // ����������� �������� �������� CESAR  ��
    US_MAX = 10.0,            // ������������ �������� ��������� ��
    US_MAX_USER = 9.5,        // ������������ �������� ��������� �� ��� ���������
    US_MIN_USER  = 0.5,       // ����������� �������� ��������� �� ��� ���������
    CESAR_MAX_PD = 600.0,    // ������������ �������� �������� CESAR ��
    CESAR_MIN_PD = 0.0,       // ����������� �������� �������� CESAR  ��
    SMESH_MIN_USER = 0, 
    SMESH_MAX_USER = 4000.0,  // ������������ �������� ��� �������
    MEX_MIN = -2147483647,    // ����������� ���� ��������
    MEX_MAX = 2147483647,     // ������������ ���� ��������
    TIME_MAX = 30.0,        // ������������ �����
    TIME_MIN = 0.0;           // ����������� �����


// ��������� �������� �����
unsigned char GK_n = 0;         // ����� ������ ����������� ����
char *loc_udb="Data\\GasNames.udb"; // ����� ����� ���������� ������ � ���������

struct IDUDBFILE
    {
        bool state[10];
        char Gas_names[10][8];
    };
unsigned char flagSBres = 0;        // ����� ���������� ��� ����� �������
IDUDBFILE GasNames;

//------------------------------------------------------------------------------
bool
    com1_err_alarm = 0,
    com2_err_alarm = 0,
    com3_err_alarm = 0,
    com4_err_alarm = 0,
        pr_yel = 0,
    firstSec = true,
    ust_ready = false,
    mnemoInAuto = true;
int
    zagolState = 0, // 0 - �����
                    // 1 - ��� 1 � �.�.
    libNmb = -1;    // ����� ��������� ������������ ���������

unsigned int  logicPerSecond = 0;
//------------------------------------------------------------------------------
float logic_time = 0.0;
bool anim_fase = 0; // ���� �������� ����������

#define SERIES_COUNT 8
// ������ ����������� ������� ��������
TLineSeries
    *serTemp[SERIES_COUNT],
    *serArh[SERIES_COUNT];

//------------------------------------------------------------------------------
// ���������� ��� ����������� �������

AnsiString pas_str,    // �������������� ������ �����
           Vpas_str,   // ��������� ������
           Vtekpas_str,   // ��������� ���. ������
           Vnew1pas_str,  // ��������� ����� 1 ������
           Vnew2pas_str;  // ��������� ����� 2 ������

char *loc_acc_udb="Data\\data2.udb"; // ����� ����� ���������� ������ � ���������

struct PAS_FILE
{
 char pass[30];
 bool state[10];
};

PAS_FILE iniPAS;
//------------------------------------------------------------------------------
