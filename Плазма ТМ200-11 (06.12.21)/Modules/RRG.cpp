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
			SetOut(prRRG,object->outKl1.nBt,object->outKl1.bMask); 	// ������� ��-��� �� 1 ���
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
	diagn[object->diagnSUM.nBt]&=(~object->diagnSUM.bMask);// ����� ���������� ������
	SetOut(0,object->outKl0.nBt,object->outKl0.bMask);// ������� ��-��� �� 0
	SetOut(0,object->outKl1.nBt,object->outKl1.bMask);// ������� ��-��� �� 1
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
        ObjRRG[i] = new SRRG();
// �������� ������� ���1
	ObjRRG[0]->nR = 20;					// ����� ���������������� ������
	ObjRRG[0]->normaOnNmb = 12;			// ��� ����� ������ �� ����� ���
	ObjRRG[0]->normaOffNmb = 13;		// ��� ����� ���������� ���
	ObjRRG[0]->ctVR = 0;				// ������� ������ �� �����
	ObjRRG[0]->tkVR = 10;				// � ��� �����. �����
	ObjRRG[0]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjRRG[0]->tkVRK = 3;				// � ��� �����. �����
	ObjRRG[0]->diagnNVR.nBt = 0;
	ObjRRG[0]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ���
	ObjRRG[0]->diagnBOR.nBt = 0;
	ObjRRG[0]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ���
	ObjRRG[0]->diagnNR.nBt = 0;
	ObjRRG[0]->diagnNR.bMask = 0x04;	// ��� ������������� ���
	ObjRRG[0]->diagnSUM.nBt = 0;
	ObjRRG[0]->diagnSUM.bMask = 0x07;	// ��������� ����������� ������ 
	ObjRRG[0]->outKl0.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[0]->outKl0.bMask = 0x01;
	ObjRRG[0]->outKl1.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[0]->outKl1.bMask = 0x01;
	ObjRRG[0]->aikBt = 5;				// ����������� ���������� ����
	ObjRRG[0]->parRRG = 0;				// �������� ���
	ObjRRG[0]->aoutBt = 0;				// ���������� ����� �������
	ObjRRG[0]->vRRG = 0;				// ������� ������ �� �����
	ObjRRG[0]->maxFl = 9.0;				// ������������ ������ ���
	ObjRRG[0]->porog1 = 0.1;			// ����� ��������� ����������� "���" 10%
	ObjRRG[0]->porog2 = 0.5;			// ����� ��������� ����������� "��" 50%
	
	// �������� ������� ���2
	ObjRRG[1]->nR = 21;					// ����� ���������������� ������
	ObjRRG[1]->normaOnNmb = 14;			// ��� ����� ������ �� ����� ���
	ObjRRG[1]->normaOffNmb = 15;		// ��� ����� ���������� ���
	ObjRRG[1]->ctVR = 0;				// ������� ������ �� �����
	ObjRRG[1]->tkVR = 10;				// � ��� �����. �����
	ObjRRG[1]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjRRG[1]->tkVRK = 3;				// � ��� �����. �����
	ObjRRG[1]->diagnNVR.nBt = 0;
	ObjRRG[1]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ���
	ObjRRG[1]->diagnBOR.nBt = 0;
	ObjRRG[1]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ���
	ObjRRG[1]->diagnNR.nBt = 0;
	ObjRRG[1]->diagnNR.bMask = 0x40;	// ��� ������������� ���
	ObjRRG[1]->diagnSUM.nBt = 0;
	ObjRRG[1]->diagnSUM.bMask = 0x70;	// ��������� ����������� ������ 
	ObjRRG[1]->outKl0.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[1]->outKl0.bMask = 0x02;
	ObjRRG[1]->outKl1.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[1]->outKl1.bMask = 0x02;
	ObjRRG[1]->aikBt = 6;				// ����������� ���������� ����
	ObjRRG[1]->parRRG = 0;				// �������� ���
	ObjRRG[1]->aoutBt = 1;				// ���������� ����� �������
	ObjRRG[1]->vRRG = 0;				// ������� ������ �� �����
	ObjRRG[1]->maxFl = 36.0;				// ������������ ������ ���
	ObjRRG[1]->porog1 = 0.1;			// ����� ��������� ����������� "���" 10%
	ObjRRG[1]->porog2 = 0.5;			// ����� ��������� ����������� "��" 50%
	
	// �������� ������� ���3
	ObjRRG[2]->nR = 22;					// ����� ���������������� ������
	ObjRRG[2]->normaOnNmb = 16;				// ��� ����� ������ �� ����� ���
	ObjRRG[2]->normaOffNmb = 17;				// ��� ����� ���������� ���
	ObjRRG[2]->ctVR = 0;				// ������� ������ �� �����
	ObjRRG[2]->tkVR = 10;					// � ��� �����. �����
	ObjRRG[2]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjRRG[2]->tkVRK = 3;					// � ��� �����. �����
	ObjRRG[2]->diagnNVR.nBt = 1;
	ObjRRG[2]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ���
	ObjRRG[2]->diagnBOR.nBt = 1;
	ObjRRG[2]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ���
	ObjRRG[2]->diagnNR.nBt = 1;
	ObjRRG[2]->diagnNR.bMask = 0x04;	// ��� ������������� ���
	ObjRRG[2]->diagnSUM.nBt = 1;
	ObjRRG[2]->diagnSUM.bMask = 0x07;	// ��������� ����������� ������ 
	ObjRRG[2]->outKl0.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[2]->outKl0.bMask = 0x04;
	ObjRRG[2]->outKl1.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[2]->outKl1.bMask = 0x04;
	ObjRRG[2]->aikBt = 7;				// ����������� ���������� ����
	ObjRRG[2]->parRRG = 0;				// �������� ���
	ObjRRG[2]->aoutBt = 2;				// ���������� ����� �������
	ObjRRG[2]->vRRG = 0;				// ������� ������ �� �����
	ObjRRG[2]->maxFl = 3.6;				// ������������ ������ ���
	ObjRRG[2]->porog1 = 0.1;			// ����� ��������� ����������� "���" 10%
	ObjRRG[2]->porog2 = 0.5;			// ����� ��������� ����������� "��" 50%
	
	// �������� ������� ���4
	ObjRRG[3]->nR = 23;					// ����� ���������������� ������
	ObjRRG[3]->normaOnNmb = 18;				// ��� ����� ������ �� ����� ���
	ObjRRG[3]->normaOffNmb = 19;				// ��� ����� ���������� ���
	ObjRRG[3]->ctVR = 0;				// ������� ������ �� �����
	ObjRRG[3]->tkVR = 10;					// � ��� �����. �����
	ObjRRG[3]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjRRG[3]->tkVRK = 3;					// � ��� �����. �����
	ObjRRG[3]->diagnNVR.nBt = 1;
	ObjRRG[3]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ���
	ObjRRG[3]->diagnBOR.nBt = 1;
	ObjRRG[3]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ���
	ObjRRG[3]->diagnNR.nBt = 1;
	ObjRRG[3]->diagnNR.bMask = 0x40;	// ��� ������������� ���
	ObjRRG[3]->diagnSUM.nBt = 1;
	ObjRRG[3]->diagnSUM.bMask = 0x70;	// ��������� ����������� ������ 
	ObjRRG[3]->outKl0.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[3]->outKl0.bMask = 0x08;
	ObjRRG[3]->outKl1.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[3]->outKl1.bMask = 0x08;
	ObjRRG[3]->aikBt = 8;				// ����������� ���������� ����
	ObjRRG[3]->parRRG = 0;				// �������� ���
	ObjRRG[3]->aoutBt = 3;				// ���������� ����� �������
	ObjRRG[3]->vRRG = 0;				// ������� ������ �� �����
	ObjRRG[3]->maxFl = 9.0;				// ������������ ������ ���
	ObjRRG[3]->porog1 = 0.1;			// ����� ��������� ����������� "���" 10%
	ObjRRG[3]->porog2 = 0.5;			// ����� ��������� ����������� "��" 50%	
	
	// �������� ������� ���5
	ObjRRG[4]->nR = 24;					// ����� ���������������� ������
	ObjRRG[4]->normaOnNmb = 53;				// ��� ����� ������ �� ����� ���
	ObjRRG[4]->normaOffNmb = 54;				// ��� ����� ���������� ���
	ObjRRG[4]->ctVR = 0;				// ������� ������ �� �����
	ObjRRG[4]->tkVR = 10;					// � ��� �����. �����
	ObjRRG[4]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjRRG[4]->tkVRK = 3;					// � ��� �����. �����
	ObjRRG[4]->diagnNVR.nBt = 2;
	ObjRRG[4]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ���
	ObjRRG[4]->diagnBOR.nBt = 2;
	ObjRRG[4]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ���
	ObjRRG[4]->diagnNR.nBt = 2;
	ObjRRG[4]->diagnNR.bMask = 0x04;	// ��� ������������� ���
	ObjRRG[4]->diagnSUM.nBt = 2;
	ObjRRG[4]->diagnSUM.bMask = 0x07;	// ��������� ����������� ������ 
	ObjRRG[4]->outKl0.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[4]->outKl0.bMask = 0x10;
	ObjRRG[4]->outKl1.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[4]->outKl1.bMask = 0x10;
	ObjRRG[4]->aikBt = 9;				// ����������� ���������� ����
	ObjRRG[4]->parRRG = 0;				// �������� ���
	ObjRRG[4]->aoutBt = 4;				// ���������� ����� �������
	ObjRRG[4]->vRRG = 0;				// ������� ������ �� �����
	ObjRRG[4]->maxFl = 18.0;				// ������������ ������ ���
	ObjRRG[4]->porog1 = 0.1;			// ����� ��������� ����������� "���" 10%
	ObjRRG[4]->porog2 = 0.5;			// ����� ��������� ����������� "��" 50%
	
	// �������� ������� ���6
	ObjRRG[5]->nR = 25;					// ����� ���������������� ������
	ObjRRG[5]->normaOnNmb = 55;				// ��� ����� ������ �� ����� ���
	ObjRRG[5]->normaOffNmb = 56;				// ��� ����� ���������� ���
	ObjRRG[5]->ctVR = 0;				// ������� ������ �� �����
	ObjRRG[5]->tkVR = 10;					// � ��� �����. �����
	ObjRRG[5]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjRRG[5]->tkVRK = 3;					// � ��� �����. �����
	ObjRRG[5]->diagnNVR.nBt = 18;
	ObjRRG[5]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ���
	ObjRRG[5]->diagnBOR.nBt = 18;
	ObjRRG[5]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ���
	ObjRRG[5]->diagnNR.nBt = 18;
	ObjRRG[5]->diagnNR.bMask = 0x04;	// ��� ������������� ���
	ObjRRG[5]->diagnSUM.nBt = 18;
	ObjRRG[5]->diagnSUM.bMask = 0x07;	// ��������� ����������� ������ 
	ObjRRG[5]->outKl0.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[5]->outKl0.bMask = 0x20;
	ObjRRG[5]->outKl1.nBt = 2;			// �������� ���������� ������ �������
	ObjRRG[5]->outKl1.bMask = 0x20;
	ObjRRG[5]->aikBt = 10;				// ����������� ���������� ����
	ObjRRG[5]->parRRG = 0;				// �������� ���
	ObjRRG[5]->aoutBt = 5;				// ���������� ����� �������
	ObjRRG[5]->vRRG = 0;				// ������� ������ �� �����
	ObjRRG[5]->maxFl = 18.0;				// ������������ ������ ���
	ObjRRG[5]->porog1 = 0.1;			// ����� ��������� ����������� "���" 10%
	ObjRRG[5]->porog2 = 0.5;			// ����� ��������� ����������� "��" 50%	
	
	// �������� ������� ���7
	ObjRRG[6]->nR = 26;					// ����� ���������������� ������
	ObjRRG[6]->normaOnNmb = 57;				// ��� ����� ������ �� ����� ���
	ObjRRG[6]->normaOffNmb = 58;				// ��� ����� ���������� ���
	ObjRRG[6]->ctVR = 0;				// ������� ������ �� �����
	ObjRRG[6]->tkVR = 10;					// � ��� �����. �����
	ObjRRG[6]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjRRG[6]->tkVRK = 3;					// � ��� �����. �����
	ObjRRG[6]->diagnNVR.nBt = 18;
	ObjRRG[6]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ���
	ObjRRG[6]->diagnBOR.nBt = 18;
	ObjRRG[6]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ���
	ObjRRG[6]->diagnNR.nBt = 18;
	ObjRRG[6]->diagnNR.bMask = 0x40;	// ��� ������������� ���
	ObjRRG[6]->diagnSUM.nBt = 18;
	ObjRRG[6]->diagnSUM.bMask = 0x70;	// ��������� ����������� ������ 
	ObjRRG[6]->outKl0.nBt = 3;			// �������� ���������� ������ �������
	ObjRRG[6]->outKl0.bMask = 0x2000;
	ObjRRG[6]->outKl1.nBt = 3;			// �������� ���������� ������ �������
	ObjRRG[6]->outKl1.bMask = 0x4000;
	ObjRRG[6]->aikBt = 11;				// ����������� ���������� ����
	ObjRRG[6]->parRRG = 0;				// �������� ���
	ObjRRG[6]->aoutBt = 6;				// ���������� ����� �������
	ObjRRG[6]->vRRG = 0;				// ������� ������ �� �����
	ObjRRG[6]->maxFl = 3.6;				// ������������ ������ ���
	ObjRRG[6]->porog1 = 0.1;			// ����� ��������� ����������� "���" 10%
	ObjRRG[6]->porog2 = 0.5;			// ����� ��������� ����������� "��" 50%	
}