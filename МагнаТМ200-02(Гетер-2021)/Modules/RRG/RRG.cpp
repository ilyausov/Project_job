//---------------------------------------------------------------------------
//--���� �������� ��������� �������--//
//---------------------------------------------------------------------------
#include "RRG.h"
//---------------------------------------------------------------------------
//--������� ��������� ���--//
//---------------------------------------------------------------------------
void RRGOn(SRRG *object, bool prRRG)
{
	switch (shr[object->nR])	// ����������� ��� ������
	{
		case 0: ;break;			// 0 - ��� ������
		case 1:
		{
			A_OUT(object->aoutBt,object->parRRG+8192);// ������ ������� �� ���
			SetOut(!prRRG,object->outKl0.nBt,object->outKl0.bMask);	// ������� ��-��� �� 0
			SetOut(prRRG,object->outKl1.nBt,object->outKl1.bMask); 	// ������� ��-��� �� 1
			object->vRRG = 0;	// ����� �������� ������ ��� �� �����
			diagn[object->diagnSUM.nBt]&=(~object->diagnSUM.bMask);// ����� ���������� ������
			object->ctVR = 0;	// ����� �������� ������ �� �����
			object->ctVRK = 0;	// ����� ������������ �������� ������ �� �����
			shr[object->nR]++;	// ������� �� ����. ���
		}; break;
		case 2:
		{
			// ��� ������ ������� �� ���
			if (aout[object->aoutBt]==(object->parRRG+8192))
			{
				// ���� ��� � ���������
				if (abs((aik[object->aikBt])-object->parRRG)<=(float(object->parRRG)*object->porog1))
				{
					// ���� ��� ��������� �� ������ ����������� �����
					if ( object->ctVRK > object->tkVRK )
					{
						diagn[object->diagnNVR.nBt]&=(~object->diagnNVR.bMask);// ����� �����������
						object->vRRG = 1;	// ��������� ������� ������ �� ������
						norma = object->normaOnNmb;	// �����: "��� ����� �� �����"
						shr[object->nR]++;			// ������� �� ����. ���
					}
				}
				// ��� �� � ���������
				else
				{
					object->ctVRK = 0;	// ����� ������������ �������� ������ �� �����
					if (object->ctVR > object->tkVR)	// ����� �����
						diagn[object->diagnNVR.nBt] |= object->diagnNVR.bMask; // �����: "��� ������ ��� �� �����"
				}
			}
			// ���� ����� ������� �� ���
			else A_OUT(object->aoutBt,object->parRRG+8192); // ������ ������� �� ���
		}; break;
		case 3:
		{
			// ��� ������ ������� �� ���
			if (aout[object->aoutBt]==(object->parRRG+8192))
			{
				// ���� �.��.�����. ���
				if (abs((aik[object->aikBt])-object->parRRG)>(float(object->parRRG)*object->porog1))
				{
					// ���� ������������� ���
					if (abs((aik[object->aikBt])-object->parRRG)<(float(object->parRRG)*object->porog2))
					{
						diagn[object->diagnBOR.nBt]|=object->diagnBOR.bMask;	// �����: "�.��.�����.���"
						diagn[object->diagnNR.nBt]&=(~object->diagnNR.bMask);	// �����: "��� �����.���"
					}
					else
					{
						diagn[object->diagnNR.nBt]|=object->diagnNR.bMask;		// �����: "��� �����.���"
						diagn[object->diagnBOR.nBt]&=(~object->diagnBOR.bMask);	// �����: "�.��.�����.���"
					}
				}
				else
				{
					diagn[object->diagnNR.nBt]&=(~object->diagnNR.bMask);		// �����: "��� �����.���"
					diagn[object->diagnBOR.nBt]&=(~object->diagnBOR.bMask);		// �����: "�.��.�����.���"
				}
			}
			// ���� ����� ������� �� ���
			else A_OUT(object->aoutBt,(object->parRRG+8192));	// ������ �������
		}; break;
		default:						// ����������� ���
		{
			sh[object->nR]  = 0;
			shr[object->nR] = 0;
		}; break;
	}
}
//---------------------------------------------------------------------------
//--������� ���������� ���--//
//---------------------------------------------------------------------------
void RRGOff(SRRG *object)
{
 	sh[object->nR]  = 0;
	shr[object->nR] = 0;
	object->vRRG = 0;						// ����� �������� ������ ��� �� �����
    SetOut(0,object->outKl0.nBt,object->outKl0.bMask);// ������� ��-��� �� 0
	SetOut(0,object->outKl1.nBt,object->outKl1.bMask);// ������� ��-��� �� 1
	diagn[object->diagnSUM.nBt]&=(~object->diagnSUM.bMask);// ����� ���������� ������
	A_OUT(object->aoutBt,8192);				// �������� ������� �� ���
	norma = object->normaOffNmb;			// �����: "����� ��� ��������"
}
//---------------------------------------------------------------------------
//--������� ���������� ���������--//
//---------------------------------------------------------------------------
void TimeRRG()
{
	for(int i=0;i<qRRG;i++)
	{
		ObjRRG[i]->ctVR++;	// ������� ������ �� �����
		ObjRRG[i]->ctVRK++;	// ����������� ������� ������ �� �����
	}
}
//---------------------------------------------------------------------------
//--������� ������������� �������� ���� ���--//
//---------------------------------------------------------------------------
void InitObjectsRRG()
{
    for (unsigned int i = 0 ; i < qRRG ; i++ )
    {
        ObjRRG[i] = new SRRG();
        ObjRRG[i]->ctVR = 0;				// ������� ������ �� �����
	    ObjRRG[i]->tkVR = 10;				// � ��� �����. �����
	    ObjRRG[i]->ctVRK = 0;				// ����������� ������� ������ �� �����
	    ObjRRG[i]->tkVRK = 3;				// � ��� �����. �����
        ObjRRG[i]->parRRG = 0;				// �������� ���

	    ObjRRG[i]->vRRG = 0;				// ������� ������ �� �����
        ObjRRG[i]->porog1 = 0.1;			// ����� ��������� ����������� "���" 10%
	    ObjRRG[i]->porog2 = 0.5;			// ����� ��������� ����������� "��" 50%
    }
// �������� ������� ���1
	ObjRRG[0]->nR = 20;					// ����� ���������������� ������
	ObjRRG[0]->normaOnNmb = 12;			// ��� ����� ������ �� ����� ���
	ObjRRG[0]->normaOffNmb = 13;		// ��� ����� ���������� ���
	ObjRRG[0]->aoutBt = 0;				// ���������� ����� �������
	ObjRRG[0]->diagnNVR.nBt = 0;
	ObjRRG[0]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ���
	ObjRRG[0]->diagnBOR.nBt = 0;
	ObjRRG[0]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ���
	ObjRRG[0]->diagnNR.nBt = 0;
	ObjRRG[0]->diagnNR.bMask = 0x04;	// ��� ������������� ���
	ObjRRG[0]->diagnSUM.nBt = 0;
	ObjRRG[0]->diagnSUM.bMask = 0x07;	// ��������� ����������� ������ 
	ObjRRG[0]->outKl0.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[0]->outKl0.bMask = 0x100;
	ObjRRG[0]->outKl1.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[0]->outKl1.bMask = 0x100;
	ObjRRG[0]->aikBt = 6;				// ����������� ���������� ����

	ObjRRG[0]->maxFl = 9.0;				// ������������ ������ ���


	// �������� ������� ���2
	ObjRRG[1]->nR = 21;					// ����� ���������������� ������
	ObjRRG[1]->normaOnNmb = 14;			// ��� ����� ������ �� ����� ���
	ObjRRG[1]->normaOffNmb = 15;		// ��� ����� ���������� ���

	ObjRRG[1]->diagnNVR.nBt = 0;
	ObjRRG[1]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ���
	ObjRRG[1]->diagnBOR.nBt = 0;
	ObjRRG[1]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ���
	ObjRRG[1]->diagnNR.nBt = 0;
	ObjRRG[1]->diagnNR.bMask = 0x40;	// ��� ������������� ���
	ObjRRG[1]->diagnSUM.nBt = 0;
	ObjRRG[1]->diagnSUM.bMask = 0x70;	// ��������� ����������� ������ 
	ObjRRG[1]->outKl0.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[1]->outKl0.bMask = 0x200;
	ObjRRG[1]->outKl1.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[1]->outKl1.bMask = 0x200;
	ObjRRG[1]->aikBt = 7;				// ����������� ���������� ����
	ObjRRG[1]->parRRG = 0;				// �������� ���
	ObjRRG[1]->aoutBt = 1;				// ���������� ����� �������

	ObjRRG[1]->maxFl = 180.0;				// ������������ ������ ���

	
	// �������� ������� ���3
	ObjRRG[2]->nR = 22;					// ����� ���������������� ������
	ObjRRG[2]->normaOnNmb = 16;				// ��� ����� ������ �� ����� ���
	ObjRRG[2]->normaOffNmb = 17;				// ��� ����� ���������� ���

	ObjRRG[2]->diagnNVR.nBt = 1;
	ObjRRG[2]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ���
	ObjRRG[2]->diagnBOR.nBt = 1;
	ObjRRG[2]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ���
	ObjRRG[2]->diagnNR.nBt = 1;
	ObjRRG[2]->diagnNR.bMask = 0x04;	// ��� ������������� ���
	ObjRRG[2]->diagnSUM.nBt = 1;
	ObjRRG[2]->diagnSUM.bMask = 0x07;	// ��������� ����������� ������ 
	ObjRRG[2]->outKl0.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[2]->outKl0.bMask = 0x400;
	ObjRRG[2]->outKl1.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[2]->outKl1.bMask = 0x400;
	ObjRRG[2]->aikBt = 8;				// ����������� ���������� ����
	ObjRRG[2]->parRRG = 0;				// �������� ���
	ObjRRG[2]->aoutBt = 2;				// ���������� ����� �������

	ObjRRG[2]->maxFl = 9,0;				// ������������ ������ ���

    // �������� ������� ���4
	ObjRRG[3]->nR = 23;					// ����� ���������������� ������
	ObjRRG[3]->normaOnNmb = 18;				// ��� ����� ������ �� ����� ���
	ObjRRG[3]->normaOffNmb = 19;				// ��� ����� ���������� ���

	ObjRRG[3]->diagnNVR.nBt = 1;
	ObjRRG[3]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ���
	ObjRRG[3]->diagnBOR.nBt = 1;
	ObjRRG[3]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ���
	ObjRRG[3]->diagnNR.nBt = 1;
	ObjRRG[3]->diagnNR.bMask = 0x40;	// ��� ������������� ���
	ObjRRG[3]->diagnSUM.nBt = 1;
	ObjRRG[3]->diagnSUM.bMask = 0x70;	// ��������� ����������� ������
	ObjRRG[3]->outKl0.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[3]->outKl0.bMask = 0x1000;
	ObjRRG[3]->outKl1.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[3]->outKl1.bMask = 0x2000;
	ObjRRG[3]->aikBt = 9;				// ����������� ���������� ����
	ObjRRG[3]->parRRG = 0;				// �������� ���
	ObjRRG[3]->aoutBt = 3;				// ���������� ����� �������

	ObjRRG[3]->maxFl = 3,6;				// ������������ ������ ���

	

}