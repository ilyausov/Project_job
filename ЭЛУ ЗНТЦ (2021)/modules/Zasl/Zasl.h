//---------------------------------------------------------------------------
//--???????? ??????????, ???????? ? ?????????? ????????? ???????--//
//---------------------------------------------------------------------------
// ????????? ???? "?????" ??? ????????? ?????????? ????????, ??????????
// ???????? ???????????: (1, 0x02) - ?????? ????, ?????? ???
struct SZaslPoint
{
    char
        x;
    int
        y;
};
// ????????? ????????, ??????????? ??????????? ????????? ? ?????????? ????????
// ??????: ???????, ??????????, ????????? ??????? ? ?.?.
struct SZasl
{
    char
        // ??? ??????? ? ??????????? ?? ???-?? ??????????? ???????? ? ????????
        // ???????? ?????, ????? ????: 11, 12, 21, 22
        // ????????: 21 - 2 ??????????? ???????, 1 ???????? ??????
        type;
    unsigned int
        // ?????
        // 0 - ??? ????? ????????? ???????
        // 1 - ??? ????? ?????????? ???????
        normaNmb[2],
        // ??????? ???????
        ctObject,
        // ?????? ??????????? ?????? ?????????? ???????? ???????? ???????
        // 0 - ??????????? ????? ????????? ???????
        // 1 - ??????????? ????? ?????????? ???????
        // 2 - ??????????? ????? ???????? ??????? ????? ?????????
        // 3 - ??????????? ????? ???????? ??????? ????? ??????????
        tkAction[4];
    SZaslPoint
        // ?????? ?????????? ???????
        // 0 - ?????? ?? ???????????? ?? ?????? "?????????"
        // 1 - ?????? ?? ???????????? ?? ?????? "??????????"
        // 2 - ????????? ??????? ????????????
        // 3 - ????????? ??????? ????????????
        diagnObject[4],
        // ?????? ??????????? ???????? ???????
        // 0 - ?????? ?? ????????
        // 1 - ?????? ? ????????
        zinIn[2],
        // ?????? ???????? ?????????? ????? ???????
        // 0 - ?????? ?? ????????
        // 1 - ?????? ?? ????????
        zinOut[2];
    // ??????? ????????????? ????????
    void Time();
};

//#define KLAPAN_COUNT 11
SZasl
	
    II,
    M1,
    M2;
	
    //*KlObjects[KLAPAN_COUNT];

// ??????? ?????????? ???????? ? ?????????? ???????-????????
void DoAction(bool action, SZasl *object);

// ??????? ?????????? ???????? ? 2 ?????????? ???????? ? 1 ??????
void DoAction21(bool action, SZasl *object);

// ??????? ?????????? ??????????
void TimeZasl();


extern
    unsigned char
        // ?????
        norma,
        // ??????????????? ??????
        sh_,
        // ???????????
        diagn[];


extern
    unsigned int
        // ?????????? ?????
        zin[],
        // ?????????? ??????
        out[];

extern
    void A_OUT(unsigned int channelNmb, unsigned int value);
    void SetOut (bool value, unsigned char byteNmb, unsigned int mask);

