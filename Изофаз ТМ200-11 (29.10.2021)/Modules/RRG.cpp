//---------------------------------------------------------------------------
//--Файл описания шаблонных функций--//
//---------------------------------------------------------------------------
#include "RRG.h"
//---------------------------------------------------------------------------
//--Функция включения РРГ--//
//---------------------------------------------------------------------------
void RRGOn(SRRG *object)
{
	switch (shr[object->nR])	// анализируем шаг режима
	{
		case 0: ;break;			// 0 - нет режима
		case 1:
		{
			A_OUT(object->aoutBt,object->parRRG+8192);// выдать уставку на РРГ
			SetOut(1,object->outKl.nBt,object->outKl.bMask);// открыть Кл-РРГ
			object->vRRG = 0;	// сброс признака выхода РРГ на режим
			diagn[object->diagnSUM.nBt]&=(~object->diagnSUM.bMask);// сброс диагностик режима
			object->ctVR = 0;	// сброс счетчика выхода на режим
			object->ctVRK = 0;	// сброс контрольного счетчика выхода на режим
			shr[object->nR]++;	// переход на след. шаг
		}; break;
		case 2:
		{
			// нет нового задания на РРГ
			if (aout[object->aoutBt]==(object->parRRG+8192))
			{
				// если РРГ в диапазоне
				if (abs((aik[object->aikBt])-object->parRRG)<=(float(object->parRRG)*object->porog1))
				{
					// если РРГ находится на режиме контрольное время
					if ( object->ctVRK > object->tkVRK )
					{
						diagn[object->diagnNVR.nBt]&=(~object->diagnNVR.bMask);// сброс диагностики
						object->vRRG = 1;	// выставить признак выхода на режиме
						norma = object->normaOnNmb;	// норма: "РРГ вышло на режим"
						shr[object->nR]++;			// переход на след. шаг
					}
				}
				// РРГ не в диапазоне
				else
				{
					object->ctVRK = 0;	// сброс контрольного счетчика выхода на режим
					if (object->ctVR > object->tkVR)	// время вышло
						diagn[object->diagnNVR.nBt] |= object->diagnNVR.bMask; // отказ: "Нет выхода РРГ на режим"
				}
			}
			// есть новое задания на РРГ
			else A_OUT(object->aoutBt,object->parRRG+8192); // выдать уставку на РРГ
		}; break;
		case 3:
		{
			// нет нового задания на РРГ
			if (aout[object->aoutBt]==(object->parRRG+8192))
			{
				// есть б.ош.регул. РРГ
				if (abs((aik[object->aikBt])-object->parRRG)>(float(object->parRRG)*object->porog1))
				{
					// есть регулирование РРГ
					if (abs((aik[object->aikBt])-object->parRRG)<(float(object->parRRG)*object->porog2))
					{
						diagn[object->diagnBOR.nBt]|=object->diagnBOR.bMask;	// отказ: "б.ош.регул.РРГ"
						diagn[object->diagnNR.nBt]&=(~object->diagnNR.bMask);	// сброс: "нет регул.РРГ"
					}
					else
					{
						diagn[object->diagnNR.nBt]|=object->diagnNR.bMask;		// отказ: "нет регул.РРГ"
						diagn[object->diagnBOR.nBt]&=(~object->diagnBOR.bMask);	// сброс: "б.ош.регул.РРГ"
					}
				}
				else
				{
					diagn[object->diagnNR.nBt]&=(~object->diagnNR.bMask);		// сброс: "нет регул.РРГ"
					diagn[object->diagnBOR.nBt]&=(~object->diagnBOR.bMask);		// сброс: "б.ош.регул.РРГ"
				}
			}
			// есть новое задания на РРГ
			else A_OUT(object->aoutBt,(object->parRRG+8192));	// выдать уставку
		}; break;
		default:						// невозможный шаг
		{
			sh[object->nR]  = 0;
			shr[object->nR] = 0;
		}; break;
	}
}
//---------------------------------------------------------------------------
//--Функция отключения РРГ--//
//---------------------------------------------------------------------------
void RRGOff(SRRG *object)
{
	sh[object->nR]  = 0;
	shr[object->nR] = 0;
	object->vRRG = 0;						// сброс признака выхода РРГ на режим
	diagn[object->diagnSUM.nBt]&=(~object->diagnSUM.bMask);// сброс диагностик режима
	SetOut(0,object->outKl.nBt,object->outKl.bMask);// закрыть Кл-РРГ
	A_OUT(object->aoutBt,8192);				// сбросить уставку на РРГ
	norma = object->normaOffNmb;			// норма: "Сброс РРГ завершен"
}
//---------------------------------------------------------------------------
//--Функция инкремента счетчиков--//
//---------------------------------------------------------------------------
void TimeRRG()
{
	for(int i=0;i<qRRG;i++)
	{
		ObjRRG[i]->ctVR++;	// счетчик выхода на режим
		ObjRRG[i]->ctVRK++;	// контрольный счетчик выхода на режим
	}
}
//---------------------------------------------------------------------------
//--Функция инициализации объектов типа РРГ--//
//---------------------------------------------------------------------------
void InitObjectsRRG()
{
    for (unsigned int i = 0 ; i < qRRG ; i++ )
        ObjRRG[i] = new SRRG();
// Описание свойств РРГ1
	ObjRRG[0]->nR = 20;					// номер соответствующего режима
	ObjRRG[0]->normaOnNmb = 12;			// код нормы выхода на режим РРГ
	ObjRRG[0]->normaOffNmb = 13;		// код нормы отключения РРГ
	ObjRRG[0]->ctVR = 0;				// счетчик выхода на режим
	ObjRRG[0]->tkVR = 10;				// и его контр. время
	ObjRRG[0]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjRRG[0]->tkVRK = 3;				// и его контр. время
	ObjRRG[0]->diagnNVR.nBt = 0;
	ObjRRG[0]->diagnNVR.bMask = 0x01;	// нет выхода на режим РРГ
	ObjRRG[0]->diagnBOR.nBt = 0;
	ObjRRG[0]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования РРГ
	ObjRRG[0]->diagnNR.nBt = 0;
	ObjRRG[0]->diagnNR.bMask = 0x04;	// нет регулирования РРГ
	ObjRRG[0]->diagnSUM.nBt = 0;
	ObjRRG[0]->diagnSUM.bMask = 0x07;	// суммарные диагностики режима 
	ObjRRG[0]->outKl.nBt = 2;			// выходной дискретный сигнал клапана
	ObjRRG[0]->outKl.bMask = 0x01;
	ObjRRG[0]->aikBt = 5;				// контрольный аналоговый вход
	ObjRRG[0]->parRRG = 0;				// параметр РРГ
	ObjRRG[0]->aoutBt = 0;				// аналоговый выход задания
	ObjRRG[0]->vRRG = 0;				// признак выхода на режим
	ObjRRG[0]->maxFl = 1.8;				// максимальный расход РРГ
	ObjRRG[0]->porog1 = 0.1;			// порог появления диагностики "БОР" 10%
	ObjRRG[0]->porog2 = 0.5;			// порог появления диагностики "НР" 50%
	
	// Описание свойств РРГ2
	ObjRRG[1]->nR = 21;					// номер соответствующего режима
	ObjRRG[1]->normaOnNmb = 14;			// код нормы выхода на режим РРГ
	ObjRRG[1]->normaOffNmb = 15;		// код нормы отключения РРГ
	ObjRRG[1]->ctVR = 0;				// счетчик выхода на режим
	ObjRRG[1]->tkVR = 10;				// и его контр. время
	ObjRRG[1]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjRRG[1]->tkVRK = 3;				// и его контр. время
	ObjRRG[1]->diagnNVR.nBt = 0;
	ObjRRG[1]->diagnNVR.bMask = 0x10;	// нет выхода на режим РРГ
	ObjRRG[1]->diagnBOR.nBt = 0;
	ObjRRG[1]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования РРГ
	ObjRRG[1]->diagnNR.nBt = 0;
	ObjRRG[1]->diagnNR.bMask = 0x40;	// нет регулирования РРГ
	ObjRRG[1]->diagnSUM.nBt = 0;
	ObjRRG[1]->diagnSUM.bMask = 0x70;	// суммарные диагностики режима 
	ObjRRG[1]->outKl.nBt = 2;			// выходной дискретный сигнал клапана
	ObjRRG[1]->outKl.bMask = 0x02;
	ObjRRG[1]->aikBt = 6;				// контрольный аналоговый вход
	ObjRRG[1]->parRRG = 0;				// параметр РРГ
	ObjRRG[1]->aoutBt = 1;				// аналоговый выход задания
	ObjRRG[1]->vRRG = 0;				// признак выхода на режим
	ObjRRG[1]->maxFl = 3.6;				// максимальный расход РРГ
	ObjRRG[1]->porog1 = 0.1;			// порог появления диагностики "БОР" 10%
	ObjRRG[1]->porog2 = 0.5;			// порог появления диагностики "НР" 50%
	
	// Описание свойств РРГ3
	ObjRRG[2]->nR = 22;					// номер соответствующего режима
	ObjRRG[2]->normaOnNmb = 16;			// код нормы выхода на режим РРГ
	ObjRRG[2]->normaOffNmb = 17;		// код нормы отключения РРГ
	ObjRRG[2]->ctVR = 0;				// счетчик выхода на режим
	ObjRRG[2]->tkVR = 10;				// и его контр. время
	ObjRRG[2]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjRRG[2]->tkVRK = 3;				// и его контр. время
	ObjRRG[2]->diagnNVR.nBt = 1;
	ObjRRG[2]->diagnNVR.bMask = 0x01;	// нет выхода на режим РРГ
	ObjRRG[2]->diagnBOR.nBt = 1;
	ObjRRG[2]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования РРГ
	ObjRRG[2]->diagnNR.nBt = 1;
	ObjRRG[2]->diagnNR.bMask = 0x04;	// нет регулирования РРГ
	ObjRRG[2]->diagnSUM.nBt = 1;
	ObjRRG[2]->diagnSUM.bMask = 0x07;	// суммарные диагностики режима 
	ObjRRG[2]->outKl.nBt = 2;			// выходной дискретный сигнал клапана
	ObjRRG[2]->outKl.bMask = 0x04;
	ObjRRG[2]->aikBt = 7;				// контрольный аналоговый вход
	ObjRRG[2]->parRRG = 0;				// параметр РРГ
	ObjRRG[2]->aoutBt = 2;				// аналоговый выход задания
	ObjRRG[2]->vRRG = 0;				// признак выхода на режим
	ObjRRG[2]->maxFl = 0.9;				// максимальный расход РРГ
	ObjRRG[2]->porog1 = 0.1;			// порог появления диагностики "БОР" 10%
	ObjRRG[2]->porog2 = 0.5;			// порог появления диагностики "НР" 50%
	
	// Описание свойств РРГ4
	ObjRRG[3]->nR = 23;					// номер соответствующего режима
	ObjRRG[3]->normaOnNmb = 18;			// код нормы выхода на режим РРГ
	ObjRRG[3]->normaOffNmb = 19;		// код нормы отключения РРГ
	ObjRRG[3]->ctVR = 0;				// счетчик выхода на режим
	ObjRRG[3]->tkVR = 10;				// и его контр. время
	ObjRRG[3]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjRRG[3]->tkVRK = 3;				// и его контр. время
	ObjRRG[3]->diagnNVR.nBt = 1;
	ObjRRG[3]->diagnNVR.bMask = 0x10;	// нет выхода на режим РРГ
	ObjRRG[3]->diagnBOR.nBt = 1;
	ObjRRG[3]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования РРГ
	ObjRRG[3]->diagnNR.nBt = 1;
	ObjRRG[3]->diagnNR.bMask = 0x40;	// нет регулирования РРГ
	ObjRRG[3]->diagnSUM.nBt = 1;
	ObjRRG[3]->diagnSUM.bMask = 0x70;	// суммарные диагностики режима 
	ObjRRG[3]->outKl.nBt = 2;			// выходной дискретный сигнал клапана
	ObjRRG[3]->outKl.bMask = 0x88;
	ObjRRG[3]->aikBt = 8;				// контрольный аналоговый вход
	ObjRRG[3]->parRRG = 0;				// параметр РРГ
	ObjRRG[3]->aoutBt = 3;				// аналоговый выход задания
	ObjRRG[3]->vRRG = 0;				// признак выхода на режим
	ObjRRG[3]->maxFl = 0.9;				// максимальный расход РРГ
	ObjRRG[3]->porog1 = 0.1;			// порог появления диагностики "БОР" 10%
	ObjRRG[3]->porog2 = 0.5;			// порог появления диагностики "НР" 50%	
	
	// Описание свойств РРГ5
	ObjRRG[4]->nR = 24;					// номер соответствующего режима
	ObjRRG[4]->normaOnNmb = 53;			// код нормы выхода на режим РРГ
	ObjRRG[4]->normaOffNmb = 54;		// код нормы отключения РРГ
	ObjRRG[4]->ctVR = 0;				// счетчик выхода на режим
	ObjRRG[4]->tkVR = 10;				// и его контр. время
	ObjRRG[4]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjRRG[4]->tkVRK = 3;				// и его контр. время
	ObjRRG[4]->diagnNVR.nBt = 2;
	ObjRRG[4]->diagnNVR.bMask = 0x01;	// нет выхода на режим РРГ
	ObjRRG[4]->diagnBOR.nBt = 2;
	ObjRRG[4]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования РРГ
	ObjRRG[4]->diagnNR.nBt = 2;
	ObjRRG[4]->diagnNR.bMask = 0x04;	// нет регулирования РРГ
	ObjRRG[4]->diagnSUM.nBt = 2;
	ObjRRG[4]->diagnSUM.bMask = 0x07;	// суммарные диагностики режима 
	ObjRRG[4]->outKl.nBt = 2;			// выходной дискретный сигнал клапана
	ObjRRG[4]->outKl.bMask = 0x110;
	ObjRRG[4]->aikBt = 9;				// контрольный аналоговый вход
	ObjRRG[4]->parRRG = 0;				// параметр РРГ
	ObjRRG[4]->aoutBt = 4;				// аналоговый выход задания
	ObjRRG[4]->vRRG = 0;				// признак выхода на режим
	ObjRRG[4]->maxFl = 0.9;				// максимальный расход РРГ
	ObjRRG[4]->porog1 = 0.1;			// порог появления диагностики "БОР" 10%
	ObjRRG[4]->porog2 = 0.5;			// порог появления диагностики "НР" 50%	
	
	// Описание свойств РРГ6
	ObjRRG[5]->nR = 25;					// номер соответствующего режима
	ObjRRG[5]->normaOnNmb = 55;			// код нормы выхода на режим РРГ
	ObjRRG[5]->normaOffNmb = 56;		// код нормы отключения РРГ
	ObjRRG[5]->ctVR = 0;				// счетчик выхода на режим
	ObjRRG[5]->tkVR = 10;				// и его контр. время
	ObjRRG[5]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjRRG[5]->tkVRK = 3;				// и его контр. время
	ObjRRG[5]->diagnNVR.nBt = 18;
	ObjRRG[5]->diagnNVR.bMask = 0x01;	// нет выхода на режим РРГ
	ObjRRG[5]->diagnBOR.nBt = 18;
	ObjRRG[5]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования РРГ
	ObjRRG[5]->diagnNR.nBt = 18;
	ObjRRG[5]->diagnNR.bMask = 0x04;	// нет регулирования РРГ
	ObjRRG[5]->diagnSUM.nBt = 18;
	ObjRRG[5]->diagnSUM.bMask = 0x07;	// суммарные диагностики режима 
	ObjRRG[5]->outKl.nBt = 2;			// выходной дискретный сигнал клапана
	ObjRRG[5]->outKl.bMask = 0x20;
	ObjRRG[5]->aikBt = 10;				// контрольный аналоговый вход
	ObjRRG[5]->parRRG = 0;				// параметр РРГ
	ObjRRG[5]->aoutBt = 5;				// аналоговый выход задания
	ObjRRG[5]->vRRG = 0;				// признак выхода на режим
	ObjRRG[5]->maxFl = 0.9;				// максимальный расход РРГ
	ObjRRG[5]->porog1 = 0.1;			// порог появления диагностики "БОР" 10%
	ObjRRG[5]->porog2 = 0.5;			// порог появления диагностики "НР" 50%	
	
	// Описание свойств РРГ7
	ObjRRG[6]->nR = 26;					// номер соответствующего режима
	ObjRRG[6]->normaOnNmb = 57;			// код нормы выхода на режим РРГ
	ObjRRG[6]->normaOffNmb = 58;		// код нормы отключения РРГ
	ObjRRG[6]->ctVR = 0;				// счетчик выхода на режим
	ObjRRG[6]->tkVR = 10;				// и его контр. время
	ObjRRG[6]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjRRG[6]->tkVRK = 3;				// и его контр. время
	ObjRRG[6]->diagnNVR.nBt = 18;
	ObjRRG[6]->diagnNVR.bMask = 0x10;	// нет выхода на режим РРГ
	ObjRRG[6]->diagnBOR.nBt = 18;
	ObjRRG[6]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования РРГ
	ObjRRG[6]->diagnNR.nBt = 18;
	ObjRRG[6]->diagnNR.bMask = 0x40;	// нет регулирования РРГ
	ObjRRG[6]->diagnSUM.nBt = 18;
	ObjRRG[6]->diagnSUM.bMask = 0x70;	// суммарные диагностики режима 
	ObjRRG[6]->outKl.nBt = 2;			// выходной дискретный сигнал клапана
	ObjRRG[6]->outKl.bMask = 0x40;
	ObjRRG[6]->aikBt = 11;				// контрольный аналоговый вход
	ObjRRG[6]->parRRG = 0;				// параметр РРГ
	ObjRRG[6]->aoutBt = 6;				// аналоговый выход задания
	ObjRRG[6]->vRRG = 0;				// признак выхода на режим
	ObjRRG[6]->maxFl = 0.9;				// максимальный расход РРГ
	ObjRRG[6]->porog1 = 0.1;			// порог появления диагностики "БОР" 10%
	ObjRRG[6]->porog2 = 0.5;			// порог появления диагностики "НР" 50%	
}