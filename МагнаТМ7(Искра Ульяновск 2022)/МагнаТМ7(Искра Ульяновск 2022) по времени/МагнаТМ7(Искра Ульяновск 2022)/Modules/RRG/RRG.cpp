//---------------------------------------------------------------------------
//--Файл описания шаблонных функций--//
//---------------------------------------------------------------------------
#include "RRG.h"
//---------------------------------------------------------------------------
//--Функция включения РРГ--//
//---------------------------------------------------------------------------
void RRGOn(SRRG *object, bool prRRG)
{
	switch (shr[object->nR])	// анализируем шаг режима
	{
		case 0: ;break;			// 0 - нет режима
		case 1:
		{
			A_OUT(object->aoutBt,object->parRRG+8192);// выдать уставку на РРГ
			SetOut(!prRRG,object->outKl0.nBt,object->outKl0.bMask);	// открыть Кл-РРГ по 0
			SetOut(prRRG,object->outKl1.nBt,object->outKl1.bMask); 	// открыть Кл-РРГ по 1
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
	SetOut(0,object->outKl0.nBt,object->outKl0.bMask);// закрыть Кл-РРГ по 0
	SetOut(0,object->outKl1.nBt,object->outKl1.bMask);// закрыть Кл-РРГ по 1
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
	ObjRRG[0]->normaOnNmb = 53;			// код нормы выхода на режим РРГ
	ObjRRG[0]->normaOffNmb = 18;		// код нормы отключения РРГ
	ObjRRG[0]->ctVR = 0;				// счетчик выхода на режим
	ObjRRG[0]->tkVR = 10;				// и его контр. время
	ObjRRG[0]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjRRG[0]->tkVRK = 3;				// и его контр. время
	ObjRRG[0]->diagnNVR.nBt = 7;
	ObjRRG[0]->diagnNVR.bMask = 0x01;	// нет выхода на режим РРГ
	ObjRRG[0]->diagnBOR.nBt = 7;
	ObjRRG[0]->diagnBOR.bMask = 0x02;	// большая ошибка регулирования РРГ
	ObjRRG[0]->diagnNR.nBt = 7;
	ObjRRG[0]->diagnNR.bMask = 0x04;	// нет регулирования РРГ
	ObjRRG[0]->diagnSUM.nBt = 7;
	ObjRRG[0]->diagnSUM.bMask = 0x07;	// суммарные диагностики режима 
	ObjRRG[0]->outKl0.nBt = 1;			// выходной дискретный сигнал клапана
	ObjRRG[0]->outKl0.bMask = 0x20;
	ObjRRG[0]->outKl1.nBt = 1;			// выходной дискретный сигнал клапана
	ObjRRG[0]->outKl1.bMask = 0x80;
	ObjRRG[0]->aikBt = 4;				// контрольный аналоговый вход
	ObjRRG[0]->parRRG = 0;				// параметр РРГ
	ObjRRG[0]->aoutBt = 4;				// аналоговый выход задания
	ObjRRG[0]->vRRG = 0;				// признак выхода на режим
	ObjRRG[0]->maxFl = 0.9;				// максимальный расход РРГ
	ObjRRG[0]->porog1 = 0.1;			// порог появления диагностики "БОР" 10%
	ObjRRG[0]->porog2 = 0.5;			// порог появления диагностики "НР" 50%
	
	// Описание свойств РРГ2
	ObjRRG[1]->nR = 21;					// номер соответствующего режима
	ObjRRG[1]->normaOnNmb = 54;			// код нормы выхода на режим РРГ
	ObjRRG[1]->normaOffNmb = 19;		// код нормы отключения РРГ
	ObjRRG[1]->ctVR = 0;				// счетчик выхода на режим
	ObjRRG[1]->tkVR = 10;				// и его контр. время
	ObjRRG[1]->ctVRK = 0;				// контрольный счетчик выхода на режим
	ObjRRG[1]->tkVRK = 3;				// и его контр. время
	ObjRRG[1]->diagnNVR.nBt = 7;
	ObjRRG[1]->diagnNVR.bMask = 0x10;	// нет выхода на режим РРГ
	ObjRRG[1]->diagnBOR.nBt = 7;
	ObjRRG[1]->diagnBOR.bMask = 0x20;	// большая ошибка регулирования РРГ
	ObjRRG[1]->diagnNR.nBt = 7;
	ObjRRG[1]->diagnNR.bMask = 0x40;	// нет регулирования РРГ
	ObjRRG[1]->diagnSUM.nBt = 7;
	ObjRRG[1]->diagnSUM.bMask = 0x70;	// суммарные диагностики режима 
	ObjRRG[1]->outKl0.nBt = 1;			// выходной дискретный сигнал клапана
	ObjRRG[1]->outKl0.bMask = 0x40;
	ObjRRG[1]->outKl1.nBt = 1;			// выходной дискретный сигнал клапана
	ObjRRG[1]->outKl1.bMask = 0x40;
	ObjRRG[1]->aikBt = 5;				// контрольный аналоговый вход
	ObjRRG[1]->parRRG = 0;				// параметр РРГ
	ObjRRG[1]->aoutBt = 5;				// аналоговый выход задания
	ObjRRG[1]->vRRG = 0;				// признак выхода на режим
	ObjRRG[1]->maxFl = 1.8;				// максимальный расход РРГ
	ObjRRG[1]->porog1 = 0.1;			// порог появления диагностики "БОР" 10%
	ObjRRG[1]->porog2 = 0.5;			// порог появления диагностики "НР" 50%
}