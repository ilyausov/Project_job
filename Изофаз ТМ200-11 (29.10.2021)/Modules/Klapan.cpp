//---------------------------------------------------------------------------
//--???? ???????? ????????? ???????--//
//---------------------------------------------------------------------------
#include "Klapan.h"
//---------------------------------------------------------------------------
//--??????? ????????????? ???????? ???????--//
//---------------------------------------------------------------------------
void SKlapan::Time()
{
    ctObject++;
}
//---------------------------------------------------------------------------
//--??????? ?????????? ????????? ????? ?????????? ?????-??????--//
//---------------------------------------------------------------------------
void Klapan(bool action, SKlapan *object)
{
    switch ( object -> type )
    {
        case 11: DoAction11(action, object); break;
        case 12: DoAction12(action, object); break;
        case 21: DoAction21(action, object); break;
        case 22: DoAction22(action, object); break;
        // ???????? ??? ???????
        default: diagn[0] |= 0x01; break;
    }
}
//---------------------------------------------------------------------------
//--??????? ?????????? ???????? ? 1 ?????????? ??????? ? 1 ??????--//
//---------------------------------------------------------------------------
void DoAction11(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // ???? ?????? ????????? ????????? - ????????? ??????
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // ????????? ?????
                norma = object->normaNmb[(int)(!action)];
                // ????????? ????????????
                sh_ = 0;
            }
            else
            {
                // ?????????/????? ??????????? ?????????? ??????
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                // ???????? ????? ??????? ???????
                object->ctObject = 0;
                // ??????? ?? ????????? ???
                sh_++;
            }
        }; break;
        case 2:
        {
            // ???????? ??????? ??????????? ?? ??????
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // ????? ???????????
                diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
                // ???????? ????? ??????? ???????
                object->ctObject = 0;
                // ??????? ?? ????????? ???
                sh_++;
            }
            // ????? ????? ??????? ???????
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
                // ????????? ???????????
                diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
        }; break;
        case 3:
        {
            // ???????? ????? ?????????? ???????? ???????? ??? ????????
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // ????????? ?????
                norma = object->normaNmb[(int)(!action)];
                // ????????? ????????????
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--??????? ?????????? ???????? ? 1 ?????????? ??????? ? 2 ???????--//
//---------------------------------------------------------------------------
void DoAction12(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // ???? ?????? ????????? ????????? - ????????? ??????
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // ????????? ?????
                norma = object->normaNmb[(int)(!action)];
                // ????????? ????????????
                sh_ = 0;
            }
            else
            {
                // ?????????/????? ??????????? ?????????? ??????
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                // ???????? ????? ??????? ???????
                object->ctObject = 0;
                // ??????? ?? ????????? ???
                sh_++;
            }
        }; break;
        case 2:
        {
            // ????? ???????????
            diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
            diagn[object->diagnObject[2].x] &= (~object->diagnObject[2].y);
            diagn[object->diagnObject[3].x] &= (~object->diagnObject[3].y);
            // ???????? ??????? ??????????? ?? ??????
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // ???????? ????? ??????? ???????
                object->ctObject = 0;
                // ??????? ?? ????????? ???
                sh_++;
            }
            // ????? ????? ??????? ???????
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
            {
                // ????????? ??????? ????????????
                if (
                        ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 0 ) &&
                        ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 0 )
                    )
                // ????????? ???????????
                diagn[object->diagnObject[2].x] |= object->diagnObject[2].y;
                // ????????? ??????? ????????????
                else if (
                            ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 1 ) &&
                            ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 1 )
                        )
                    // ????????? ???????????
                    diagn[object->diagnObject[3].x] |= object->diagnObject[3].y;
                else
                    // ????????? ??????????? ?? ?????????? ???????
                    diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
            }
        }; break;
        case 3:
        {
            // ???????? ????? ?????????? ???????? ???????? ??? ????????
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // ????????? ?????
                norma = object->normaNmb[(int)(!action)];
                // ????????? ????????????
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--??????? ?????????? ???????? ? 2 ??????????? ???????? ? 1 ??????--//
//---------------------------------------------------------------------------
void DoAction21(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // ???? ?????? ????????? ????????? - ????????? ??????
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // ????????? ?????
                norma = object->normaNmb[(int)(!action)];
                // ????????? ????????????
                sh_ = 0;
            }
            else
            {
                // ?????????/????? ??????????? ?????????? ??????
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                SetOut(!action, object->zinOut[1].x, object->zinOut[1].y);
                // ???????? ????? ??????? ???????
                object->ctObject = 0;
                // ??????? ?? ????????? ???
                sh_++;
            }
        }; break;
        case 2:
        {
            // ???????? ??????? ??????????? ?? ??????
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // ????? ???????????
                diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
                // ???????? ????? ??????? ???????
                object->ctObject = 0;
                // ??????? ?? ????????? ???
                sh_++;
            }
            // ????? ????? ??????? ???????
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
                // ????????? ???????????
                diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
        }; break;
        case 3:
        {
            // ???????? ????? ?????????? ???????? ???????? ??? ????????
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // ????????? ?????
                norma = object->normaNmb[(int)(!action)];
                // ????????? ????????????
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--??????? ?????????? ???????? ? 2 ??????????? ???????? ? 2 ???????--//
//---------------------------------------------------------------------------
void DoAction22(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // ???? ?????? ????????? ????????? - ????????? ??????
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // ????????? ?????
                norma = object->normaNmb[(int)(!action)];
                // ????????? ????????????
                sh_ = 0;
            }
            else
            {
                // ?????????/????? ??????????? ?????????? ??????
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                SetOut(!action, object->zinOut[1].x, object->zinOut[1].y);
                // ???????? ????? ??????? ???????
                object->ctObject = 0;
                // ??????? ?? ????????? ???
                sh_++;
            }
        }; break;
        case 2:
        {
            // ????? ???????????
            diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
            diagn[object->diagnObject[2].x] &= (~object->diagnObject[2].y);
            diagn[object->diagnObject[3].x] &= (~object->diagnObject[3].y);
            // ???????? ??????? ??????????? ?? ??????
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // ???????? ????? ??????? ???????
                object->ctObject = 0;
                // ??????? ?? ????????? ???
                sh_++;
            }
            // ????? ????? ??????? ???????
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
            {
                // ????????? ??????? ????????????
                if (
                        ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 0 ) &&
                        ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 0 )
                    )
                // ????????? ???????????
                diagn[object->diagnObject[2].x] |= object->diagnObject[2].y;
                // ????????? ??????? ????????????
                else if (
                            ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 1 ) &&
                            ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 1 )
                        )
                    // ????????? ???????????
                    diagn[object->diagnObject[3].x] |= object->diagnObject[3].y;
                else
                    // ????????? ??????????? ?? ?????????? ???????
                    diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
            }
        }; break;
        case 3:
        {
            // ???????? ????? ?????????? ???????? ???????? ??? ????????
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // ????????? ?????
                norma = object->normaNmb[(int)(!action)];
                // ????????? ????????????
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--??????? ?????????? ?????????--//
//---------------------------------------------------------------------------
void TimeKlapan()
{	
	FvnKam.Time();
	FvnShl.Time();
	KlShl.Time();
	KlTmn.Time();
	KlKam.Time();
	KlD4.Time();
	KlD2.Time();
	ShZatvor.Time();
	DZasl.Time();
	PP.Time();
	ZIP.Time();
	Zatvor.Time();
}
//---------------------------------------------------------------------------
//--??????? ????????????? ???????? ? ?????????? ??????????? ? ?????????? ???????? ??????--//
//---------------------------------------------------------------------------
void InitObjectsKl()
{
	// ???????? ????????? ??????(?????)
	FvnKam.type = 11;					// ??? 1/1
	// ????? ???????
	FvnKam.normaNmb[0] = 37;				// ??? ????? ?????????
	FvnKam.normaNmb[1] = 38;				// ??? ????? ??????????
	// ??????????? ??????? ???????
	FvnKam.tkAction[0] = 2;					// ?.????? ????????
	FvnKam.tkAction[1] = 15;				// ?.????? ????????
	FvnKam.tkAction[2] = 30;					// ?.????? ???????? ????? ????????
	FvnKam.tkAction[3] = 2;					// ?.????? ???????? ????? ????????
	// ??????????? ???????
	FvnKam.diagnObject[0].x = 15;			// ????? ????? ??????????? "?? ????????"
	FvnKam.diagnObject[0].y = 0x10;			// ????? ???? ??????????? "?? ????????"
	FvnKam.diagnObject[1].x = 15;			// ????? ????? ??????????? "?? ????????"
	FvnKam.diagnObject[1].y = 0x20;			// ????? ???? ??????????? "?? ????????"
	FvnKam.diagnObject[2].x = 15;			// ????? ????? ??????????? "?? ??????????"
	FvnKam.diagnObject[2].y = 0x20;			// ????? ???? ??????????? "?? ??????????"
	FvnKam.diagnObject[3].x = 15;			// ????? ????? ??????????? "????????????"
	FvnKam.diagnObject[3].y = 0x20;			// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	FvnKam.zinOut[0].x = 4;					// ????? ????? ???. ????????? ??????? ?? "????????"
	FvnKam.zinOut[0].y = 0x10;				// ????? ???? ???. ????????? ???????  ?? "????????"
	FvnKam.zinOut[1].x = 4;					// ????? ????? ???. ????????? ??????? ?? "????????"
	FvnKam.zinOut[1].y = 0x10;				// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	FvnKam.zinIn[0].x = 2;					// ????? ????? ????????? ???????? ??????? ?? "????????"
	FvnKam.zinIn[0].y = 0x02;				// ????? ???? ????????? ???????? ??????? ?? "????????"
	FvnKam.zinIn[1].x = 2;					// ????? ????? ????????? ???????? ??????? ? "????????"
	FvnKam.zinIn[1].y = 0x02;				// ????? ???? ????????? ???????? ??????? ? "????????"

	// ???????? ????????? ?????
	FvnShl.type = 11;					// ??? 1/1
	// ????? ???????
	FvnShl.normaNmb[0] = 41;			// ??? ????? ?????????
	FvnShl.normaNmb[1] = 42;			// ??? ????? ??????????
	// ??????????? ??????? ???????
	FvnShl.tkAction[0] = 2;				// ?.????? ????????
	FvnShl.tkAction[1] = 2;				// ?.????? ????????
	FvnShl.tkAction[2] = 2;				// ?.????? ???????? ????? ????????
	FvnShl.tkAction[3] = 2;				// ?.????? ???????? ????? ????????
	// ??????????? ???????
	FvnShl.diagnObject[0].x = 15;		// ????? ????? ??????????? "?? ????????"
	FvnShl.diagnObject[0].y = 0x40;		// ????? ???? ??????????? "?? ????????"
	FvnShl.diagnObject[1].x = 15;		// ????? ????? ??????????? "?? ????????"
	FvnShl.diagnObject[1].y = 0x80;		// ????? ???? ??????????? "?? ????????"
	FvnShl.diagnObject[2].x = 15;		// ????? ????? ??????????? "?? ??????????"
	FvnShl.diagnObject[2].y = 0x80;		// ????? ???? ??????????? "?? ??????????"
	FvnShl.diagnObject[3].x = 15;		// ????? ????? ??????????? "????????????"
	FvnShl.diagnObject[3].y = 0x80;		// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	FvnShl.zinOut[0].x = 4;				// ????? ????? ???. ????????? ??????? ?? "????????"
	FvnShl.zinOut[0].y = 0x20;			// ????? ???? ???. ????????? ???????  ?? "????????"
	FvnShl.zinOut[1].x = 4;				// ????? ????? ???. ????????? ??????? ?? "????????"
	FvnShl.zinOut[1].y = 0x20;			// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	FvnShl.zinIn[0].x = 2;				// ????? ????? ????????? ???????? ??????? ?? "????????"
	FvnShl.zinIn[0].y = 0x01;			// ????? ???? ????????? ???????? ??????? ?? "????????"
	FvnShl.zinIn[1].x = 2;				// ????? ????? ????????? ???????? ??????? ? "????????"
	FvnShl.zinIn[1].y = 0x01;			// ????? ???? ????????? ???????? ??????? ? "????????"
	
	// ???????? ??-??
	KlShl.type = 12;					// ??? 1/2
	// ????? ???????
	KlShl.normaNmb[0] = 29;				// ??? ????? ?????????
	KlShl.normaNmb[1] = 30;				// ??? ????? ??????????
	// ??????????? ??????? ???????
	KlShl.tkAction[0] = 2;				// ?.????? ????????
	KlShl.tkAction[1] = 2;				// ?.????? ????????
	KlShl.tkAction[2] = 2;				// ?.????? ???????? ????? ????????
	KlShl.tkAction[3] = 2;				// ?.????? ???????? ????? ????????
	// ??????????? ???????
	KlShl.diagnObject[0].x = 10;		// ????? ????? ??????????? "?? ????????"
	KlShl.diagnObject[0].y = 0x01;		// ????? ???? ??????????? "?? ????????"
	KlShl.diagnObject[1].x = 10;		// ????? ????? ??????????? "?? ????????"
	KlShl.diagnObject[1].y = 0x02;		// ????? ???? ??????????? "?? ????????"
	KlShl.diagnObject[2].x = 10;		// ????? ????? ??????????? "?? ??????????"
	KlShl.diagnObject[2].y = 0x04;		// ????? ???? ??????????? "?? ??????????"
	KlShl.diagnObject[3].x = 10;		// ????? ????? ??????????? "????????????"
	KlShl.diagnObject[3].y = 0x08;		// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	KlShl.zinOut[0].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	KlShl.zinOut[0].y = 0x03;			// ????? ???? ???. ????????? ???????  ?? "????????"
	KlShl.zinOut[1].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	KlShl.zinOut[1].y = 0x03;			// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	KlShl.zinIn[0].x = 0;				// ????? ????? ????????? ???????? ??????? ?? "????????"
	KlShl.zinIn[0].y = 0x100;			// ????? ???? ????????? ???????? ??????? ?? "????????"
	KlShl.zinIn[1].x = 0;				// ????? ????? ????????? ???????? ??????? ? "????????"
	KlShl.zinIn[1].y = 0x200;			// ????? ???? ????????? ???????? ??????? ? "????????"	
	
	// ???????? ??-???
	KlTmn.type = 12;					// ??? 1/2
	// ????? ???????
	KlTmn.normaNmb[0] = 31;				// ??? ????? ?????????
	KlTmn.normaNmb[1] = 32;				// ??? ????? ??????????
	// ??????????? ??????? ???????
	KlTmn.tkAction[0] = 2;				// ?.????? ????????
	KlTmn.tkAction[1] = 2;				// ?.????? ????????
	KlTmn.tkAction[2] = 2;				// ?.????? ???????? ????? ????????
	KlTmn.tkAction[3] = 2;				// ?.????? ???????? ????? ????????
	// ??????????? ???????
	KlTmn.diagnObject[0].x = 9;			// ????? ????? ??????????? "?? ????????"
	KlTmn.diagnObject[0].y = 0x10;		// ????? ???? ??????????? "?? ????????"
	KlTmn.diagnObject[1].x = 9;			// ????? ????? ??????????? "?? ????????"
	KlTmn.diagnObject[1].y = 0x20;		// ????? ???? ??????????? "?? ????????"
	KlTmn.diagnObject[2].x = 9;			// ????? ????? ??????????? "?? ??????????"
	KlTmn.diagnObject[2].y = 0x40;		// ????? ???? ??????????? "?? ??????????"
	KlTmn.diagnObject[3].x = 9;			// ????? ????? ??????????? "????????????"
	KlTmn.diagnObject[3].y = 0x80;		// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	KlTmn.zinOut[0].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	KlTmn.zinOut[0].y = 0x04;			// ????? ???? ???. ????????? ???????  ?? "????????"
	KlTmn.zinOut[1].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	KlTmn.zinOut[1].y = 0x04;			// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	KlTmn.zinIn[0].x = 0;				// ????? ????? ????????? ???????? ??????? ?? "????????"
	KlTmn.zinIn[0].y = 0x1000;			// ????? ???? ????????? ???????? ??????? ?? "????????"
	KlTmn.zinIn[1].x = 0;				// ????? ????? ????????? ???????? ??????? ? "????????"
	KlTmn.zinIn[1].y = 0x2000;			// ????? ???? ????????? ???????? ??????? ? "????????"	
	
	// ???????? ??-???
	KlKam.type = 12;					// ??? 1/2
	// ????? ???????
	KlKam.normaNmb[0] = 27;				// ??? ????? ?????????
	KlKam.normaNmb[1] = 28;				// ??? ????? ??????????
	// ??????????? ??????? ???????
	KlKam.tkAction[0] = 2;				// ?.????? ????????
	KlKam.tkAction[1] = 2;				// ?.????? ????????
	KlKam.tkAction[2] = 2;				// ?.????? ???????? ????? ????????
	KlKam.tkAction[3] = 2;				// ?.????? ???????? ????? ????????
	// ??????????? ???????
	KlKam.diagnObject[0].x = 10;		// ????? ????? ??????????? "?? ????????"
	KlKam.diagnObject[0].y = 0x10;		// ????? ???? ??????????? "?? ????????"
	KlKam.diagnObject[1].x = 10;		// ????? ????? ??????????? "?? ????????"
	KlKam.diagnObject[1].y = 0x20;		// ????? ???? ??????????? "?? ????????"
	KlKam.diagnObject[2].x = 10;		// ????? ????? ??????????? "?? ??????????"
	KlKam.diagnObject[2].y = 0x40;		// ????? ???? ??????????? "?? ??????????"
	KlKam.diagnObject[3].x = 10;		// ????? ????? ??????????? "????????????"
	KlKam.diagnObject[3].y = 0x80;		// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	KlKam.zinOut[0].x = 4;				// ????? ????? ???. ????????? ??????? ?? "????????"
	KlKam.zinOut[0].y = 0x4000;			// ????? ???? ???. ????????? ???????  ?? "????????"
	KlKam.zinOut[1].x = 4;				// ????? ????? ???. ????????? ??????? ?? "????????"
	KlKam.zinOut[1].y = 0x4000;			// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	KlKam.zinIn[0].x = 0;				// ????? ????? ????????? ???????? ??????? ?? "????????"
	KlKam.zinIn[0].y = 0x400;			// ????? ???? ????????? ???????? ??????? ?? "????????"
	KlKam.zinIn[1].x = 0;				// ????? ????? ????????? ???????? ??????? ? "????????"
	KlKam.zinIn[1].y = 0x800;			// ????? ???? ????????? ???????? ??????? ? "????????"	
	
	// ???????? ??-?4
	KlD4.type = 12;						// ??? 1/2
	// ????? ???????
	KlD4.normaNmb[0] = 46;				// ??? ????? ?????????
	KlD4.normaNmb[1] = 47;				// ??? ????? ??????????
	// ??????????? ??????? ???????
	KlD4.tkAction[0] = 2;				// ?.????? ????????
	KlD4.tkAction[1] = 2;				// ?.????? ????????
	KlD4.tkAction[2] = 2;				// ?.????? ???????? ????? ????????
	KlD4.tkAction[3] = 2;				// ?.????? ???????? ????? ????????
	// ??????????? ???????
	KlD4.diagnObject[0].x = 6;			// ????? ????? ??????????? "?? ????????"
	KlD4.diagnObject[0].y = 0x01;		// ????? ???? ??????????? "?? ????????"
	KlD4.diagnObject[1].x = 6;			// ????? ????? ??????????? "?? ????????"
	KlD4.diagnObject[1].y = 0x02;		// ????? ???? ??????????? "?? ????????"
	KlD4.diagnObject[2].x = 6;			// ????? ????? ??????????? "?? ??????????"
	KlD4.diagnObject[2].y = 0x04;		// ????? ???? ??????????? "?? ??????????"
	KlD4.diagnObject[3].x = 6;			// ????? ????? ??????????? "????????????"
	KlD4.diagnObject[3].y = 0x08;		// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	KlD4.zinOut[0].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	KlD4.zinOut[0].y = 0x08;			// ????? ???? ???. ????????? ???????  ?? "????????"
	KlD4.zinOut[1].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	KlD4.zinOut[1].y = 0x08;			// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	KlD4.zinIn[0].x = 0;				// ????? ????? ????????? ???????? ??????? ?? "????????"
	KlD4.zinIn[0].y = 0x4000;			// ????? ???? ????????? ???????? ??????? ?? "????????"
	KlD4.zinIn[1].x = 0;				// ????? ????? ????????? ???????? ??????? ? "????????"
	KlD4.zinIn[1].y = 0x8000;			// ????? ???? ????????? ???????? ??????? ? "????????"	

	// ???????? ??-?2
	KlD2.type = 12;						// ??? 1/2
	// ????? ???????
	KlD2.normaNmb[0] = 48;				// ??? ????? ?????????
	KlD2.normaNmb[1] = 49;				// ??? ????? ??????????
	// ??????????? ??????? ???????
	KlD2.tkAction[0] = 2;				// ?.????? ????????
	KlD2.tkAction[1] = 2;				// ?.????? ????????
	KlD2.tkAction[2] = 2;				// ?.????? ???????? ????? ????????
	KlD2.tkAction[3] = 2;				// ?.????? ???????? ????? ????????
	// ??????????? ???????
	KlD2.diagnObject[0].x = 6;			// ????? ????? ??????????? "?? ????????"
	KlD2.diagnObject[0].y = 0x10;		// ????? ???? ??????????? "?? ????????"
	KlD2.diagnObject[1].x = 6;			// ????? ????? ??????????? "?? ????????"
	KlD2.diagnObject[1].y = 0x20;		// ????? ???? ??????????? "?? ????????"
	KlD2.diagnObject[2].x = 6;			// ????? ????? ??????????? "?? ??????????"
	KlD2.diagnObject[2].y = 0x40;		// ????? ???? ??????????? "?? ??????????"
	KlD2.diagnObject[3].x = 6;			// ????? ????? ??????????? "????????????"
	KlD2.diagnObject[3].y = 0x80;		// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	KlD2.zinOut[0].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	KlD2.zinOut[0].y = 0x10;			// ????? ???? ???. ????????? ???????  ?? "????????"
	KlD2.zinOut[1].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	KlD2.zinOut[1].y = 0x10;			// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	KlD2.zinIn[0].x = 1;				// ????? ????? ????????? ???????? ??????? ?? "????????"
	KlD2.zinIn[0].y = 0x01;				// ????? ???? ????????? ???????? ??????? ?? "????????"
	KlD2.zinIn[1].x = 1;				// ????? ????? ????????? ???????? ??????? ? "????????"
	KlD2.zinIn[1].y = 0x02;				// ????? ???? ????????? ???????? ??????? ? "????????"
	
	// ???????? ??
	ShZatvor.type = 22;					// ??? 2/2
	// ????? ???????
	ShZatvor.normaNmb[0] = 25;			// ??? ????? ?????????
	ShZatvor.normaNmb[1] = 26;			// ??? ????? ??????????
	// ??????????? ??????? ???????
	ShZatvor.tkAction[0] = 4;			// ?.????? ????????
	ShZatvor.tkAction[1] = 4;			// ?.????? ????????
	ShZatvor.tkAction[2] = 1;			// ?.????? ???????? ????? ????????
	ShZatvor.tkAction[3] = 1;			// ?.????? ???????? ????? ????????
	// ??????????? ???????
	ShZatvor.diagnObject[0].x = 9;		// ????? ????? ??????????? "?? ????????"
	ShZatvor.diagnObject[0].y = 0x01;	// ????? ???? ??????????? "?? ????????"
	ShZatvor.diagnObject[1].x = 9;		// ????? ????? ??????????? "?? ????????"
	ShZatvor.diagnObject[1].y = 0x02;	// ????? ???? ??????????? "?? ????????"
	ShZatvor.diagnObject[2].x = 9;		// ????? ????? ??????????? "?? ??????????"
	ShZatvor.diagnObject[2].y = 0x04;	// ????? ???? ??????????? "?? ??????????"
	ShZatvor.diagnObject[3].x = 9;		// ????? ????? ??????????? "????????????"
	ShZatvor.diagnObject[3].y = 0x08;	// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	ShZatvor.zinOut[0].x = 0;			// ????? ????? ???. ????????? ??????? ?? "????????"
	ShZatvor.zinOut[0].y = 0x20;		// ????? ???? ???. ????????? ???????  ?? "????????"
	ShZatvor.zinOut[1].x = 0;			// ????? ????? ???. ????????? ??????? ?? "????????"
	ShZatvor.zinOut[1].y = 0x40;		// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	ShZatvor.zinIn[0].x = 1;			// ????? ????? ????????? ???????? ??????? ?? "????????"
	ShZatvor.zinIn[0].y = 0x04;			// ????? ???? ????????? ???????? ??????? ?? "????????"
	ShZatvor.zinIn[1].x = 1;			// ????? ????? ????????? ???????? ??????? ? "????????"
	ShZatvor.zinIn[1].y = 0x08;			// ????? ???? ????????? ???????? ??????? ? "????????"	

	// ???????? ??
	DZasl.type = 22;					// ??? 2/2
	// ????? ???????
	DZasl.normaNmb[0] = 22;				// ??? ????? ?????????
	DZasl.normaNmb[1] = 23;				// ??? ????? ??????????
	// ??????????? ??????? ???????
	DZasl.tkAction[0] = 4;				// ?.????? ????????
	DZasl.tkAction[1] = 4;				// ?.????? ????????
	DZasl.tkAction[2] = 1;				// ?.????? ???????? ????? ????????
	DZasl.tkAction[3] = 1;				// ?.????? ???????? ????? ????????
	// ??????????? ???????
	DZasl.diagnObject[0].x = 2;			// ????? ????? ??????????? "?? ????????"
	DZasl.diagnObject[0].y = 0x10;		// ????? ???? ??????????? "?? ????????"
	DZasl.diagnObject[1].x = 2;			// ????? ????? ??????????? "?? ????????"
	DZasl.diagnObject[1].y = 0x20;		// ????? ???? ??????????? "?? ????????"
	DZasl.diagnObject[2].x = 2;			// ????? ????? ??????????? "?? ??????????"
	DZasl.diagnObject[2].y = 0x40;		// ????? ???? ??????????? "?? ??????????"
	DZasl.diagnObject[3].x = 2;			// ????? ????? ??????????? "????????????"
	DZasl.diagnObject[3].y = 0x80;		// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	DZasl.zinOut[0].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	DZasl.zinOut[0].y = 0x80;			// ????? ???? ???. ????????? ???????  ?? "????????"
	DZasl.zinOut[1].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	DZasl.zinOut[1].y = 0x100;			// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	DZasl.zinIn[0].x = 1;				// ????? ????? ????????? ???????? ??????? ?? "????????"
	DZasl.zinIn[0].y = 0x10;			// ????? ???? ????????? ???????? ??????? ?? "????????"
	DZasl.zinIn[1].x = 1;				// ????? ????? ????????? ???????? ??????? ? "????????"
	DZasl.zinIn[1].y = 0x20;			// ????? ???? ????????? ???????? ??????? ? "????????"
	
	// ???????? ??????????
	PP.type = 22;						// ??? 2/2
	// ????? ???????
	PP.normaNmb[0] = 35;				// ??? ????? ?????????
	PP.normaNmb[1] = 36;				// ??? ????? ??????????
	// ??????????? ??????? ???????
	PP.tkAction[0] = 10;				// ?.????? ????????
	PP.tkAction[1] = 10;				// ?.????? ????????
	PP.tkAction[2] = 1;				// ?.????? ???????? ????? ????????
	PP.tkAction[3] = 1;				// ?.????? ???????? ????? ????????
	// ??????????? ???????
	PP.diagnObject[0].x = 11;			// ????? ????? ??????????? "?? ????????"
	PP.diagnObject[0].y = 0x10;		// ????? ???? ??????????? "?? ????????"
	PP.diagnObject[1].x = 11;			// ????? ????? ??????????? "?? ????????"
	PP.diagnObject[1].y = 0x20;		// ????? ???? ??????????? "?? ????????"
	PP.diagnObject[2].x = 11;			// ????? ????? ??????????? "?? ??????????"
	PP.diagnObject[2].y = 0x40;		// ????? ???? ??????????? "?? ??????????"
	PP.diagnObject[3].x = 11;			// ????? ????? ??????????? "????????????"
	PP.diagnObject[3].y = 0x80;		// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	PP.zinOut[0].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	PP.zinOut[0].y = 0x200;			// ????? ???? ???. ????????? ???????  ?? "????????"
	PP.zinOut[1].x = 0;				// ????? ????? ???. ????????? ??????? ?? "????????"
	PP.zinOut[1].y = 0x400;			// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	PP.zinIn[0].x = 1;				// ????? ????? ????????? ???????? ??????? ?? "????????"
	PP.zinIn[0].y = 0x40;			// ????? ???? ????????? ???????? ??????? ?? "????????"
	PP.zinIn[1].x = 1;				// ????? ????? ????????? ???????? ??????? ? "????????"
	PP.zinIn[1].y = 0x80;			// ????? ???? ????????? ???????? ??????? ? "????????"
	
	// ???????? ?????? ??
	ZIP.type = 12;						// ??? 1/2
	// ????? ???????
	ZIP.normaNmb[0] = 100;				// ??? ????? ?????????
	ZIP.normaNmb[1] = 101;				// ??? ????? ??????????
	// ??????????? ??????? ???????
	ZIP.tkAction[0] = 10;				// ?.????? ????????
	ZIP.tkAction[1] = 10;				// ?.????? ????????
	ZIP.tkAction[2] = 1;				// ?.????? ???????? ????? ????????
	ZIP.tkAction[3] = 1;				// ?.????? ???????? ????? ????????
	// ??????????? ???????
	ZIP.diagnObject[0].x = 32;			// ????? ????? ??????????? "?? ????????"
	ZIP.diagnObject[0].y = 0x01;		// ????? ???? ??????????? "?? ????????"
	ZIP.diagnObject[1].x = 32;			// ????? ????? ??????????? "?? ????????"
	ZIP.diagnObject[1].y = 0x02;		// ????? ???? ??????????? "?? ????????"
	ZIP.diagnObject[2].x = 32;			// ????? ????? ??????????? "?? ??????????"
	ZIP.diagnObject[2].y = 0x04;		// ????? ???? ??????????? "?? ??????????"
	ZIP.diagnObject[3].x = 32;			// ????? ????? ??????????? "????????????"
	ZIP.diagnObject[3].y = 0x08;		// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	ZIP.zinOut[0].x = 4;				// ????? ????? ???. ????????? ??????? ?? "????????"
	ZIP.zinOut[0].y = 0x2000;			// ????? ???? ???. ????????? ???????  ?? "????????"
	ZIP.zinOut[1].x = 4;				// ????? ????? ???. ????????? ??????? ?? "????????"
	ZIP.zinOut[1].y = 0x2000;			// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	ZIP.zinIn[0].x = 1;					// ????? ????? ????????? ???????? ??????? ?? "????????"
	ZIP.zinIn[0].y = 0x400;				// ????? ???? ????????? ???????? ??????? ?? "????????"
	ZIP.zinIn[1].x = 1;					// ????? ????? ????????? ???????? ??????? ? "????????"
	ZIP.zinIn[1].y = 0x800;				// ????? ???? ????????? ???????? ??????? ? "????????"
	
	// ???????? ???????
	Zatvor.type = 12;					// ??? 1/2
	// ????? ???????
	Zatvor.normaNmb[0] = 102;			// ??? ????? ?????????
	Zatvor.normaNmb[1] = 103;			// ??? ????? ??????????
	// ??????????? ??????? ???????
	Zatvor.tkAction[0] = 10;			// ?.????? ????????
	Zatvor.tkAction[1] = 10;			// ?.????? ????????
	Zatvor.tkAction[2] = 1;				// ?.????? ???????? ????? ????????
	Zatvor.tkAction[3] = 1;				// ?.????? ???????? ????? ????????
	// ??????????? ???????
	Zatvor.diagnObject[0].x = 32;		// ????? ????? ??????????? "?? ????????"
	Zatvor.diagnObject[0].y = 0x10;		// ????? ???? ??????????? "?? ????????"
	Zatvor.diagnObject[1].x = 32;		// ????? ????? ??????????? "?? ????????"
	Zatvor.diagnObject[1].y = 0x20;		// ????? ???? ??????????? "?? ????????"
	Zatvor.diagnObject[2].x = 32;		// ????? ????? ??????????? "?? ??????????"
	Zatvor.diagnObject[2].y = 0x40;		// ????? ???? ??????????? "?? ??????????"
	Zatvor.diagnObject[3].x = 32;		// ????? ????? ??????????? "????????????"
	Zatvor.diagnObject[3].y = 0x80;		// ????? ???? ??????????? "????????????"
	// ?????????? ??????????? ??????
	Zatvor.zinOut[0].x = 4;				// ????? ????? ???. ????????? ??????? ?? "????????"
	Zatvor.zinOut[0].y = 0x8000;		// ????? ???? ???. ????????? ???????  ?? "????????"
	Zatvor.zinOut[1].x = 4;				// ????? ????? ???. ????????? ??????? ?? "????????"
	Zatvor.zinOut[1].y = 0x8000;		// ????? ???? ???. ????????? ??????? ?? "????????"
	// ?????????? ????? ???????? ?????
	Zatvor.zinIn[0].x = 1;				// ????? ????? ????????? ???????? ??????? ?? "????????"
	Zatvor.zinIn[0].y = 0x100;			// ????? ???? ????????? ???????? ??????? ?? "????????"
	Zatvor.zinIn[1].x = 1;				// ????? ????? ????????? ???????? ??????? ? "????????"
	Zatvor.zinIn[1].y = 0x200;			// ????? ???? ????????? ???????? ??????? ? "????????"
}