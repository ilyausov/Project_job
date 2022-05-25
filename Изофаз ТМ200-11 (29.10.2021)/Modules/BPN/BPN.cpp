 //---------------------------------------------------------------------------
//--���� �������� ��������� �������--//
//---------------------------------------------------------------------------
#include "BPN.h"
//---------------------------------------------------------------------------
//--������� ��������� ������--//
//---------------------------------------------------------------------------
void BPNOn(SBPN *object)
{
	switch (shr[object->nR1])	// ����������� ��� ������
	{
		case 0: ;break;			// 0 - ��� ������
		case 1:
		{
			shr[object->nR0] = 0;	// ����� ������ ����������
			object->vBPN = 0;		// ����� �������� ������ ������ �� �����
			diagn[object->diagnSUM.nBt]&=(~object->diagnSUM.bMask);// ����� ���������� ������
			shr[object->nR1]++;	// ������� �� ����. ���
		}; break;
		case 2:
		{
			if(!(diagn[diagnNA.nBt]&diagnNA.bMask))	// ��� ����������� "��� ����� � ��������"
			{
				object->prBPN = 0;	// ����� �������� �������������� ������
				object->zadBPN = object->parBPN; // ������ �������
				object->ctVRK = 0;	// ����� �������� ������ �� �������
				shr[object->nR1]++;	// ������� �� ����. ���
			}
		}; break;
		case 3:
		{
			if((diagn[diagnNA.nBt]&diagnNA.bMask)||(!object->prBPN))
			{ // ��� ����� � �������� ��� ��� �������������� ������ ������� ������� �� ������
				if(object->ctVRK > BPN_tkVRK) // ����� �����. �����
					diagn[object->diagnNAU.nBt]|=(object->diagnNAU.bMask);
					// ������ ����������� ��� ������ �� ������ �������
			}
			else
			{
				diagn[object->diagnNAU.nBt]&=(~object->diagnNAU.bMask);// ����� �����������
				object->ctVR = 0;	// ����� �������� ������ �� �����
				object->ctVRK = 0;
				shr[object->nR1]++;	// ������� �� ����. ���
			}
		}; break;
		case 4:
		{
			// ��� ������ ������� �� �����
			if(object->zadBPN == object->parBPN)
			{
				// ���� ������ � ���������
				if(abs(object->tekBPN - object->zadBPN) <= (float(object->zadBPN)*BPN_porog1))
				{
					// ���� ������ ��������� �� ������ ����������� �����
					if(object->ctVRK > BPN_tkVRV)
					{
						diagn[object->diagnNVR.nBt]&=(~object->diagnNVR.bMask);// ����� �����������
						object->vBPN = 1;			// ��������� ������� ������ �� ������
						norma = object->normaOnNmb;	// �����: "����� ����� �� �����"
						shr[object->nR1]++;			// ������� �� ����. ���						
					}
                }
                else
                {
                    object->ctVRK = 0;	// ����� ������������ �������� ������ �� �����
                    if (object->ctVR > BPN_tkVR)	// ����� �����
                        diagn[object->diagnNVR.nBt] |= object->diagnNVR.bMask;
                        // �����: "��� ������ ������ �� �����"	
                }
			}
			// ���� ����� ������� �� �����
			else
			{
				shr[object->nR1] = 2; // ������� �� ��� 2
			}
		}; break;
		case 5:
		{
			// ��� ������ ������� �� �����
			if(object->zadBPN == object->parBPN)
			{
				// ���� �.��.�����. ������
				if (abs(object->tekBPN - object->zadBPN)>(float(object->zadBPN)*BPN_porog1))
				{
					// ���� ������������� ������
					if (abs(object->tekBPN - object->zadBPN)<(float(object->zadBPN)*BPN_porog2))
					{
						diagn[object->diagnBOR.nBt]|=object->diagnBOR.bMask;	// �����: "�.��.�����.������"
						diagn[object->diagnNR.nBt]&=(~object->diagnNR.bMask);	// �����: "��� �����.������"
					}
					else
					{
						diagn[object->diagnNR.nBt]|=object->diagnNR.bMask;		// �����: "��� �����.������"
						diagn[object->diagnBOR.nBt]&=(~object->diagnBOR.bMask);	// �����: "�.��.�����.������"
					}
				}
				else
				{
					diagn[object->diagnNR.nBt]&=(~object->diagnNR.bMask);		// �����: "��� �����.������"
					diagn[object->diagnBOR.nBt]&=(~object->diagnBOR.bMask);		// �����: "�.��.�����.������"
				}
			}
			// ���� ����� ������� �� �����
			else
			{
				shr[object->nR1] = 2; // ������� �� ��� 2
			}
		}; break;
		default: // ����������� ���
		{
			sh[object->nR1]  = 0;
			shr[object->nR1] = 0;
		}; break;
	}
}
//---------------------------------------------------------------------------
//--������� ���������� ������--//
//---------------------------------------------------------------------------
void BPNOff(SBPN *object)
{
	switch (shr[object->nR0])	// ����������� ��� ������
	{
		case 0: ;break;			// 0 - ��� ������
		case 1:
		{
			shr[object->nR1] = 0;	// ����� ������ ����������
			diagn[object->diagnSUM.nBt]&=(~object->diagnSUM.bMask);// ����� ���������� ������
			shr[object->nR0]++;	// ������� �� ����. ���
		}; break;
		case 2:
		{
			if(!(diagn[diagnNA.nBt]&diagnNA.bMask))	// ��� ����������� "��� ����� � ��������"
			{
				object->prBPN = 0;	// ����� �������� �������������� ������
				object->zadBPN = 0; // ������ �������
				object->ctVRK = 0;	// ����� �������� ������ �� �������
				shr[object->nR0]++;	// ������� �� ����. ���
			}
		}; break;
		case 3:
		{
			if((diagn[diagnNA.nBt]&diagnNA.bMask)||(!object->prBPN))
			{ // ��� ����� � �������� ��� ��� �������������� ������ ������� ������� �� ������
				if(object->ctVRK > BPN_tkVRK) // ����� �����. �����
					diagn[object->diagnNAU.nBt]|=(object->diagnNAU.bMask);
					// ������ ����������� ��� ������ �� ������ �������
			}
			else
			{
				diagn[object->diagnNAU.nBt]&=(~object->diagnNAU.bMask);// ����� �����������
				object->parBPN = 0; // �������� ��������
				object->vBPN = 0; // �������� ������� ������ �� ������
				norma = object->normaOffNmb;	// �����: "����� ��������"
				shr[object->nR0] = 0; // ����� ������
			}
		}; break;
		default: // ����������� ���
		{
			sh[object->nR0]  = 0;
			shr[object->nR0] = 0;
		}; break;
    }
}
//---------------------------------------------------------------------------
//--������� ���������� ���������--//
//---------------------------------------------------------------------------
void TimeBPN()
{
	for(int i=0;i<qBPN;i++)
	{
		ObjBPN[i]->ctVR++;	// ������� ������ �� �����
		ObjBPN[i]->ctVRK++;	// ����������� ������� ������ �� �����
	}
	
	CT_BPN++;
}
//---------------------------------------------------------------------------
//--������� "�������� ��� (���. ���. �� ��������)" --//
//---------------------------------------------------------------------------
void VBPN()
{
	switch (sh_)	// ����������� ���
	{
		case 0: ;break;			// 0 - ��� ������
		case 1:
		{
			SetOut(0,BPN_OnOut.nBt,BPN_OnOut.bMask); // ����� �������� �������� �� ���/����
			SetOut(0,BPN_OffOut.nBt,BPN_OffOut.bMask);
			CT_BPN = 0; // ����� ��. �������
			sh_ = 2;
		}; break;
		case 2:
		{
			if(CT_BPN > T_VKL_BPN) // �������� �� ����. ���� � ���������
				sh_ = 3;
		}; break;
		case 3:
		{
			if(zin[BPN_OnZin.nBt] & BPN_OnZin.bMask) // ������� ������� ��� ��������
			{
				diagn[diagnNON.nBt] &=(~ diagnNON.bMask); // ����� �����������
				diagn[diagnNOFF.nBt] &=(~ diagnNOFF.bMask); // ����� �����������
				sh_ = 0;
			}
			else
			{
				SetOut(1,BPN_OnOut.nBt,BPN_OnOut.bMask); // ���. ������� ������� ���
				CT_BPN = 0; // ����� ��. �������
				sh_ = 4;	
			}
		}; break;
		case 4:
		{
			if(!(zin[BPN_OnZin.nBt] & BPN_OnZin.bMask)) // ��� ��������� �������� ������� ���
			{
				if (CT_BPN<=T_VKL_BPN) return; // ����� �� �����
					diagn[diagnNON.nBt] |= diagnNON.bMask; // �����:"������� ������� ��� �� ����."
			}
			else
			{
				SetOut(0,BPN_OnOut.nBt,BPN_OnOut.bMask); // ����� �������� �������� �� ���/����
				SetOut(0,BPN_OffOut.nBt,BPN_OffOut.bMask);
				diagn[diagnNON.nBt] &=(~ diagnNON.bMask); // ����� �����������
				sh_ = 0;	
			}
		}; break;
	}
}
//---------------------------------------------------------------------------
//--������� "��������� ������� ������� ���" --//
//---------------------------------------------------------------------------
void OBPN()
{
	switch (sh_) // ����������� ���
	{
		case 0: ;break;			// 0 - ��� ������
		case 1:
		{
			SetOut(0,BPN_OnOut.nBt,BPN_OnOut.bMask); // ����� �������� �������� �� ���/����
			SetOut(0,BPN_OffOut.nBt,BPN_OffOut.bMask);
			CT_BPN = 0; // ����� ��. �������
			sh_ = 2;
		}; break;
		case 2:
		{
			if(CT_BPN > T_VKL_BPN) // �������� �� ����. ���� � ���������
				sh_ = 3;
		}; break;
		case 3:
		{
			if(!(zin[BPN_OnZin.nBt] & BPN_OnZin.bMask)) // ������� ������� ��� ���������
			{
				diagn[diagnNON.nBt] &=(~ diagnNON.bMask); // ����� �����������
				diagn[diagnNOFF.nBt] &=(~ diagnNOFF.bMask); // ����� �����������
				sh_ = 0;
			}
			else
			{
				SetOut(1,BPN_OffOut.nBt,BPN_OffOut.bMask); // ����. ������� ������� ���
				CT_BPN = 0; // ����� ��. �������
				sh_ = 4;	
			}
		}; break;
		case 4:
		{
			if(zin[BPN_OnZin.nBt] & BPN_OnZin.bMask) // ���� ��������� �������� ������� ���
			{
				if (CT_BPN<=T_VKL_BPN) return; // ����� �� �����
					diagn[diagnNOFF.nBt] |= diagnNOFF.bMask; // �����:"������� ������� ��� �� ����."
			}
			else
			{
				SetOut(0,BPN_OnOut.nBt,BPN_OnOut.bMask); // ����� �������� �������� �� ���/����
				SetOut(0,BPN_OffOut.nBt,BPN_OffOut.bMask);
				diagn[diagnNOFF.nBt] &=(~ diagnNOFF.bMask); // ����� �����������
				sh_ = 0;	
			}
		}; break;
	}
}
//---------------------------------------------------------------------------
//--������� ������������� �������� ���� ������--//
//---------------------------------------------------------------------------
void InitObjectsBPN()
{
	// �������� ������� ���
	BPN_normaOn = 20; // ��� ����� ������ �� ������
	BPN_normaOff = 21; // ��� ����� ���������� ������
	BPN_OnOut.nBt = 4; // �������� ���������� ������ ��������� ���������
	BPN_OnOut.bMask = 0x01;
	BPN_OffOut.nBt = 4; // �������� ���������� ������ ���������� ���������
	BPN_OffOut.bMask = 0x02;
	BPN_OnZin.nBt = 0; // ������� ���������� ������ ���������
	BPN_OnZin.bMask = 0x80;
	diagnNA.nBt = 0; // ��� �����
	diagnNA.bMask = 0x20;
	diagnNON.nBt = 16; // ��� ��������� ���������
	diagnNON.bMask = 0x08;
	diagnNOFF.nBt = 16; // ��� ���������� ���������	
	diagnNOFF.bMask = 0x10;
	
	// �������� ������� �������
    for (unsigned int i = 0 ; i < qBPN ; i++ )
        ObjBPN[i] = new SBPN();
	
	// �������� ������� ������ 1
	ObjBPN[0]->nR1 = 40;				// ����� ��������� ������
	ObjBPN[0]->nR0 = 41;				// ����� ���������� ������
	ObjBPN[0]->normaOnNmb = 60;			// ��� ����� ������ �� ����� ������
	ObjBPN[0]->normaOffNmb = 61;		// ��� ����� ���������� ������
	ObjBPN[0]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[0]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[0]->diagnNVR.nBt = 22;
	ObjBPN[0]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[0]->diagnBOR.nBt = 22;
	ObjBPN[0]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[0]->diagnNR.nBt = 22;
	ObjBPN[0]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[0]->diagnNAU.nBt = 22;
	ObjBPN[0]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[0]->diagnSUM.nBt = 22;
	ObjBPN[0]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������ 
	ObjBPN[0]->parBPN = 0;				// �������� ������
	ObjBPN[0]->zadBPN = 0;				// ������� �� �����
	ObjBPN[0]->tekBPN = 0;				// ������ ��������
	ObjBPN[0]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[0]->vBPN = 0;				// ������� ������ �� �����
	
	// �������� ������� ������ 2
	ObjBPN[1]->nR1 = 42;				// ����� ��������� ������
	ObjBPN[1]->nR0 = 43;				// ����� ���������� ������
	ObjBPN[1]->normaOnNmb = 62;			// ��� ����� ������ �� ����� ������
	ObjBPN[1]->normaOffNmb = 63;		// ��� ����� ���������� ������
	ObjBPN[1]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[1]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[1]->diagnNVR.nBt = 22;
	ObjBPN[1]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ������
	ObjBPN[1]->diagnBOR.nBt = 22;
	ObjBPN[1]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ������
	ObjBPN[1]->diagnNR.nBt = 22;
	ObjBPN[1]->diagnNR.bMask = 0x40;	// ��� ������������� ������
	ObjBPN[1]->diagnNAU.nBt = 22;
	ObjBPN[1]->diagnNAU.bMask = 0x80;	// ��� ������ �� ������� �������
	ObjBPN[1]->diagnSUM.nBt = 22;
	ObjBPN[1]->diagnSUM.bMask = 0xF0;	// ��������� ����������� ������ 
	ObjBPN[1]->parBPN = 0;				// �������� ������
	ObjBPN[1]->zadBPN = 0;				// ������� �� �����
	ObjBPN[1]->tekBPN = 0;				// ������ ��������
	ObjBPN[1]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[1]->vBPN = 0;				// ������� ������ �� �����

    // ����� 3 �� ������������!
	// �������� ������� ������ 3
	ObjBPN[2]->parBPN = 0;				// �������� ������
	ObjBPN[2]->zadBPN = 0;				// ������� �� �����
	ObjBPN[2]->tekBPN = 0;				// ������ ��������
	ObjBPN[2]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[2]->vBPN = 0;				// ������� ������ �� �����

	// ����� 4 �� ������������!
	// �������� ������� ������ 4
	ObjBPN[3]->parBPN = 0;				// �������� ������
	ObjBPN[3]->zadBPN = 0;				// ������� �� �����
	ObjBPN[3]->tekBPN = 0;				// ������ ��������
	ObjBPN[3]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[3]->vBPN = 0;				// ������� ������ �� �����		

	// ����� 5
	// �������� ������� ������ 5
	ObjBPN[4]->nR1 = 84;				// ����� ��������� ������
	ObjBPN[4]->nR0 = 85;				// ����� ���������� ������
	ObjBPN[4]->normaOnNmb = 112;			// ��� ����� ������ �� ����� ������
	ObjBPN[4]->normaOffNmb = 113;		// ��� ����� ���������� ������
	ObjBPN[4]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[4]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[4]->diagnNVR.nBt = 35;
	ObjBPN[4]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[4]->diagnBOR.nBt = 35;
	ObjBPN[4]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[4]->diagnNR.nBt = 35;
	ObjBPN[4]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[4]->diagnNAU.nBt = 35;
	ObjBPN[4]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[4]->diagnSUM.nBt = 35;
	ObjBPN[4]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������
	ObjBPN[4]->parBPN = 0;				// �������� ������
	ObjBPN[4]->zadBPN = 0;				// ������� �� �����
	ObjBPN[4]->tekBPN = 0;				// ������ ��������
	ObjBPN[4]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[4]->vBPN = 0;				// ������� ������ �� �����

	// �������� ������� ������ 6
	ObjBPN[5]->nR1 = 48;				// ����� ��������� ������
	ObjBPN[5]->nR0 = 49;				// ����� ���������� ������
	ObjBPN[5]->normaOnNmb = 68;			// ��� ����� ������ �� ����� ������
	ObjBPN[5]->normaOffNmb = 69;		// ��� ����� ���������� ������
	ObjBPN[5]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[5]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[5]->diagnNVR.nBt = 24;
	ObjBPN[5]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[5]->diagnBOR.nBt = 24;
	ObjBPN[5]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[5]->diagnNR.nBt = 24;
	ObjBPN[5]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[5]->diagnNAU.nBt = 24;
	ObjBPN[5]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[5]->diagnSUM.nBt = 24;
	ObjBPN[5]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������ 
	ObjBPN[5]->parBPN = 0;				// �������� ������
	ObjBPN[5]->zadBPN = 0;				// ������� �� �����
	ObjBPN[5]->tekBPN = 0;				// ������ ��������
	ObjBPN[5]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[5]->vBPN = 0;				// ������� ������ �� �����		
	
	// �������� ������� ������ 7
	ObjBPN[6]->nR1 = 50;				// ����� ��������� ������
	ObjBPN[6]->nR0 = 51;				// ����� ���������� ������
	ObjBPN[6]->normaOnNmb = 70;			// ��� ����� ������ �� ����� ������
	ObjBPN[6]->normaOffNmb = 71;		// ��� ����� ���������� ������
	ObjBPN[6]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[6]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[6]->diagnNVR.nBt = 24;
	ObjBPN[6]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ������
	ObjBPN[6]->diagnBOR.nBt = 24;
	ObjBPN[6]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ������
	ObjBPN[6]->diagnNR.nBt = 24;
	ObjBPN[6]->diagnNR.bMask = 0x40;	// ��� ������������� ������
	ObjBPN[6]->diagnNAU.nBt = 24;
	ObjBPN[6]->diagnNAU.bMask = 0x80;	// ��� ������ �� ������� �������
	ObjBPN[6]->diagnSUM.nBt = 24;
	ObjBPN[6]->diagnSUM.bMask = 0xF0;	// ��������� ����������� ������ 
	ObjBPN[6]->parBPN = 0;				// �������� ������
	ObjBPN[6]->zadBPN = 0;				// ������� �� �����
	ObjBPN[6]->tekBPN = 0;				// ������ ��������
	ObjBPN[6]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[6]->vBPN = 0;				// ������� ������ �� �����			
	
	// �������� ������� ������ 8
	ObjBPN[7]->nR1 = 52;				// ����� ��������� ������
	ObjBPN[7]->nR0 = 53;				// ����� ���������� ������
	ObjBPN[7]->normaOnNmb = 72;			// ��� ����� ������ �� ����� ������
	ObjBPN[7]->normaOffNmb = 73;		// ��� ����� ���������� ������
	ObjBPN[7]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[7]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[7]->diagnNVR.nBt = 25;
	ObjBPN[7]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[7]->diagnBOR.nBt = 25;
	ObjBPN[7]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[7]->diagnNR.nBt = 25;
	ObjBPN[7]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[7]->diagnNAU.nBt = 25;
	ObjBPN[7]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[7]->diagnSUM.nBt = 25;
	ObjBPN[7]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������ 
	ObjBPN[7]->parBPN = 0;				// �������� ������
	ObjBPN[7]->zadBPN = 0;				// ������� �� �����
	ObjBPN[7]->tekBPN = 0;				// ������ ��������
	ObjBPN[7]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[7]->vBPN = 0;				// ������� ������ �� �����		
	
	// �������� ������� ������ 9
	ObjBPN[8]->nR1 = 54;				// ����� ��������� ������
	ObjBPN[8]->nR0 = 55;				// ����� ���������� ������
	ObjBPN[8]->normaOnNmb = 74;			// ��� ����� ������ �� ����� ������
	ObjBPN[8]->normaOffNmb = 75;		// ��� ����� ���������� ������
	ObjBPN[8]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[8]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[8]->diagnNVR.nBt = 25;
	ObjBPN[8]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ������
	ObjBPN[8]->diagnBOR.nBt = 25;
	ObjBPN[8]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ������
	ObjBPN[8]->diagnNR.nBt = 25;
	ObjBPN[8]->diagnNR.bMask = 0x40;	// ��� ������������� ������
	ObjBPN[8]->diagnNAU.nBt = 25;
	ObjBPN[8]->diagnNAU.bMask = 0x80;	// ��� ������ �� ������� �������
	ObjBPN[8]->diagnSUM.nBt = 25;
	ObjBPN[8]->diagnSUM.bMask = 0xF0;	// ��������� ����������� ������ 
	ObjBPN[8]->parBPN = 0;				// �������� ������
	ObjBPN[8]->zadBPN = 0;				// ������� �� �����
	ObjBPN[8]->tekBPN = 0;				// ������ ��������
	ObjBPN[8]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[8]->vBPN = 0;				// ������� ������ �� �����		
	
	// �������� ������� ������ 10
	ObjBPN[9]->nR1 = 56;				// ����� ��������� ������
	ObjBPN[9]->nR0 = 57;				// ����� ���������� ������
	ObjBPN[9]->normaOnNmb = 76;			// ��� ����� ������ �� ����� ������
	ObjBPN[9]->normaOffNmb = 77;		// ��� ����� ���������� ������
	ObjBPN[9]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[9]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[9]->diagnNVR.nBt = 26;
	ObjBPN[9]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[9]->diagnBOR.nBt = 26;
	ObjBPN[9]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[9]->diagnNR.nBt = 26;
	ObjBPN[9]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[9]->diagnNAU.nBt = 26;
	ObjBPN[9]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[9]->diagnSUM.nBt = 26;
	ObjBPN[9]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������ 
	ObjBPN[9]->parBPN = 0;				// �������� ������
	ObjBPN[9]->zadBPN = 0;				// ������� �� �����
	ObjBPN[9]->tekBPN = 0;				// ������ ��������
	ObjBPN[9]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[9]->vBPN = 0;				// ������� ������ �� �����		
	
	// �������� ������� ������ 11
	ObjBPN[10]->nR1 = 58;				// ����� ��������� ������
	ObjBPN[10]->nR0 = 59;				// ����� ���������� ������
	ObjBPN[10]->normaOnNmb = 78;		// ��� ����� ������ �� ����� ������
	ObjBPN[10]->normaOffNmb = 79;		// ��� ����� ���������� ������
	ObjBPN[10]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[10]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[10]->diagnNVR.nBt = 26;
	ObjBPN[10]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ������
	ObjBPN[10]->diagnBOR.nBt = 26;
	ObjBPN[10]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ������
	ObjBPN[10]->diagnNR.nBt = 26;
	ObjBPN[10]->diagnNR.bMask = 0x40;	// ��� ������������� ������
	ObjBPN[10]->diagnNAU.nBt = 26;
	ObjBPN[10]->diagnNAU.bMask = 0x80;	// ��� ������ �� ������� �������
	ObjBPN[10]->diagnSUM.nBt = 26;
	ObjBPN[10]->diagnSUM.bMask = 0xF0;	// ��������� ����������� ������ 
	ObjBPN[10]->parBPN = 0;				// �������� ������
	ObjBPN[10]->zadBPN = 0;				// ������� �� �����
	ObjBPN[10]->tekBPN = 0;				// ������ ��������
	ObjBPN[10]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[10]->vBPN = 0;				// ������� ������ �� �����		

	// �������� ������� ������ 12
	ObjBPN[11]->nR1 = 60;				// ����� ��������� ������
	ObjBPN[11]->nR0 = 61;				// ����� ���������� ������
	ObjBPN[11]->normaOnNmb = 80;		// ��� ����� ������ �� ����� ������
	ObjBPN[11]->normaOffNmb = 81;		// ��� ����� ���������� ������
	ObjBPN[11]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[11]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[11]->diagnNVR.nBt = 27;
	ObjBPN[11]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[11]->diagnBOR.nBt = 27;
	ObjBPN[11]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[11]->diagnNR.nBt = 27;
	ObjBPN[11]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[11]->diagnNAU.nBt = 27;
	ObjBPN[11]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[11]->diagnSUM.nBt = 27;
	ObjBPN[11]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������ 
	ObjBPN[11]->parBPN = 0;				// �������� ������
	ObjBPN[11]->zadBPN = 0;				// ������� �� �����
	ObjBPN[11]->tekBPN = 0;				// ������ ��������
	ObjBPN[11]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[11]->vBPN = 0;				// ������� ������ �� �����		
	
	// �������� ������� ������ 13
	ObjBPN[12]->nR1 = 62;				// ����� ��������� ������
	ObjBPN[12]->nR0 = 63;				// ����� ���������� ������
	ObjBPN[12]->normaOnNmb = 82;		// ��� ����� ������ �� ����� ������
	ObjBPN[12]->normaOffNmb = 83;		// ��� ����� ���������� ������
	ObjBPN[12]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[12]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[12]->diagnNVR.nBt = 27;
	ObjBPN[12]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ������
	ObjBPN[12]->diagnBOR.nBt = 27;
	ObjBPN[12]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ������
	ObjBPN[12]->diagnNR.nBt = 27;
	ObjBPN[12]->diagnNR.bMask = 0x40;	// ��� ������������� ������
	ObjBPN[12]->diagnNAU.nBt = 27;
	ObjBPN[12]->diagnNAU.bMask = 0x80;	// ��� ������ �� ������� �������
	ObjBPN[12]->diagnSUM.nBt = 27;
	ObjBPN[12]->diagnSUM.bMask = 0xF0;	// ��������� ����������� ������ 
	ObjBPN[12]->parBPN = 0;				// �������� ������
	ObjBPN[12]->zadBPN = 0;				// ������� �� �����
	ObjBPN[12]->tekBPN = 0;				// ������ ��������
	ObjBPN[12]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[12]->vBPN = 0;				// ������� ������ �� �����

	// �������� ������� ������ 14
	ObjBPN[13]->nR1 = 64;				// ����� ��������� ������
	ObjBPN[13]->nR0 = 65;				// ����� ���������� ������
	ObjBPN[13]->normaOnNmb = 84;		// ��� ����� ������ �� ����� ������
	ObjBPN[13]->normaOffNmb = 85;		// ��� ����� ���������� ������
	ObjBPN[13]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[13]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[13]->diagnNVR.nBt = 28;
	ObjBPN[13]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[13]->diagnBOR.nBt = 28;
	ObjBPN[13]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[13]->diagnNR.nBt = 28;
	ObjBPN[13]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[13]->diagnNAU.nBt = 28;
	ObjBPN[13]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[13]->diagnSUM.nBt = 28;
	ObjBPN[13]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������ 
	ObjBPN[13]->parBPN = 0;				// �������� ������
	ObjBPN[13]->zadBPN = 0;				// ������� �� �����
	ObjBPN[13]->tekBPN = 0;				// ������ ��������
	ObjBPN[13]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[13]->vBPN = 0;				// ������� ������ �� �����		
	
	// �������� ������� ������ 15
	ObjBPN[14]->nR1 = 66;				// ����� ��������� ������
	ObjBPN[14]->nR0 = 67;				// ����� ���������� ������
	ObjBPN[14]->normaOnNmb = 86;		// ��� ����� ������ �� ����� ������
	ObjBPN[14]->normaOffNmb = 87;		// ��� ����� ���������� ������
	ObjBPN[14]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[14]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[14]->diagnNVR.nBt = 28;
	ObjBPN[14]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ������
	ObjBPN[14]->diagnBOR.nBt = 28;
	ObjBPN[14]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ������
	ObjBPN[14]->diagnNR.nBt = 28;
	ObjBPN[14]->diagnNR.bMask = 0x40;	// ��� ������������� ������
	ObjBPN[14]->diagnNAU.nBt = 28;
	ObjBPN[14]->diagnNAU.bMask = 0x80;	// ��� ������ �� ������� �������
	ObjBPN[14]->diagnSUM.nBt = 28;
	ObjBPN[14]->diagnSUM.bMask = 0xF0;	// ��������� ����������� ������ 
	ObjBPN[14]->parBPN = 0;				// �������� ������
	ObjBPN[14]->zadBPN = 0;				// ������� �� �����
	ObjBPN[14]->tekBPN = 0;				// ������ ��������
	ObjBPN[14]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[14]->vBPN = 0;				// ������� ������ �� �����		

	// �������� ������� ������ 16
	ObjBPN[15]->nR1 = 68;				// ����� ��������� ������
	ObjBPN[15]->nR0 = 69;				// ����� ���������� ������
	ObjBPN[15]->normaOnNmb = 88;		// ��� ����� ������ �� ����� ������
	ObjBPN[15]->normaOffNmb = 89;		// ��� ����� ���������� ������
	ObjBPN[15]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[15]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[15]->diagnNVR.nBt = 29;
	ObjBPN[15]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[15]->diagnBOR.nBt = 29;
	ObjBPN[15]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[15]->diagnNR.nBt = 29;
	ObjBPN[15]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[15]->diagnNAU.nBt = 29;
	ObjBPN[15]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[15]->diagnSUM.nBt = 29;
	ObjBPN[15]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������ 
	ObjBPN[15]->parBPN = 0;				// �������� ������
	ObjBPN[15]->zadBPN = 0;				// ������� �� �����
	ObjBPN[15]->tekBPN = 0;				// ������ ��������
	ObjBPN[15]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[15]->vBPN = 0;				// ������� ������ �� �����		
	
	// �������� ������� ������ 17
	ObjBPN[16]->nR1 = 70;				// ����� ��������� ������
	ObjBPN[16]->nR0 = 71;				// ����� ���������� ������
	ObjBPN[16]->normaOnNmb = 90;		// ��� ����� ������ �� ����� ������
	ObjBPN[16]->normaOffNmb = 91;		// ��� ����� ���������� ������
	ObjBPN[16]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[16]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[16]->diagnNVR.nBt = 29;
	ObjBPN[16]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ������
	ObjBPN[16]->diagnBOR.nBt = 29;
	ObjBPN[16]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ������
	ObjBPN[16]->diagnNR.nBt = 29;
	ObjBPN[16]->diagnNR.bMask = 0x40;	// ��� ������������� ������
	ObjBPN[16]->diagnNAU.nBt = 29;
	ObjBPN[16]->diagnNAU.bMask = 0x80;	// ��� ������ �� ������� �������
	ObjBPN[16]->diagnSUM.nBt = 29;
	ObjBPN[16]->diagnSUM.bMask = 0xF0;	// ��������� ����������� ������ 
	ObjBPN[16]->parBPN = 0;				// �������� ������
	ObjBPN[16]->zadBPN = 0;				// ������� �� �����
	ObjBPN[16]->tekBPN = 0;				// ������ ��������
	ObjBPN[16]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[16]->vBPN = 0;				// ������� ������ �� �����

	// �������� ������� ������ 18
	ObjBPN[17]->nR1 = 72;				// ����� ��������� ������
	ObjBPN[17]->nR0 = 73;				// ����� ���������� ������
	ObjBPN[17]->normaOnNmb = 92;		// ��� ����� ������ �� ����� ������
	ObjBPN[17]->normaOffNmb = 93;		// ��� ����� ���������� ������
	ObjBPN[17]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[17]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[17]->diagnNVR.nBt = 30;
	ObjBPN[17]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[17]->diagnBOR.nBt = 30;
	ObjBPN[17]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[17]->diagnNR.nBt = 30;
	ObjBPN[17]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[17]->diagnNAU.nBt = 30;
	ObjBPN[17]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[17]->diagnSUM.nBt = 30;
	ObjBPN[17]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������ 
	ObjBPN[17]->parBPN = 0;				// �������� ������
	ObjBPN[17]->zadBPN = 0;				// ������� �� �����
	ObjBPN[17]->tekBPN = 0;				// ������ ��������
	ObjBPN[17]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[17]->vBPN = 0;				// ������� ������ �� �����
	
	// �������� ������� ������ 19
	ObjBPN[18]->nR1 = 74;				// ����� ��������� ������
	ObjBPN[18]->nR0 = 75;				// ����� ���������� ������
	ObjBPN[18]->normaOnNmb = 94;		// ��� ����� ������ �� ����� ������
	ObjBPN[18]->normaOffNmb = 95;		// ��� ����� ���������� ������
	ObjBPN[18]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[18]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[18]->diagnNVR.nBt = 30;
	ObjBPN[18]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ������
	ObjBPN[18]->diagnBOR.nBt = 30;
	ObjBPN[18]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ������
	ObjBPN[18]->diagnNR.nBt = 30;
	ObjBPN[18]->diagnNR.bMask = 0x40;	// ��� ������������� ������
	ObjBPN[18]->diagnNAU.nBt = 30;
	ObjBPN[18]->diagnNAU.bMask = 0x80;	// ��� ������ �� ������� �������
	ObjBPN[18]->diagnSUM.nBt = 30;
	ObjBPN[18]->diagnSUM.bMask = 0xF0;	// ��������� ����������� ������ 
	ObjBPN[18]->parBPN = 0;				// �������� ������
	ObjBPN[18]->zadBPN = 0;				// ������� �� �����
	ObjBPN[18]->tekBPN = 0;				// ������ ��������
	ObjBPN[18]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[18]->vBPN = 0;				// ������� ������ �� �����

	// �������� ������� ������ 20
	ObjBPN[19]->nR1 = 76;				// ����� ��������� ������
	ObjBPN[19]->nR0 = 77;				// ����� ���������� ������
	ObjBPN[19]->normaOnNmb = 96;		// ��� ����� ������ �� ����� ������
	ObjBPN[19]->normaOffNmb = 97;		// ��� ����� ���������� ������
	ObjBPN[19]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[19]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[19]->diagnNVR.nBt = 31;
	ObjBPN[19]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[19]->diagnBOR.nBt = 31;
	ObjBPN[19]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[19]->diagnNR.nBt = 31;
	ObjBPN[19]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[19]->diagnNAU.nBt = 31;
	ObjBPN[19]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[19]->diagnSUM.nBt = 31;
	ObjBPN[19]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������ 
	ObjBPN[19]->parBPN = 0;				// �������� ������
	ObjBPN[19]->zadBPN = 0;				// ������� �� �����
	ObjBPN[19]->tekBPN = 0;				// ������ ��������
	ObjBPN[19]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[19]->vBPN = 0;				// ������� ������ �� �����

	// �������� ������� ������ 21
	ObjBPN[20]->nR1 = 78;				// ����� ��������� ������
	ObjBPN[20]->nR0 = 79;				// ����� ���������� ������
	ObjBPN[20]->normaOnNmb = 98;		// ��� ����� ������ �� ����� ������
	ObjBPN[20]->normaOffNmb = 99;		// ��� ����� ���������� ������
	ObjBPN[20]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[20]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[20]->diagnNVR.nBt = 31;
	ObjBPN[20]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ������
	ObjBPN[20]->diagnBOR.nBt = 31;
	ObjBPN[20]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ������
	ObjBPN[20]->diagnNR.nBt = 31;
	ObjBPN[20]->diagnNR.bMask = 0x40;	// ��� ������������� ������
	ObjBPN[20]->diagnNAU.nBt = 31;
	ObjBPN[20]->diagnNAU.bMask = 0x80;	// ��� ������ �� ������� �������
	ObjBPN[20]->diagnSUM.nBt = 31;
	ObjBPN[20]->diagnSUM.bMask = 0xF0;	// ��������� ����������� ������
	ObjBPN[20]->parBPN = 0;				// �������� ������
	ObjBPN[20]->zadBPN = 0;				// ������� �� �����
	ObjBPN[20]->tekBPN = 0;				// ������ ��������
	ObjBPN[20]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[20]->vBPN = 0;				// ������� ������ �� �����

	// �������� ������� ������ 22 (������ ������ �������)
	ObjBPN[21]->parBPN = 0;				// �������� ������
	ObjBPN[21]->zadBPN = 0;				// ������� �� �����
	ObjBPN[21]->tekBPN = 0;				// ������ ��������
	ObjBPN[21]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[21]->vBPN = 0;				// ������� ������ �� �����

    // �������� ������� ������ 23
	ObjBPN[22]->nR1 = 80;				// ����� ��������� ������
	ObjBPN[22]->nR0 = 81;				// ����� ���������� ������
	ObjBPN[22]->normaOnNmb = 104;		// ��� ����� ������ �� ����� ������
	ObjBPN[22]->normaOffNmb = 105;		// ��� ����� ���������� ������
	ObjBPN[22]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[22]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[22]->diagnNVR.nBt = 23;
	ObjBPN[22]->diagnNVR.bMask = 0x01;	// ��� ������ �� ����� ������
	ObjBPN[22]->diagnBOR.nBt = 23;
	ObjBPN[22]->diagnBOR.bMask = 0x02;	// ������� ������ ������������� ������
	ObjBPN[22]->diagnNR.nBt = 23;
	ObjBPN[22]->diagnNR.bMask = 0x04;	// ��� ������������� ������
	ObjBPN[22]->diagnNAU.nBt = 23;
	ObjBPN[22]->diagnNAU.bMask = 0x08;	// ��� ������ �� ������� �������
	ObjBPN[22]->diagnSUM.nBt = 23;
	ObjBPN[22]->diagnSUM.bMask = 0x0F;	// ��������� ����������� ������
	ObjBPN[22]->parBPN = 0;				// �������� ������
	ObjBPN[22]->zadBPN = 0;				// ������� �� �����
	ObjBPN[22]->tekBPN = 0;				// ������ ��������
	ObjBPN[22]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[22]->vBPN = 0;				// ������� ������ �� �����

	// �������� ������� ������ 24
	ObjBPN[23]->nR1 = 82;				// ����� ��������� ������
	ObjBPN[23]->nR0 = 83;				// ����� ���������� ������
	ObjBPN[23]->normaOnNmb = 106;		// ��� ����� ������ �� ����� ������
	ObjBPN[23]->normaOffNmb = 107;		// ��� ����� ���������� ������
	ObjBPN[23]->ctVR = 0;				// ������� ������ �� �����
	ObjBPN[23]->ctVRK = 0;				// ����������� ������� ������ �� �����
	ObjBPN[23]->diagnNVR.nBt = 23;
	ObjBPN[23]->diagnNVR.bMask = 0x10;	// ��� ������ �� ����� ������
	ObjBPN[23]->diagnBOR.nBt = 23;
	ObjBPN[23]->diagnBOR.bMask = 0x20;	// ������� ������ ������������� ������
	ObjBPN[23]->diagnNR.nBt = 23;
	ObjBPN[23]->diagnNR.bMask = 0x40;	// ��� ������������� ������
	ObjBPN[23]->diagnNAU.nBt = 23;
	ObjBPN[23]->diagnNAU.bMask = 0x80;	// ��� ������ �� ������� �������
	ObjBPN[23]->diagnSUM.nBt = 23;
	ObjBPN[23]->diagnSUM.bMask = 0xF0;	// ��������� ����������� ������
	ObjBPN[23]->parBPN = 0;				// �������� ������
	ObjBPN[23]->zadBPN = 0;				// ������� �� �����
	ObjBPN[23]->tekBPN = 0;				// ������ ��������
	ObjBPN[23]->prBPN = 0;				// ������� �������������� ������
	ObjBPN[23]->vBPN = 0;				// ������� ������ �� �����
}
