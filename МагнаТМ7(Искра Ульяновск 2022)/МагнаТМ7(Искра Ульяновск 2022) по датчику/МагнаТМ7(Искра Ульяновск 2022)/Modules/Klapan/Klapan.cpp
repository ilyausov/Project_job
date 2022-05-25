//---------------------------------------------------------------------------
//--���� �������� ��������� �������--//
//---------------------------------------------------------------------------
#include "Klapan.h"
//---------------------------------------------------------------------------
//--������� ������������� �������� �������--//
//---------------------------------------------------------------------------
void SKlapan::Time()
{
    ctObject++;
}
//---------------------------------------------------------------------------
//--������� ���������� ��������� ����� ���������� �����-������--//
//---------------------------------------------------------------------------
void Klapan(bool action, SKlapan *object)
{
    switch ( object -> type )
    {
        case 11: DoAction11(action, object); break;
        case 12: DoAction12(action, object); break;
        case 21: DoAction21(action, object); break;
        case 22: DoAction22(action, object); break;
        // �������� ��� �������
        default: diagn[0] |= 0x01; break;
    }
}
//---------------------------------------------------------------------------
//--������� ���������� �������� � 1 ���������� ������� � 1 ������--//
//---------------------------------------------------------------------------
void DoAction11(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // ���� ������ ��������� ��������� - ��������� ������
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // ���������/����� ����������� ���������� ������
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                // ��������� �����
                norma = object->normaNmb[(int)(!action)];
                // ��������� ������������
                sh_ = 0;
            }
            else
            {
                // ���������/����� ����������� ���������� ������
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                // �������� ����� ������� �������
                object->ctObject = 0;
                // ������� �� ��������� ���
                sh_++;
            }
        }; break;
        case 2:
        {
            // �������� ������� ����������� �� ������
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // ����� �����������
                diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
                // �������� ����� ������� �������
                object->ctObject = 0;
                // ������� �� ��������� ���
                sh_++;
            }
            // ����� ����� ������� �������
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
                // ��������� �����������
                diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
        }; break;
        case 3:
        {
            // �������� ����� ���������� �������� �������� ��� ��������
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // ��������� �����
                norma = object->normaNmb[(int)(!action)];
                // ��������� ������������
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--������� ���������� �������� � 1 ���������� ������� � 2 �������--//
//---------------------------------------------------------------------------
void DoAction12(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // ���� ������ ��������� ��������� - ��������� ������
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // ���������/����� ����������� ���������� ������
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                // ��������� �����
                norma = object->normaNmb[(int)(!action)];
                // ��������� ������������
                sh_ = 0;
            }
            else
            {
                // ���������/����� ����������� ���������� ������
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                // �������� ����� ������� �������
                object->ctObject = 0;
                // ������� �� ��������� ���
                sh_++;
            }
        }; break;
        case 2:
        {
            // ����� �����������
            diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
            diagn[object->diagnObject[2].x] &= (~object->diagnObject[2].y);
            diagn[object->diagnObject[3].x] &= (~object->diagnObject[3].y);
            // �������� ������� ����������� �� ������
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // �������� ����� ������� �������
                object->ctObject = 0;
                // ������� �� ��������� ���
                sh_++;
            }
            // ����� ����� ������� �������
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
            {
                // ��������� ������� ������������
                if (
                        ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 0 ) &&
                        ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 0 )
                    )
                // ��������� �����������
                diagn[object->diagnObject[2].x] |= object->diagnObject[2].y;
                // ��������� ������� ������������
                else if (
                            ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 1 ) &&
                            ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 1 )
                        )
                    // ��������� �����������
                    diagn[object->diagnObject[3].x] |= object->diagnObject[3].y;
                else
                    // ��������� ����������� �� ���������� �������
                    diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
            }
        }; break;
        case 3:
        {
            // �������� ����� ���������� �������� �������� ��� ��������
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // ��������� �����
                norma = object->normaNmb[(int)(!action)];
                // ��������� ������������
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--������� ���������� �������� � 2 ����������� �������� � 1 ������--//
//---------------------------------------------------------------------------
void DoAction21(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // ���� ������ ��������� ��������� - ��������� ������
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // ���������/����� ����������� ���������� ������
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                SetOut(!action, object->zinOut[1].x, object->zinOut[1].y);
                // ��������� �����
                norma = object->normaNmb[(int)(!action)];
                // ��������� ������������
                sh_ = 0;
            }
            else
            {
                // ���������/����� ����������� ���������� ������
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                SetOut(!action, object->zinOut[1].x, object->zinOut[1].y);
                // �������� ����� ������� �������
                object->ctObject = 0;
                // ������� �� ��������� ���
                sh_++;
            }
        }; break;
        case 2:
        {
            // �������� ������� ����������� �� ������
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // ����� �����������
                diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
                // �������� ����� ������� �������
                object->ctObject = 0;
                // ������� �� ��������� ���
                sh_++;
            }
            // ����� ����� ������� �������
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
                // ��������� �����������
                diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
        }; break;
        case 3:
        {
            // �������� ����� ���������� �������� �������� ��� ��������
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // ��������� �����
                norma = object->normaNmb[(int)(!action)];
                // ��������� ������������
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--������� ���������� �������� � 2 ����������� �������� � 2 �������--//
//---------------------------------------------------------------------------
void DoAction22(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // ���� ������ ��������� ��������� - ��������� ������
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // ���������/����� ����������� ���������� ������
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                SetOut(!action, object->zinOut[1].x, object->zinOut[1].y);
                // ��������� �����
                norma = object->normaNmb[(int)(!action)];
                // ��������� ������������
                sh_ = 0;
            }
            else
            {
                // ���������/����� ����������� ���������� ������
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                SetOut(!action, object->zinOut[1].x, object->zinOut[1].y);
                // �������� ����� ������� �������
                object->ctObject = 0;
                // ������� �� ��������� ���
                sh_++;
            }
        }; break;
        case 2:
        {
            // ����� �����������
            diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
            diagn[object->diagnObject[2].x] &= (~object->diagnObject[2].y);
            diagn[object->diagnObject[3].x] &= (~object->diagnObject[3].y);
            // �������� ������� ����������� �� ������
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // �������� ����� ������� �������
                object->ctObject = 0;
                // ������� �� ��������� ���
                sh_++;
            }
            // ����� ����� ������� �������
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
            {
                // ��������� ������� ������������
                if (
                        ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 0 ) &&
                        ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 0 )
                    )
                // ��������� �����������
                diagn[object->diagnObject[2].x] |= object->diagnObject[2].y;
                // ��������� ������� ������������
                else if (
                            ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 1 ) &&
                            ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 1 )
                        )
                    // ��������� �����������
                    diagn[object->diagnObject[3].x] |= object->diagnObject[3].y;
                else
                    // ��������� ����������� �� ���������� �������
                    diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
            }
        }; break;
        case 3:
        {
            // �������� ����� ���������� �������� �������� ��� ��������
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // ��������� �����
                norma = object->normaNmb[(int)(!action)];
                // ��������� ������������
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--������� ���������� ���������--//
//---------------------------------------------------------------------------
void TimeKlapan()
{
    KlKn.Time();
    KlKam.Time();
    Fvn.Time();
	
}
//---------------------------------------------------------------------------
//--������� ������������� �������� � ���������� ����������� � ���������� �������� ������--//
//---------------------------------------------------------------------------
void InitObjectsKl()
{
	// �������� ������� ��-��
	KlKn.type = 12;					// ��-��
	// ����� �������
	KlKn.normaNmb[0] = 36;				// ��� ����� ���������
	KlKn.normaNmb[1] = 37;				// ��� ����� ����������
	// ����������� ������� �������
	KlKn.tkAction[0] = 2;				// �.����� ��������
	KlKn.tkAction[1] = 2;				// �.����� ��������
	KlKn.tkAction[2] = 2;				// �.����� �������� ����� ��������
	KlKn.tkAction[3] = 2;				// �.����� �������� ����� ��������
	// ����������� �������
	KlKn.diagnObject[0].x = 12;		// ����� ����� ����������� "�� ��������"
	KlKn.diagnObject[0].y = 0x1;		// ����� ���� ����������� "�� ��������"
	KlKn.diagnObject[1].x = 12;		// ����� ����� ����������� "�� ��������"
	KlKn.diagnObject[1].y = 0x2;		// ����� ���� ����������� "�� ��������"
	KlKn.diagnObject[2].x = 12;		// ����� ����� ����������� "�� ����������"
	KlKn.diagnObject[2].y = 0x4;		// ����� ���� ����������� "�� ����������"
	KlKn.diagnObject[3].x = 12;		// ����� ����� ����������� "������������"
	KlKn.diagnObject[3].y = 0x8;		// ����� ���� ����������� "������������"
	// ���������� ����������� ������
	KlKn.zinOut[0].x = 0;				// ����� ����� ���. ��������� ������� �� "��������"
	KlKn.zinOut[0].y = 0x20;			// ����� ���� ���. ��������� �������  �� "��������"
	KlKn.zinOut[1].x = 0;				// ����� ����� ���. ��������� ������� �� "��������"
	KlKn.zinOut[1].y = 0x20;			// ����� ���� ���. ��������� ������� �� "��������"
	// ���������� ����� �������� �����
	KlKn.zinIn[0].x = 0;				// ����� ����� ��������� �������� ������� �� "��������"
	KlKn.zinIn[0].y = 0x4000;			// ����� ���� ��������� ��������� ������� �� "��������"
	KlKn.zinIn[1].x = 0;				// ����� ����� ��������� �������� ������� � "��������"
	KlKn.zinIn[1].y = 0x8000;			// ����� ���� ��������� ��������� ������� � "��������"

	// �������� ������� ��-���
	KlKam.type = 12;					// ��-���
	// ����� �������
	KlKam.normaNmb[0] = 34;				// ��� ����� ���������
	KlKam.normaNmb[1] = 35;				// ��� ����� ����������
	// ����������� ������� �������
	KlKam.tkAction[0] = 2;				// �.����� ��������
	KlKam.tkAction[1] = 2;				// �.����� ��������
	KlKam.tkAction[2] = 2;				// �.����� �������� ����� ��������
	KlKam.tkAction[3] = 2;				// �.����� �������� ����� ��������
	// ����������� �������
	KlKam.diagnObject[0].x = 13;		// ����� ����� ����������� "�� ��������"
	KlKam.diagnObject[0].y = 0x01;		// ����� ���� ����������� "�� ��������"
	KlKam.diagnObject[1].x = 13;		// ����� ����� ����������� "�� ��������"
	KlKam.diagnObject[1].y = 0x02;		// ����� ���� ����������� "�� ��������"
	KlKam.diagnObject[2].x = 13;		// ����� ����� ����������� "�� ����������"
	KlKam.diagnObject[2].y = 0x04;		// ����� ���� ����������� "�� ����������"
	KlKam.diagnObject[3].x = 13;		// ����� ����� ����������� "������������"
	KlKam.diagnObject[3].y = 0x08;		// ����� ���� ����������� "������������"
	// ���������� ����������� ������
	KlKam.zinOut[0].x = 0;				// ����� ����� ���. ��������� ������� �� "��������"
	KlKam.zinOut[0].y = 0x10;			// ����� ���� ���. ��������� �������  �� "��������"
	KlKam.zinOut[1].x = 0;				// ����� ����� ���. ��������� ������� �� "��������"
	KlKam.zinOut[1].y = 0x10;			// ����� ���� ���. ��������� ������� �� "��������"
	// ���������� ����� �������� �����
	KlKam.zinIn[0].x = 0;				// ����� ����� ��������� �������� ������� �� "��������"
	KlKam.zinIn[0].y = 0x1000;			// ����� ���� ��������� ��������� ������� �� "��������"
	KlKam.zinIn[1].x = 0;				// ����� ����� ��������� �������� ������� � "��������"
	KlKam.zinIn[1].y = 0x2000;			// ����� ���� ��������� ��������� ������� � "��������"


	// �������� ���������
	Fvn.type = 11;						// ���
	// ����� �������
	Fvn.normaNmb[0] = 42;				// ��� ����� ���������
	Fvn.normaNmb[1] = 43;				// ��� ����� ����������
	// ����������� ������� �������
	Fvn.tkAction[0] = 5;				// �.����� ��������
	Fvn.tkAction[1] = 5;				// �.����� ��������
	Fvn.tkAction[2] = 15;				// �.����� �������� ����� ��������
	Fvn.tkAction[3] = 2;				// �.����� �������� ����� ��������
	// ����������� �������
	Fvn.diagnObject[0].x = 14;			// ����� ����� ����������� "�� ��������"
	Fvn.diagnObject[0].y = 0x40;		// ����� ���� ����������� "�� ��������"
	Fvn.diagnObject[1].x = 14;			// ����� ����� ����������� "�� ��������"
	Fvn.diagnObject[1].y = 0x80;		// ����� ���� ����������� "�� ��������"
	Fvn.diagnObject[2].x = 14;			// ����� ����� ����������� "�� ����������"
	Fvn.diagnObject[2].y = 0x80;		// ����� ���� ����������� "�� ����������"
	Fvn.diagnObject[3].x = 14;			// ����� ����� ����������� "������������"
	Fvn.diagnObject[3].y = 0x80;		// ����� ���� ����������� "������������"
	// ���������� ����������� ������
	Fvn.zinOut[0].x = 0;				// ����� ����� ���. ��������� ������� �� "��������"
	Fvn.zinOut[0].y = 0x01;				// ����� ���� ���. ��������� �������  �� "��������"
	Fvn.zinOut[1].x = 0;				// ����� ����� ���. ��������� ������� �� "��������"
	Fvn.zinOut[1].y = 0x01;				// ����� ���� ���. ��������� ������� �� "��������"
	// ���������� ����� �������� �����
	Fvn.zinIn[0].x = 1;					// ����� ����� ��������� �������� ������� �� "��������"
	Fvn.zinIn[0].y = 0x01;				// ����� ���� ��������� ��������� ������� �� "��������"
	Fvn.zinIn[1].x = 1;					// ����� ����� ��������� �������� ������� � "��������"
	Fvn.zinIn[1].y = 0x01;				// ����� ���� ��������� ��������� ������� � "��������"
}
