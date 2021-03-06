#ifndef MehanikaH
#define MehanikaH
//---------------------------------------------------------------------------
//--?????????? ????????--//
//---------------------------------------------------------------------------
float
    //zaslAngle360 = 258356.0,
    zaslAngle360 = 1033348.0,
    dzaslAngle360 = 108160.0,
    pderjAngle360 = 400.0;
//---------------------------------------------------------------------------
//--????????--//
//---------------------------------------------------------------------------
bool
    pr_op_zasl = false,     // ??????? ??????? ?????????

    zaslPrRuch = false,     // ??????? ??????? ???????????? ???????
                            // true (1) - ????????? ???????
                            // false (0) - ????????? ?? ????????
    zaslPrNeopr = true,     // ??????? ??????????????? ?????????
                            // ????????? ????????
                            // true (1) - ????????? ????????????
                            // false (0) - ????????? ??????????
    zaslPrIsh = false,      // ??????? ??????????? ?????????
                            // ?????????? ????????
                            // true (1) - ?????????
                            // false (0) - ?? ?????????
    zaslPrNahVIsh = false,  // ??????? ?????????? ????????? ? ???????? ?????????
                            // true (1) - ?????????
                            // false (0) - ?? ?????????
    zaslPrFront2 = false;   // ??????? ??????????? ????????? ??????? ?????? ???.
                            // true (1) - ???????
                            // false (0) - ?? ???????
unsigned char
    zaslStepNmb = 0,        // ??????? ??? ????????????? ?????
    zaslStepMaxNmb = 0;     // ???????????? ??? ????????????? ?????
unsigned int
    ctZaslOpros = 0,        // (??) ???????? ?? ????? ????????
    ctZaslDvij = 0,         // (???) ??????? ??????? ???????? ????????
    ctZaslErr = 0,          // (???) ??????? ??????? ??????????? ??????
    tkZaslOpros = 0,        // (??) 1 ?? ???????? ?? ????? ??? ????????
    tkZaslIzIsh = 100,      // (???) 2 ???, ?????. ??. ??????? ?????? ?? ???.
    tkZaslDvij = 12000,      // (???) 120 ???, ?????. ??????? ?? ??? ???????? ????.
    tkZaslRazgon = 150,     // (???) 3 ???, ?. ????? ??????? ?????????
    tkZaslBezDvij = 100;    // (???) 2 ???, ?. ??. ?? ????????? ????????? ???
long
    zaslCountFRP = 0,       // (???) ?????????? ????????? ??? ? 360 ????????
    zaslP1 = 0,             // (???) ???? ??????? ????????? ????????
    zaslP4 = zaslAngle360 / 360.0, // (???) 1 ?????? ???? ?????????? ????????? ????????
    zaslPutMin = zaslAngle360 * 5.0 / 360.0, // (???) 3 ??????? ??????????? ???? ????????
    zaslUgolPodhoda = 40,   // (???) ???? ??????? ? ?????????
    zaslPutZad = 0,         // (???) ??????? ????
    zaslPutTek = 0,         // (???) ???. ????. ??????????? ????, ??????? ????
    zaslUgolPred = 0,       // (???) ???????? ??????????? ????????? ???
    zaslUgolTek = 0,        // (???) ???????? ???????? ????????? ???
    zaslUgolAbs = 0,        // (???) ?????????? ???????? ???? ????????? ????.
#define TR_TEST_UGOL_COUNT 256
    zaslTrTestUgol[TR_TEST_UGOL_COUNT]; // (???) ???????? ????? ????????????? ?????
// ?????????? ????????????
void CountZaslFRP();        // ??????? ???-?? ????????? ? 360 ????????
void ZaslVibNapr();         // ????? ? ????????? ??????????? ?? ????????
void SetZaslPut(long, long);// ??????? ????????? ????
unsigned char ZaslDvijAnalis(); // ?????? ??????? ????? ? ???????? ????????
void SetZaslV(float vMin, float vMax); // ???????????? ???????? ? ?????? ????????
// ???????????? ???????? ????????? ????????
unsigned char GoZaslonka(long zaslUgol1, long zaslUgol2, bool izIsh, bool vIsh, bool cherezIsh);
/*
//---------------------------------------------------------------------------
//--?.????????--//
//---------------------------------------------------------------------------
bool
    dzaslPrRuch = false,    // ??????? ??????? ???????????? ???????
                            // true (1) - ????????? ???????
                            // false (0) - ????????? ?? ????????
    dzaslPrNeopr = true,    // ??????? ??????????????? ?????????
                            // ????????? ?.????????
                            // true (1) - ????????? ????????????
                            // false (0) - ????????? ??????????
    dzaslPrIsh = false,     // ??????? ??????????? ?????????
                            // ?????????? ?????. ????????
                            // true (1) - ?????????
                            // false (0) - ?? ?????????
    dzaslPrNahVIsh = false, // ??????? ?????????? ????????? ? ???????? ?????????
                            // true (1) - ?????????
                            // false (0) - ?? ?????????
    dzaslPrFront2 = false;  // ??????? ??????????? ?.????????? ??????? ?????? ???.
                            // true (1) - ???????
                            // false (0) - ?? ???????
unsigned int
    ctDzaslOpros = 0,       // (??) ???????? ?? ????? ????????
    ctDzaslDvij = 0,        // (???) ??????? ??????? ???????? ?.????????
    ctDzaslErr = 0,         // (???) ??????? ??????? ??????????? ??????
    tkDzaslOpros = 0,       // (??) 1 ?? ???????? ?? ????? ??? ?/????????
    tkDzaslIzIsh = 100,     // (???) 2 ???, ?????. ??. ??????? ?????? ?? ???.
    tkDzaslDvij = 7500,     // (???) 150 ???, ?????. ??????? ?? ??? ???????? ?.????.
    tkDzaslRazgon = 150,    // (???) 3 ???, ?. ????? ??????? ?????????
    tkDzaslBezDvij = 100;   // (???) 2 ???, ?. ??. ?? ????????? ????????? ???
long
    dzaslCountFRP = 0,      // (???) ?????????? ????????? ??? ? 360 ????????
    dzaslP1 = 0,            // (???) ???? ??????? ????????? ?.????????
    dzaslP4 = dzaslAngle360 / 360.0, // (???) 1 ?????? ???? ?????????? ????????? ?.????????
    dzaslPutMin = dzaslAngle360 * 3.0 / 360.0, // (???) 3 ??????? ??????????? ???? ????????
    dzaslUgolPodhoda = 40,  // (???) ???? ??????? ? ?????????
    dzaslPutZad = 0,        // (???) ??????? ????
    dzaslPutTek = 0,        // (???) ???. ????. ??????????? ????, ??????? ????
    dzaslUgolPred = 0,      // (???) ???????? ??????????? ????????? ???
    dzaslUgolTek = 0,       // (???) ???????? ???????? ????????? ???
    dzaslUgolAbs = 0;       // (???) ?????????? ???????? ???? ????????? ?.????.
// ?????????? ????????????
void CountDzaslFRP();       // ??????? ???-?? ????????? ? 360 ????????
void DzaslVibNapr(long, long); // ????? ? ????????? ??????????? ?? ?.????????
void SetDzaslPut(long, long); // ??????? ????????? ????
unsigned char DzaslDvijAnalis(); // ?????? ??????? ????? ? ???????? ?.????????
void SetDzaslV(float vMin, float vMax); // ???????????? ???????? ? ?????? ????????
// ???????????? ???????? ????????? ????????
unsigned char GoDZaslonka(long dzaslUgol1, long dzaslUgol2, bool izIsh, bool vIsh, bool cherezIsh);
*/
//---------------------------------------------------------------------------
//--?/?????????--//
//---------------------------------------------------------------------------
bool
    pderjPrRuch = false,    // ??????? ??????? ???????????? ???????
                            // true (1) - ????????? ???????
                            // false (0) - ????????? ?? ????????
    pderjPrNeopr = true,    // ??????? ??????????????? ?????????
                            // ????????? ?/?????????
                            // true (1) - ????????? ????????????
                            // false (0) - ????????? ??????????
    pderjPrIsh = false,     // ??????? ??????????? ?????????
                            // ?????????? ?/?????????
                            // true (1) - ?????????
                            // false (0) - ?? ?????????
    pderjPrNahVIsh = false, // ??????? ?????????? ????????? ? ???????? ?????????
                            // true (1) - ?????????
                            // false (0) - ?? ?????????
    pderjPrFront2 = false;  // ??????? ??????????? ?/????. ??????? ?????? ???.
                            // true (1) - ???????
                            // false (0) - ?? ???????
unsigned char
    pderjStepNmb = 0,       // ??????? ??? ????????????? ?????
    pderjStepMaxNmb = 0;    // ???????????? ??? ????????????? ?????
unsigned int
    ctPderjOpros = 0,       // (??) ???????? ?? ????? ?/?????????
    ctPderjDvij = 0,        // (???) ??????? ??????? ???????? ?/?????????
    ctPderjErr = 0,         // (???) ??????? ??????? ??????????? ??????
    tkPderjOpros = 0,       // (??) 1 ?? ???????? ?? ????? ??? ?/?????????
    tkPderjIzIsh = 100,     // (???) 2 ???, ?????. ??. ??????? ?????? ?? ???.
    tkPderjDvij = 7500,     // (???) 150 ???, ?????. ??????? ?? ??? ???????? ?/?????????
    tkPderjRazgon = 150,    // (???) 3 ???, ?. ????? ??????? ?????????
    tkPderjBezDvij = 100;   // (???) 2 ???, ?. ??. ?? ????????? ????????? ???
long
    pderjCountFRP = 0,      // (???) ?????????? ????????? ??? ? 360 ????????
    pderjP1 = 0,            // (???) ???? ??????? ????????? ?/?????????
    pderjP4 = pderjAngle360 / 360.0, // (???) 1 ?????? ???? ?????????? ????????? ?/?????????
    pderjPutMin = pderjAngle360 * 3.0 / 360.0 , // (???) 3 ??????? ??????????? ???? ????????
    pderjUgolPodhoda = 40,  // (???) ???? ??????? ? ?????????
    pderjPutZad = 0,        // (???) ??????? ????
    pderjPutTek = 0,        // (???) ???. ????. ??????????? ????, ??????? ????
    pderjUgolPred = 0,      // (???) ???????? ??????????? ????????? ???
    pderjUgolTek = 0,       // (???) ???????? ???????? ????????? ???
    pderjUgolAbs = 0;       // (???) ?????????? ???????? ???? ????????? ?/?????????
    pderjTrTestUgol[TR_TEST_UGOL_COUNT]; // (???) ???????? ????? ????????????? ?????
// ?????????? ????????????
void CountPderjFRP();       // ??????? ???-?? ????????? ? 360 ????????
void PderjVibNapr();        // ????? ? ????????? ??????????? ?? ?/?????????
void SetPderjPut(long, long); // ??????? ????????? ????
unsigned char PderjDvijAnalis(); // ?????? ??????? ????? ? ???????? ?/?????????
void SetPderjV(float vMin, float vMax); // ???????????? ???????? ? ?????? ????????
// ???????????? ???????? ????????? ?????????????????
unsigned char GoPderjatel(long pderjUgol1, long pderjUgol2, bool izIsh, bool vIsh, bool cherezIsh);
// ??? ???????? ???????? ???????? !!!
unsigned long
        putPderjSpd = 0;        // ?????????? ????????? ????
unsigned int
        pderjSpeed = 0;         // ???????? ???????? ????/???

unsigned int
    M1_N = 0,       // ??????? ??? ????? ?????????????
    M1_V = 0,
    VCHM_N = 0,
    VCHM_V = 0,
    M2_N = 0,
    M2_V = 0;
//---------------------------------------------------------------------------
extern
    unsigned int
        zin[],
        out[],
        nasmod[];
extern
    unsigned char
        sh_,
        norma,
        diagn[];
//---------------------------------------------------------------------------
extern void A_OUT(unsigned int Nmb, unsigned int Value);    // ??????? ??????????? ??????
extern void SetOut( bool, unsigned char, unsigned int);     // ?????????? ?????????? ????????
//---------------------------------------------------------------------------
#endif
