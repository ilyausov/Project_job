 //---------------------------------------------------------------------------
//--Файл описания шаблонных функций--//
//---------------------------------------------------------------------------
#include "BPN.h"
//---------------------------------------------------------------------------
//--Функция включения канала--//
//---------------------------------------------------------------------------
void BPNOn(SBPN *object)
{
	switch (shr[object->nR1])	// анализируем шаг режима
	{
		case 0: ;break;			// 0 - нет режима
		case 1:
		{
			shr[object->nR0] = 0;	// сброс режима отключения
			object->vBPN = 0;		// сброс признака выхода канала на режим
			diagn[object->diagnSUM.nBt]&=(~object->diagnSUM.bMask);// сброс диагностик режима
			shr[object->nR1]++;	// переход на след. шаг
		}; break;
		case 2:
		{
			if(!(diagn[diagnNA.nBt]&diagnNA.bMask))	// нет диагностики "Нет связи с ТЕРМОДАТ"
			{
				object->prBPN = 0;	// сброс признака положительного ответа
				object->zadBPN = object->parBPN; // запись задания
				object->ctVRK = 0;	// сброс счетчика ответа на команду
				shr[object->nR1]++;	// переход на след. шаг
			}
		}; break;
		case 3:
		{
			if((diagn[diagnNA.nBt]&diagnNA.bMask)||(!object->prBPN))
			{ // нет свзяи с ТЕРМОДАТ или нет положительного ответа задания уставки по каналу
				if(object->ctVRK > BPN_tkVRK) // вышло контр. время
					diagn[object->diagnNAU.nBt]|=(object->diagnNAU.bMask);
					// выдать диагностику нет ответа на запись уставки
			}
			else
			{
				diagn[object->diagnNAU.nBt]&=(~object->diagnNAU.bMask);// сброс диагностики
				object->ctVR = 0;	// сброс счетчика выхода на режим
				object->ctVRK = 0;
				shr[object->nR1]++;	// переход на след. шаг
			}
		}; break;
		case 4:
		{
			// нет нового задания на канал
			if(object->zadBPN == object->parBPN)
			{
				// если канала в диапазоне
				if(abs(object->tekBPN - object->zadBPN) <= (float(object->zadBPN)*BPN_porog1))
				{
					// если канала находится на режиме контрольное время
					if(object->ctVRK > BPN_tkVRV)
					{
						diagn[object->diagnNVR.nBt]&=(~object->diagnNVR.bMask);// сброс диагностики
						object->vBPN = 1;			// выставить признак выхода на режиме
						norma = object->normaOnNmb;	// норма: "канал вышло на режим"
						shr[object->nR1]++;			// переход на след. шаг						
					}
                }
                else
                {
                    object->ctVRK = 0;	// сброс контрольного счетчика выхода на режим
                    if (object->ctVR > BPN_tkVR)	// время вышло
                        diagn[object->diagnNVR.nBt] |= object->diagnNVR.bMask;
                        // отказ: "Нет выхода канала на режим"	
                }
			}
			// есть новое задания на канал
			else
			{
				shr[object->nR1] = 2; // переход на шаг 2
			}
		}; break;
		case 5:
		{
			// нет нового задания на канал
			if(object->zadBPN == object->parBPN)
			{
				// есть б.ош.регул. канала
				if (abs(object->tekBPN - object->zadBPN)>(float(object->zadBPN)*BPN_porog1))
				{
					// есть регулирование канала
					if (abs(object->tekBPN - object->zadBPN)<(float(object->zadBPN)*BPN_porog2))
					{
						diagn[object->diagnBOR.nBt]|=object->diagnBOR.bMask;	// отказ: "б.ош.регул.канала"
						diagn[object->diagnNR.nBt]&=(~object->diagnNR.bMask);	// сброс: "нет регул.канала"
					}
					else
					{
						diagn[object->diagnNR.nBt]|=object->diagnNR.bMask;		// отказ: "нет регул.канала"
						diagn[object->diagnBOR.nBt]&=(~object->diagnBOR.bMask);	// сброс: "б.ош.регул.канала"
					}
				}
				else
				{
					diagn[object->diagnNR.nBt]&=(~object->diagnNR.bMask);		// сброс: "нет регул.канала"
					diagn[object->diagnBOR.nBt]&=(~object->diagnBOR.bMask);		// сброс: "б.ош.регул.канала"
				}
			}
			// есть новое задания на канал
			else
			{
				shr[object->nR1] = 2; // переход на шаг 2
			}
		}; break;
		default: // невозможный шаг
		{
			sh[object->nR1]  = 0;
			shr[object->nR1] = 0;
		}; break;
	}
}
//---------------------------------------------------------------------------
//--Функция отключения канала--//
//---------------------------------------------------------------------------
void BPNOff(SBPN *object)
{
	switch (shr[object->nR0])	// анализируем шаг режима
	{
		case 0: ;break;			// 0 - нет режима
		case 1:
		{
			shr[object->nR1] = 0;	// сброс режима отключения
			diagn[object->diagnSUM.nBt]&=(~object->diagnSUM.bMask);// сброс диагностик режима
			shr[object->nR0]++;	// переход на след. шаг
		}; break;
		case 2:
		{
			if(!(diagn[diagnNA.nBt]&diagnNA.bMask))	// нет диагностики "Нет связи с ТЕРМОДАТ"
			{
				object->prBPN = 0;	// сброс признака положительного ответа
				object->zadBPN = 0; // запись задания
				object->ctVRK = 0;	// сброс счетчика ответа на команду
				shr[object->nR0]++;	// переход на след. шаг
			}
		}; break;
		case 3:
		{
			if((diagn[diagnNA.nBt]&diagnNA.bMask)||(!object->prBPN))
			{ // нет свзяи с ТЕРМОДАТ или нет положительного ответа задания уставки по каналу
				if(object->ctVRK > BPN_tkVRK) // вышло контр. время
					diagn[object->diagnNAU.nBt]|=(object->diagnNAU.bMask);
					// выдать диагностику нет ответа на запись уставки
			}
			else
			{
				diagn[object->diagnNAU.nBt]&=(~object->diagnNAU.bMask);// сброс диагностики
				object->parBPN = 0; // сбросить параметр
				object->vBPN = 0; // сбросить признак выхода на режиме
				norma = object->normaOffNmb;	// норма: "канал выключен"
				shr[object->nR0] = 0; // конец режима
			}
		}; break;
		default: // невозможный шаг
		{
			sh[object->nR0]  = 0;
			shr[object->nR0] = 0;
		}; break;
    }
}
//---------------------------------------------------------------------------
//--Функция инкремента счетчиков--//
//---------------------------------------------------------------------------
void TimeBPN()
{
	for(int i=0;i<qBPN;i++)
	{
		ObjBPN[i]->ctVR++;	// счетчик выхода на режим
		ObjBPN[i]->ctVRK++;	// контрольный счетчик выхода на режим
	}
	
	CT_BPN++;
}
//---------------------------------------------------------------------------
//--Функция "Включить БПН (сил. пит. на термодат)" --//
//---------------------------------------------------------------------------
void VBPN()
{
	switch (sh_)	// анализируем шаг
	{
		case 0: ;break;			// 0 - нет режима
		case 1:
		{
			SetOut(0,BPN_OnOut.nBt,BPN_OnOut.bMask); // сброс выходных сигналов на вкл/откл
			SetOut(0,BPN_OffOut.nBt,BPN_OffOut.bMask);
			CT_BPN = 0; // Сброс сч. времени
			sh_ = 2;
		}; break;
		case 2:
		{
			if(CT_BPN > T_VKL_BPN) // задержка на откл. реле и пускателя
				sh_ = 3;
		}; break;
		case 3:
		{
			if(zin[BPN_OnZin.nBt] & BPN_OnZin.bMask) // силовое питание БПН включено
			{
				diagn[diagnNON.nBt] &=(~ diagnNON.bMask); // сброс диагностики
				diagn[diagnNOFF.nBt] &=(~ diagnNOFF.bMask); // сброс диагностики
				sh_ = 0;
			}
			else
			{
				SetOut(1,BPN_OnOut.nBt,BPN_OnOut.bMask); // вкл. силовое питание БПН
				CT_BPN = 0; // сброс сч. времени
				sh_ = 4;	
			}
		}; break;
		case 4:
		{
			if(!(zin[BPN_OnZin.nBt] & BPN_OnZin.bMask)) // нет включения силового питания БПН
			{
				if (CT_BPN<=T_VKL_BPN) return; // время не вышло
					diagn[diagnNON.nBt] |= diagnNON.bMask; // отказ:"Силовое питание БПН не выкл."
			}
			else
			{
				SetOut(0,BPN_OnOut.nBt,BPN_OnOut.bMask); // сброс выходных сигналов на вкл/откл
				SetOut(0,BPN_OffOut.nBt,BPN_OffOut.bMask);
				diagn[diagnNON.nBt] &=(~ diagnNON.bMask); // сброс диагностики
				sh_ = 0;	
			}
		}; break;
	}
}
//---------------------------------------------------------------------------
//--Функция "Отключить силовое питание БПН" --//
//---------------------------------------------------------------------------
void OBPN()
{
	switch (sh_) // анализируем шаг
	{
		case 0: ;break;			// 0 - нет режима
		case 1:
		{
			SetOut(0,BPN_OnOut.nBt,BPN_OnOut.bMask); // сброс выходных сигналов на вкл/откл
			SetOut(0,BPN_OffOut.nBt,BPN_OffOut.bMask);
			CT_BPN = 0; // Сброс сч. времени
			sh_ = 2;
		}; break;
		case 2:
		{
			if(CT_BPN > T_VKL_BPN) // задержка на откл. реле и пускателя
				sh_ = 3;
		}; break;
		case 3:
		{
			if(!(zin[BPN_OnZin.nBt] & BPN_OnZin.bMask)) // силовое питание БПН отключено
			{
				diagn[diagnNON.nBt] &=(~ diagnNON.bMask); // сброс диагностики
				diagn[diagnNOFF.nBt] &=(~ diagnNOFF.bMask); // сброс диагностики
				sh_ = 0;
			}
			else
			{
				SetOut(1,BPN_OffOut.nBt,BPN_OffOut.bMask); // откл. силовое питание БПН
				CT_BPN = 0; // сброс сч. времени
				sh_ = 4;	
			}
		}; break;
		case 4:
		{
			if(zin[BPN_OnZin.nBt] & BPN_OnZin.bMask) // есть включения силового питания БПН
			{
				if (CT_BPN<=T_VKL_BPN) return; // время не вышло
					diagn[diagnNOFF.nBt] |= diagnNOFF.bMask; // отказ:"Силовое питание БПН не выкл."
			}
			else
			{
				SetOut(0,BPN_OnOut.nBt,BPN_OnOut.bMask); // сброс выходных сигналов на вкл/откл
				SetOut(0,BPN_OffOut.nBt,BPN_OffOut.bMask);
				diagn[diagnNOFF.nBt] &=(~ diagnNOFF.bMask); // сброс диагностики
				sh_ = 0;	
			}
		}; break;
	}
}
//---------------------------------------------------------------------------
//--Функция инициализации объектов типа канала--//
//---------------------------------------------------------------------------
void InitObjectsBPN()
{
	// описание свойств БПН
	BPN_normaOn = 20; // код нормы выхода на режима
	BPN_normaOff = 21; // код нормы отключения режима
	BPN_OnOut.nBt = 4; // выходной дискретный сигнал включения пускателя
	BPN_OnOut.bMask = 0x01;
	BPN_OffOut.nBt = 4; // выходной дискретный сигнал отключения пускателя
	BPN_OffOut.bMask = 0x02;
	BPN_OnZin.nBt = 0; // входной дискретный сигнал пускателя
	BPN_OnZin.bMask = 0x80;
	diagnNA.nBt = 0; // нет связи
	diagnNA.bMask = 0x20;
	diagnNON.nBt = 16; // нет включения пускателя
	diagnNON.bMask = 0x08;
	diagnNOFF.nBt = 16; // нет отключения пускателя	
	diagnNOFF.bMask = 0x10;
	
	// описание свойств каналов
    for (unsigned int i = 0 ; i < qBPN ; i++ )
        ObjBPN[i] = new SBPN();
	
	// Описание свойств канала 1
	ObjBPN[0]->nR1 = 40;				// номер включения режима
	ObjBPN[0]->nR0 = 41;				// номер выключения режима
	ObjBPN[0]->normaOnNmb = 60;			// код нормы выхода на режим канала
	ObjBPN[0]->normaOffNmb = 61;		// код нормы отключения канала
	ObjBPN[0]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[0]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[0]->diagnNVR.nBt = 22;
	ObjBPN[0]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[0]->diagnBOR.nBt = 22;
	ObjBPN[0]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[0]->diagnNR.nBt = 22;
	ObjBPN[0]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[0]->diagnNAU.nBt = 22;
	ObjBPN[0]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[0]->diagnSUM.nBt = 22;
	ObjBPN[0]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима 
	ObjBPN[0]->parBPN = 0;				// параметр канала
	ObjBPN[0]->zadBPN = 0;				// задание на канал
	ObjBPN[0]->tekBPN = 0;				// текуще значение
	ObjBPN[0]->prBPN = 0;				// признак положительного ответа
	ObjBPN[0]->vBPN = 0;				// признак выхода на режим
	
	// Описание свойств канала 2
	ObjBPN[1]->nR1 = 42;				// номер включения режима
	ObjBPN[1]->nR0 = 43;				// номер выключения режима
	ObjBPN[1]->normaOnNmb = 62;			// код нормы выхода на режим канала
	ObjBPN[1]->normaOffNmb = 63;		// код нормы отключения канала
	ObjBPN[1]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[1]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[1]->diagnNVR.nBt = 22;
	ObjBPN[1]->diagnNVR.bMask = 0x10;	// нет выхода на режим канала
	ObjBPN[1]->diagnBOR.nBt = 22;
	ObjBPN[1]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования канала
	ObjBPN[1]->diagnNR.nBt = 22;
	ObjBPN[1]->diagnNR.bMask = 0x40;	// нет регулирования канала
	ObjBPN[1]->diagnNAU.nBt = 22;
	ObjBPN[1]->diagnNAU.bMask = 0x80;	// нет ответа на задание уставки
	ObjBPN[1]->diagnSUM.nBt = 22;
	ObjBPN[1]->diagnSUM.bMask = 0xF0;	// суммарные диагностики режима 
	ObjBPN[1]->parBPN = 0;				// параметр канала
	ObjBPN[1]->zadBPN = 0;				// задание на канал
	ObjBPN[1]->tekBPN = 0;				// текуще значение
	ObjBPN[1]->prBPN = 0;				// признак положительного ответа
	ObjBPN[1]->vBPN = 0;				// признак выхода на режим

    // Канал 3 не используется!
	// Описание свойств канала 3
	ObjBPN[2]->parBPN = 0;				// параметр канала
	ObjBPN[2]->zadBPN = 0;				// задание на канал
	ObjBPN[2]->tekBPN = 0;				// текуще значение
	ObjBPN[2]->prBPN = 0;				// признак положительного ответа
	ObjBPN[2]->vBPN = 0;				// признак выхода на режим

	// Канал 4 не используется!
	// Описание свойств канала 4
	ObjBPN[3]->parBPN = 0;				// параметр канала
	ObjBPN[3]->zadBPN = 0;				// задание на канал
	ObjBPN[3]->tekBPN = 0;				// текуще значение
	ObjBPN[3]->prBPN = 0;				// признак положительного ответа
	ObjBPN[3]->vBPN = 0;				// признак выхода на режим		

	// Канал 5
	// Описание свойств канала 5
	ObjBPN[4]->nR1 = 84;				// номер включения режима
	ObjBPN[4]->nR0 = 85;				// номер выключения режима
	ObjBPN[4]->normaOnNmb = 112;			// код нормы выхода на режим канала
	ObjBPN[4]->normaOffNmb = 113;		// код нормы отключения канала
	ObjBPN[4]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[4]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[4]->diagnNVR.nBt = 35;
	ObjBPN[4]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[4]->diagnBOR.nBt = 35;
	ObjBPN[4]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[4]->diagnNR.nBt = 35;
	ObjBPN[4]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[4]->diagnNAU.nBt = 35;
	ObjBPN[4]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[4]->diagnSUM.nBt = 35;
	ObjBPN[4]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима
	ObjBPN[4]->parBPN = 0;				// параметр канала
	ObjBPN[4]->zadBPN = 0;				// задание на канал
	ObjBPN[4]->tekBPN = 0;				// текуще значение
	ObjBPN[4]->prBPN = 0;				// признак положительного ответа
	ObjBPN[4]->vBPN = 0;				// признак выхода на режим

	// Описание свойств канала 6
	ObjBPN[5]->nR1 = 48;				// номер включения режима
	ObjBPN[5]->nR0 = 49;				// номер выключения режима
	ObjBPN[5]->normaOnNmb = 68;			// код нормы выхода на режим канала
	ObjBPN[5]->normaOffNmb = 69;		// код нормы отключения канала
	ObjBPN[5]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[5]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[5]->diagnNVR.nBt = 24;
	ObjBPN[5]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[5]->diagnBOR.nBt = 24;
	ObjBPN[5]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[5]->diagnNR.nBt = 24;
	ObjBPN[5]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[5]->diagnNAU.nBt = 24;
	ObjBPN[5]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[5]->diagnSUM.nBt = 24;
	ObjBPN[5]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима 
	ObjBPN[5]->parBPN = 0;				// параметр канала
	ObjBPN[5]->zadBPN = 0;				// задание на канал
	ObjBPN[5]->tekBPN = 0;				// текуще значение
	ObjBPN[5]->prBPN = 0;				// признак положительного ответа
	ObjBPN[5]->vBPN = 0;				// признак выхода на режим		
	
	// Описание свойств канала 7
	ObjBPN[6]->nR1 = 50;				// номер включения режима
	ObjBPN[6]->nR0 = 51;				// номер выключения режима
	ObjBPN[6]->normaOnNmb = 70;			// код нормы выхода на режим канала
	ObjBPN[6]->normaOffNmb = 71;		// код нормы отключения канала
	ObjBPN[6]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[6]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[6]->diagnNVR.nBt = 24;
	ObjBPN[6]->diagnNVR.bMask = 0x10;	// нет выхода на режим канала
	ObjBPN[6]->diagnBOR.nBt = 24;
	ObjBPN[6]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования канала
	ObjBPN[6]->diagnNR.nBt = 24;
	ObjBPN[6]->diagnNR.bMask = 0x40;	// нет регулирования канала
	ObjBPN[6]->diagnNAU.nBt = 24;
	ObjBPN[6]->diagnNAU.bMask = 0x80;	// нет ответа на задание уставки
	ObjBPN[6]->diagnSUM.nBt = 24;
	ObjBPN[6]->diagnSUM.bMask = 0xF0;	// суммарные диагностики режима 
	ObjBPN[6]->parBPN = 0;				// параметр канала
	ObjBPN[6]->zadBPN = 0;				// задание на канал
	ObjBPN[6]->tekBPN = 0;				// текуще значение
	ObjBPN[6]->prBPN = 0;				// признак положительного ответа
	ObjBPN[6]->vBPN = 0;				// признак выхода на режим			
	
	// Описание свойств канала 8
	ObjBPN[7]->nR1 = 52;				// номер включения режима
	ObjBPN[7]->nR0 = 53;				// номер выключения режима
	ObjBPN[7]->normaOnNmb = 72;			// код нормы выхода на режим канала
	ObjBPN[7]->normaOffNmb = 73;		// код нормы отключения канала
	ObjBPN[7]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[7]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[7]->diagnNVR.nBt = 25;
	ObjBPN[7]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[7]->diagnBOR.nBt = 25;
	ObjBPN[7]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[7]->diagnNR.nBt = 25;
	ObjBPN[7]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[7]->diagnNAU.nBt = 25;
	ObjBPN[7]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[7]->diagnSUM.nBt = 25;
	ObjBPN[7]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима 
	ObjBPN[7]->parBPN = 0;				// параметр канала
	ObjBPN[7]->zadBPN = 0;				// задание на канал
	ObjBPN[7]->tekBPN = 0;				// текуще значение
	ObjBPN[7]->prBPN = 0;				// признак положительного ответа
	ObjBPN[7]->vBPN = 0;				// признак выхода на режим		
	
	// Описание свойств канала 9
	ObjBPN[8]->nR1 = 54;				// номер включения режима
	ObjBPN[8]->nR0 = 55;				// номер выключения режима
	ObjBPN[8]->normaOnNmb = 74;			// код нормы выхода на режим канала
	ObjBPN[8]->normaOffNmb = 75;		// код нормы отключения канала
	ObjBPN[8]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[8]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[8]->diagnNVR.nBt = 25;
	ObjBPN[8]->diagnNVR.bMask = 0x10;	// нет выхода на режим канала
	ObjBPN[8]->diagnBOR.nBt = 25;
	ObjBPN[8]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования канала
	ObjBPN[8]->diagnNR.nBt = 25;
	ObjBPN[8]->diagnNR.bMask = 0x40;	// нет регулирования канала
	ObjBPN[8]->diagnNAU.nBt = 25;
	ObjBPN[8]->diagnNAU.bMask = 0x80;	// нет ответа на задание уставки
	ObjBPN[8]->diagnSUM.nBt = 25;
	ObjBPN[8]->diagnSUM.bMask = 0xF0;	// суммарные диагностики режима 
	ObjBPN[8]->parBPN = 0;				// параметр канала
	ObjBPN[8]->zadBPN = 0;				// задание на канал
	ObjBPN[8]->tekBPN = 0;				// текуще значение
	ObjBPN[8]->prBPN = 0;				// признак положительного ответа
	ObjBPN[8]->vBPN = 0;				// признак выхода на режим		
	
	// Описание свойств канала 10
	ObjBPN[9]->nR1 = 56;				// номер включения режима
	ObjBPN[9]->nR0 = 57;				// номер выключения режима
	ObjBPN[9]->normaOnNmb = 76;			// код нормы выхода на режим канала
	ObjBPN[9]->normaOffNmb = 77;		// код нормы отключения канала
	ObjBPN[9]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[9]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[9]->diagnNVR.nBt = 26;
	ObjBPN[9]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[9]->diagnBOR.nBt = 26;
	ObjBPN[9]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[9]->diagnNR.nBt = 26;
	ObjBPN[9]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[9]->diagnNAU.nBt = 26;
	ObjBPN[9]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[9]->diagnSUM.nBt = 26;
	ObjBPN[9]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима 
	ObjBPN[9]->parBPN = 0;				// параметр канала
	ObjBPN[9]->zadBPN = 0;				// задание на канал
	ObjBPN[9]->tekBPN = 0;				// текуще значение
	ObjBPN[9]->prBPN = 0;				// признак положительного ответа
	ObjBPN[9]->vBPN = 0;				// признак выхода на режим		
	
	// Описание свойств канала 11
	ObjBPN[10]->nR1 = 58;				// номер включения режима
	ObjBPN[10]->nR0 = 59;				// номер выключения режима
	ObjBPN[10]->normaOnNmb = 78;		// код нормы выхода на режим канала
	ObjBPN[10]->normaOffNmb = 79;		// код нормы отключения канала
	ObjBPN[10]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[10]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[10]->diagnNVR.nBt = 26;
	ObjBPN[10]->diagnNVR.bMask = 0x10;	// нет выхода на режим канала
	ObjBPN[10]->diagnBOR.nBt = 26;
	ObjBPN[10]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования канала
	ObjBPN[10]->diagnNR.nBt = 26;
	ObjBPN[10]->diagnNR.bMask = 0x40;	// нет регулирования канала
	ObjBPN[10]->diagnNAU.nBt = 26;
	ObjBPN[10]->diagnNAU.bMask = 0x80;	// нет ответа на задание уставки
	ObjBPN[10]->diagnSUM.nBt = 26;
	ObjBPN[10]->diagnSUM.bMask = 0xF0;	// суммарные диагностики режима 
	ObjBPN[10]->parBPN = 0;				// параметр канала
	ObjBPN[10]->zadBPN = 0;				// задание на канал
	ObjBPN[10]->tekBPN = 0;				// текуще значение
	ObjBPN[10]->prBPN = 0;				// признак положительного ответа
	ObjBPN[10]->vBPN = 0;				// признак выхода на режим		

	// Описание свойств канала 12
	ObjBPN[11]->nR1 = 60;				// номер включения режима
	ObjBPN[11]->nR0 = 61;				// номер выключения режима
	ObjBPN[11]->normaOnNmb = 80;		// код нормы выхода на режим канала
	ObjBPN[11]->normaOffNmb = 81;		// код нормы отключения канала
	ObjBPN[11]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[11]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[11]->diagnNVR.nBt = 27;
	ObjBPN[11]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[11]->diagnBOR.nBt = 27;
	ObjBPN[11]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[11]->diagnNR.nBt = 27;
	ObjBPN[11]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[11]->diagnNAU.nBt = 27;
	ObjBPN[11]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[11]->diagnSUM.nBt = 27;
	ObjBPN[11]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима 
	ObjBPN[11]->parBPN = 0;				// параметр канала
	ObjBPN[11]->zadBPN = 0;				// задание на канал
	ObjBPN[11]->tekBPN = 0;				// текуще значение
	ObjBPN[11]->prBPN = 0;				// признак положительного ответа
	ObjBPN[11]->vBPN = 0;				// признак выхода на режим		
	
	// Описание свойств канала 13
	ObjBPN[12]->nR1 = 62;				// номер включения режима
	ObjBPN[12]->nR0 = 63;				// номер выключения режима
	ObjBPN[12]->normaOnNmb = 82;		// код нормы выхода на режим канала
	ObjBPN[12]->normaOffNmb = 83;		// код нормы отключения канала
	ObjBPN[12]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[12]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[12]->diagnNVR.nBt = 27;
	ObjBPN[12]->diagnNVR.bMask = 0x10;	// нет выхода на режим канала
	ObjBPN[12]->diagnBOR.nBt = 27;
	ObjBPN[12]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования канала
	ObjBPN[12]->diagnNR.nBt = 27;
	ObjBPN[12]->diagnNR.bMask = 0x40;	// нет регулирования канала
	ObjBPN[12]->diagnNAU.nBt = 27;
	ObjBPN[12]->diagnNAU.bMask = 0x80;	// нет ответа на задание уставки
	ObjBPN[12]->diagnSUM.nBt = 27;
	ObjBPN[12]->diagnSUM.bMask = 0xF0;	// суммарные диагностики режима 
	ObjBPN[12]->parBPN = 0;				// параметр канала
	ObjBPN[12]->zadBPN = 0;				// задание на канал
	ObjBPN[12]->tekBPN = 0;				// текуще значение
	ObjBPN[12]->prBPN = 0;				// признак положительного ответа
	ObjBPN[12]->vBPN = 0;				// признак выхода на режим

	// Описание свойств канала 14
	ObjBPN[13]->nR1 = 64;				// номер включения режима
	ObjBPN[13]->nR0 = 65;				// номер выключения режима
	ObjBPN[13]->normaOnNmb = 84;		// код нормы выхода на режим канала
	ObjBPN[13]->normaOffNmb = 85;		// код нормы отключения канала
	ObjBPN[13]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[13]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[13]->diagnNVR.nBt = 28;
	ObjBPN[13]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[13]->diagnBOR.nBt = 28;
	ObjBPN[13]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[13]->diagnNR.nBt = 28;
	ObjBPN[13]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[13]->diagnNAU.nBt = 28;
	ObjBPN[13]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[13]->diagnSUM.nBt = 28;
	ObjBPN[13]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима 
	ObjBPN[13]->parBPN = 0;				// параметр канала
	ObjBPN[13]->zadBPN = 0;				// задание на канал
	ObjBPN[13]->tekBPN = 0;				// текуще значение
	ObjBPN[13]->prBPN = 0;				// признак положительного ответа
	ObjBPN[13]->vBPN = 0;				// признак выхода на режим		
	
	// Описание свойств канала 15
	ObjBPN[14]->nR1 = 66;				// номер включения режима
	ObjBPN[14]->nR0 = 67;				// номер выключения режима
	ObjBPN[14]->normaOnNmb = 86;		// код нормы выхода на режим канала
	ObjBPN[14]->normaOffNmb = 87;		// код нормы отключения канала
	ObjBPN[14]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[14]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[14]->diagnNVR.nBt = 28;
	ObjBPN[14]->diagnNVR.bMask = 0x10;	// нет выхода на режим канала
	ObjBPN[14]->diagnBOR.nBt = 28;
	ObjBPN[14]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования канала
	ObjBPN[14]->diagnNR.nBt = 28;
	ObjBPN[14]->diagnNR.bMask = 0x40;	// нет регулирования канала
	ObjBPN[14]->diagnNAU.nBt = 28;
	ObjBPN[14]->diagnNAU.bMask = 0x80;	// нет ответа на задание уставки
	ObjBPN[14]->diagnSUM.nBt = 28;
	ObjBPN[14]->diagnSUM.bMask = 0xF0;	// суммарные диагностики режима 
	ObjBPN[14]->parBPN = 0;				// параметр канала
	ObjBPN[14]->zadBPN = 0;				// задание на канал
	ObjBPN[14]->tekBPN = 0;				// текуще значение
	ObjBPN[14]->prBPN = 0;				// признак положительного ответа
	ObjBPN[14]->vBPN = 0;				// признак выхода на режим		

	// Описание свойств канала 16
	ObjBPN[15]->nR1 = 68;				// номер включения режима
	ObjBPN[15]->nR0 = 69;				// номер выключения режима
	ObjBPN[15]->normaOnNmb = 88;		// код нормы выхода на режим канала
	ObjBPN[15]->normaOffNmb = 89;		// код нормы отключения канала
	ObjBPN[15]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[15]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[15]->diagnNVR.nBt = 29;
	ObjBPN[15]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[15]->diagnBOR.nBt = 29;
	ObjBPN[15]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[15]->diagnNR.nBt = 29;
	ObjBPN[15]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[15]->diagnNAU.nBt = 29;
	ObjBPN[15]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[15]->diagnSUM.nBt = 29;
	ObjBPN[15]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима 
	ObjBPN[15]->parBPN = 0;				// параметр канала
	ObjBPN[15]->zadBPN = 0;				// задание на канал
	ObjBPN[15]->tekBPN = 0;				// текуще значение
	ObjBPN[15]->prBPN = 0;				// признак положительного ответа
	ObjBPN[15]->vBPN = 0;				// признак выхода на режим		
	
	// Описание свойств канала 17
	ObjBPN[16]->nR1 = 70;				// номер включения режима
	ObjBPN[16]->nR0 = 71;				// номер выключения режима
	ObjBPN[16]->normaOnNmb = 90;		// код нормы выхода на режим канала
	ObjBPN[16]->normaOffNmb = 91;		// код нормы отключения канала
	ObjBPN[16]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[16]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[16]->diagnNVR.nBt = 29;
	ObjBPN[16]->diagnNVR.bMask = 0x10;	// нет выхода на режим канала
	ObjBPN[16]->diagnBOR.nBt = 29;
	ObjBPN[16]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования канала
	ObjBPN[16]->diagnNR.nBt = 29;
	ObjBPN[16]->diagnNR.bMask = 0x40;	// нет регулирования канала
	ObjBPN[16]->diagnNAU.nBt = 29;
	ObjBPN[16]->diagnNAU.bMask = 0x80;	// нет ответа на задание уставки
	ObjBPN[16]->diagnSUM.nBt = 29;
	ObjBPN[16]->diagnSUM.bMask = 0xF0;	// суммарные диагностики режима 
	ObjBPN[16]->parBPN = 0;				// параметр канала
	ObjBPN[16]->zadBPN = 0;				// задание на канал
	ObjBPN[16]->tekBPN = 0;				// текуще значение
	ObjBPN[16]->prBPN = 0;				// признак положительного ответа
	ObjBPN[16]->vBPN = 0;				// признак выхода на режим

	// Описание свойств канала 18
	ObjBPN[17]->nR1 = 72;				// номер включения режима
	ObjBPN[17]->nR0 = 73;				// номер выключения режима
	ObjBPN[17]->normaOnNmb = 92;		// код нормы выхода на режим канала
	ObjBPN[17]->normaOffNmb = 93;		// код нормы отключения канала
	ObjBPN[17]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[17]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[17]->diagnNVR.nBt = 30;
	ObjBPN[17]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[17]->diagnBOR.nBt = 30;
	ObjBPN[17]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[17]->diagnNR.nBt = 30;
	ObjBPN[17]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[17]->diagnNAU.nBt = 30;
	ObjBPN[17]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[17]->diagnSUM.nBt = 30;
	ObjBPN[17]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима 
	ObjBPN[17]->parBPN = 0;				// параметр канала
	ObjBPN[17]->zadBPN = 0;				// задание на канал
	ObjBPN[17]->tekBPN = 0;				// текуще значение
	ObjBPN[17]->prBPN = 0;				// признак положительного ответа
	ObjBPN[17]->vBPN = 0;				// признак выхода на режим
	
	// Описание свойств канала 19
	ObjBPN[18]->nR1 = 74;				// номер включения режима
	ObjBPN[18]->nR0 = 75;				// номер выключения режима
	ObjBPN[18]->normaOnNmb = 94;		// код нормы выхода на режим канала
	ObjBPN[18]->normaOffNmb = 95;		// код нормы отключения канала
	ObjBPN[18]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[18]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[18]->diagnNVR.nBt = 30;
	ObjBPN[18]->diagnNVR.bMask = 0x10;	// нет выхода на режим канала
	ObjBPN[18]->diagnBOR.nBt = 30;
	ObjBPN[18]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования канала
	ObjBPN[18]->diagnNR.nBt = 30;
	ObjBPN[18]->diagnNR.bMask = 0x40;	// нет регулирования канала
	ObjBPN[18]->diagnNAU.nBt = 30;
	ObjBPN[18]->diagnNAU.bMask = 0x80;	// нет ответа на задание уставки
	ObjBPN[18]->diagnSUM.nBt = 30;
	ObjBPN[18]->diagnSUM.bMask = 0xF0;	// суммарные диагностики режима 
	ObjBPN[18]->parBPN = 0;				// параметр канала
	ObjBPN[18]->zadBPN = 0;				// задание на канал
	ObjBPN[18]->tekBPN = 0;				// текуще значение
	ObjBPN[18]->prBPN = 0;				// признак положительного ответа
	ObjBPN[18]->vBPN = 0;				// признак выхода на режим

	// Описание свойств канала 20
	ObjBPN[19]->nR1 = 76;				// номер включения режима
	ObjBPN[19]->nR0 = 77;				// номер выключения режима
	ObjBPN[19]->normaOnNmb = 96;		// код нормы выхода на режим канала
	ObjBPN[19]->normaOffNmb = 97;		// код нормы отключения канала
	ObjBPN[19]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[19]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[19]->diagnNVR.nBt = 31;
	ObjBPN[19]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[19]->diagnBOR.nBt = 31;
	ObjBPN[19]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[19]->diagnNR.nBt = 31;
	ObjBPN[19]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[19]->diagnNAU.nBt = 31;
	ObjBPN[19]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[19]->diagnSUM.nBt = 31;
	ObjBPN[19]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима 
	ObjBPN[19]->parBPN = 0;				// параметр канала
	ObjBPN[19]->zadBPN = 0;				// задание на канал
	ObjBPN[19]->tekBPN = 0;				// текуще значение
	ObjBPN[19]->prBPN = 0;				// признак положительного ответа
	ObjBPN[19]->vBPN = 0;				// признак выхода на режим

	// Описание свойств канала 21
	ObjBPN[20]->nR1 = 78;				// номер включения режима
	ObjBPN[20]->nR0 = 79;				// номер выключения режима
	ObjBPN[20]->normaOnNmb = 98;		// код нормы выхода на режим канала
	ObjBPN[20]->normaOffNmb = 99;		// код нормы отключения канала
	ObjBPN[20]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[20]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[20]->diagnNVR.nBt = 31;
	ObjBPN[20]->diagnNVR.bMask = 0x10;	// нет выхода на режим канала
	ObjBPN[20]->diagnBOR.nBt = 31;
	ObjBPN[20]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования канала
	ObjBPN[20]->diagnNR.nBt = 31;
	ObjBPN[20]->diagnNR.bMask = 0x40;	// нет регулирования канала
	ObjBPN[20]->diagnNAU.nBt = 31;
	ObjBPN[20]->diagnNAU.bMask = 0x80;	// нет ответа на задание уставки
	ObjBPN[20]->diagnSUM.nBt = 31;
	ObjBPN[20]->diagnSUM.bMask = 0xF0;	// суммарные диагностики режима
	ObjBPN[20]->parBPN = 0;				// параметр канала
	ObjBPN[20]->zadBPN = 0;				// задание на канал
	ObjBPN[20]->tekBPN = 0;				// текуще значение
	ObjBPN[20]->prBPN = 0;				// признак положительного ответа
	ObjBPN[20]->vBPN = 0;				// признак выхода на режим

	// Описание свойств канала 22 (только чтение текущих)
	ObjBPN[21]->parBPN = 0;				// параметр канала
	ObjBPN[21]->zadBPN = 0;				// задание на канал
	ObjBPN[21]->tekBPN = 0;				// текуще значение
	ObjBPN[21]->prBPN = 0;				// признак положительного ответа
	ObjBPN[21]->vBPN = 0;				// признак выхода на режим

    // Описание свойств канала 23
	ObjBPN[22]->nR1 = 80;				// номер включения режима
	ObjBPN[22]->nR0 = 81;				// номер выключения режима
	ObjBPN[22]->normaOnNmb = 104;		// код нормы выхода на режим канала
	ObjBPN[22]->normaOffNmb = 105;		// код нормы отключения канала
	ObjBPN[22]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[22]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[22]->diagnNVR.nBt = 23;
	ObjBPN[22]->diagnNVR.bMask = 0x01;	// нет выхода на режим канала
	ObjBPN[22]->diagnBOR.nBt = 23;
	ObjBPN[22]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования канала
	ObjBPN[22]->diagnNR.nBt = 23;
	ObjBPN[22]->diagnNR.bMask = 0x04;	// нет регулирования канала
	ObjBPN[22]->diagnNAU.nBt = 23;
	ObjBPN[22]->diagnNAU.bMask = 0x08;	// нет ответа на задание уставки
	ObjBPN[22]->diagnSUM.nBt = 23;
	ObjBPN[22]->diagnSUM.bMask = 0x0F;	// суммарные диагностики режима
	ObjBPN[22]->parBPN = 0;				// параметр канала
	ObjBPN[22]->zadBPN = 0;				// задание на канал
	ObjBPN[22]->tekBPN = 0;				// текуще значение
	ObjBPN[22]->prBPN = 0;				// признак положительного ответа
	ObjBPN[22]->vBPN = 0;				// признак выхода на режим

	// Описание свойств канала 24
	ObjBPN[23]->nR1 = 82;				// номер включения режима
	ObjBPN[23]->nR0 = 83;				// номер выключения режима
	ObjBPN[23]->normaOnNmb = 106;		// код нормы выхода на режим канала
	ObjBPN[23]->normaOffNmb = 107;		// код нормы отключения канала
	ObjBPN[23]->ctVR = 0;				// счетчик выхода на режим
	ObjBPN[23]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjBPN[23]->diagnNVR.nBt = 23;
	ObjBPN[23]->diagnNVR.bMask = 0x10;	// нет выхода на режим канала
	ObjBPN[23]->diagnBOR.nBt = 23;
	ObjBPN[23]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования канала
	ObjBPN[23]->diagnNR.nBt = 23;
	ObjBPN[23]->diagnNR.bMask = 0x40;	// нет регулирования канала
	ObjBPN[23]->diagnNAU.nBt = 23;
	ObjBPN[23]->diagnNAU.bMask = 0x80;	// нет ответа на задание уставки
	ObjBPN[23]->diagnSUM.nBt = 23;
	ObjBPN[23]->diagnSUM.bMask = 0xF0;	// суммарные диагностики режима
	ObjBPN[23]->parBPN = 0;				// параметр канала
	ObjBPN[23]->zadBPN = 0;				// задание на канал
	ObjBPN[23]->tekBPN = 0;				// текуще значение
	ObjBPN[23]->prBPN = 0;				// признак положительного ответа
	ObjBPN[23]->vBPN = 0;				// признак выхода на режим
}
