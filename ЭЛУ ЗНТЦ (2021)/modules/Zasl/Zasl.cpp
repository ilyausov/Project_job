//---------------------------------------------------------------------------
//--���� �������� ��������� �������--//
//---------------------------------------------------------------------------
#include "Zasl.h"
//---------------------------------------------------------------------------
//--������� ������������� �������� �������--//
//---------------------------------------------------------------------------
void SZasl::Time()
{
    ctObject++;
}
//---------------------------------------------------------------------------
//--������� ���������� ��������� ����� ���������� �����-������--//
//---------------------------------------------------------------------------
void Zasl(bool action, SZasl *object)
{
    switch ( object -> type )
    {
        
        case 21: DoAction21(action, object); break;
        
        // �������� ��� �������
        default: diagn[0] |= 0x01; break;
    }
}
//---------------------------------------------------------------------------
//--������� ���������� �������� � 2 ����������� �������� � 1 ������--//
//---------------------------------------------------------------------------
void DoAction21(bool action, SZasl *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // ���� ������ ��������� ��������� - ��������� ������
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) != action )
            {
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
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) != action )
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
//--������� ���������� ���������--//
//---------------------------------------------------------------------------
void TimeZasl()
{

	
    II.Time();
    M1.Time();
    M2.Time();

}
//---------------------------------------------------------------------------
//--������� ������������� �������� � ���������� ����������� � ���������� �������� ������--//
//---------------------------------------------------------------------------
void InitObjectsZasl()
{
	
    
    // �������� �������� ��
	II.type = 21;						// ��� 2/1
	// ����� �������
	II.normaNmb[0] = 76;				// ��� ����� ���������
	II.normaNmb[1] = 77;				// ��� ����� ����������
	// ����������� ������� �������
	II.tkAction[0] = 10;				// �.����� ��������
	II.tkAction[1] = 10;				// �.����� ��������
	II.tkAction[2] = 1;					// �.����� �������� ����� ��������
	II.tkAction[3] = 1;					// �.����� �������� ����� ��������
	// ����������� �������
	II.diagnObject[0].x = 29;			// ����� ����� ����������� "�� ��������"
	II.diagnObject[0].y = 0x01;			// ����� ���� ����������� "�� ��������"
	II.diagnObject[1].x = 29;			// ����� ����� ����������� "�� ��������"
	II.diagnObject[1].y = 0x02;			// ����� ���� ����������� "�� ��������"
	II.diagnObject[2].x = 29;			// ����� ����� ����������� "�� ����������"
	II.diagnObject[2].y = 0x04;			// ����� ���� ����������� "�� ����������"
	II.diagnObject[3].x = 29;			// ����� ����� ����������� "������������"
	II.diagnObject[3].y = 0x08;			// ����� ���� ����������� "������������"
	// ���������� ����������� ������
	II.zinOut[0].x = 1;				// ����� ����� ���. ��������� ������� �� "��������"
	II.zinOut[0].y = 0x100;			// ����� ���� ���. ��������� �������  �� "��������"
	II.zinOut[1].x = 1;				// ����� ����� ���. ��������� ������� �� "��������"
	II.zinOut[1].y = 0x200;			// ����� ���� ���. ��������� ������� �� "��������"
	// ���������� ����� �������� �����
	II.zinIn[0].x = 3;				// ����� ����� ��������� �������� ������� �� "��������"
	II.zinIn[0].y = 0x20;			// ����� ���� ��������� �������� ������� �� "��������"
	II.zinIn[1].x = 3;				// ����� ����� ��������� �������� ������� � "��������"
	II.zinIn[1].y = 0x20;			// ����� ���� ��������� �������� ������� � "��������"
    
    // �������� �������� �1
	M1.type = 21;						// ��� 2/1
	// ����� �������
	M1.normaNmb[0] = 78;				// ��� ����� ���������
	M1.normaNmb[1] = 79;				// ��� ����� ����������
	// ����������� ������� �������
	M1.tkAction[0] = 10;				// �.����� ��������
	M1.tkAction[1] = 10;				// �.����� ��������
	M1.tkAction[2] = 1;					// �.����� �������� ����� ��������
	M1.tkAction[3] = 1;					// �.����� �������� ����� ��������
	// ����������� �������
	M1.diagnObject[0].x = 29;			// ����� ����� ����������� "�� ��������"
	M1.diagnObject[0].y = 0x10;			// ����� ���� ����������� "�� ��������"
	M1.diagnObject[1].x = 29;			// ����� ����� ����������� "�� ��������"
	M1.diagnObject[1].y = 0x20;			// ����� ���� ����������� "�� ��������"
	M1.diagnObject[2].x = 29;			// ����� ����� ����������� "�� ����������"
	M1.diagnObject[2].y = 0x40;			// ����� ���� ����������� "�� ����������"
	M1.diagnObject[3].x = 29;			// ����� ����� ����������� "������������"
	M1.diagnObject[3].y = 0x80;			// ����� ���� ����������� "������������"
	// ���������� ����������� ������
	M1.zinOut[0].x = 1;				// ����� ����� ���. ��������� ������� �� "��������"
	M1.zinOut[0].y = 0x10;			// ����� ���� ���. ��������� �������  �� "��������"
	M1.zinOut[1].x = 1;				// ����� ����� ���. ��������� ������� �� "��������"
	M1.zinOut[1].y = 0x20;			// ����� ���� ���. ��������� ������� �� "��������"
	// ���������� ����� �������� �����
	M1.zinIn[0].x = 3;				// ����� ����� ��������� �������� ������� �� "��������"
	M1.zinIn[0].y = 0x08;			// ����� ���� ��������� �������� ������� �� "��������"
	M1.zinIn[1].x = 3;				// ����� ����� ��������� �������� ������� � "��������"
	M1.zinIn[1].y = 0x08;			// ����� ���� ��������� �������� ������� � "��������"

    // �������� �������� �2
	M2.type = 21;						// ��� 2/1
	// ����� �������
	M2.normaNmb[0] = 80;				// ��� ����� ���������
	M2.normaNmb[1] = 81;				// ��� ����� ����������
	// ����������� ������� �������
	M2.tkAction[0] = 10;				// �.����� ��������
	M2.tkAction[1] = 10;				// �.����� ��������
	M2.tkAction[2] = 1;					// �.����� �������� ����� ��������
	M2.tkAction[3] = 1;					// �.����� �������� ����� ��������
	// ����������� �������
	M2.diagnObject[0].x = 30;			// ����� ����� ����������� "�� ��������"
	M2.diagnObject[0].y = 0x01;			// ����� ���� ����������� "�� ��������"
	M2.diagnObject[1].x = 30;			// ����� ����� ����������� "�� ��������"
	M2.diagnObject[1].y = 0x02;			// ����� ���� ����������� "�� ��������"
	M2.diagnObject[2].x = 30;			// ����� ����� ����������� "�� ����������"
	M2.diagnObject[2].y = 0x04;			// ����� ���� ����������� "�� ����������"
	M2.diagnObject[3].x = 30;			// ����� ����� ����������� "������������"
	M2.diagnObject[3].y = 0x08;			// ����� ���� ����������� "������������"
	// ���������� ����������� ������
	M2.zinOut[0].x = 1;				// ����� ����� ���. ��������� ������� �� "��������"
	M2.zinOut[0].y = 0x40;			// ����� ���� ���. ��������� �������  �� "��������"
	M2.zinOut[1].x = 1;				// ����� ����� ���. ��������� ������� �� "��������"
	M2.zinOut[1].y = 0x80;			// ����� ���� ���. ��������� ������� �� "��������"
	// ���������� ����� �������� �����
	M2.zinIn[0].x = 3;				// ����� ����� ��������� �������� ������� �� "��������"
	M2.zinIn[0].y = 0x10;			// ����� ���� ��������� �������� ������� �� "��������"
	M2.zinIn[1].x = 3;				// ����� ����� ��������� �������� ������� � "��������"
	M2.zinIn[1].y = 0x10;			// ����� ���� ��������� �������� ������� � "��������"

}
