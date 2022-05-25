//---------------------------------------------------------------------------
//--Файл описания шаблонных функций--//
//---------------------------------------------------------------------------
#include "Klapan.h"
//---------------------------------------------------------------------------
//--Функция инкрементации счетчика объекта--//
//---------------------------------------------------------------------------
void SKlapan::Time()
{
    ctObject++;
}
//---------------------------------------------------------------------------
//--Функция управления объектами через дискретные входы-выходы--//
//---------------------------------------------------------------------------
void Klapan(bool action, SKlapan *object)
{
    switch ( object -> type )
    {
        case 11: DoAction11(action, object); break;
        case 12: DoAction12(action, object); break;
        case 21: DoAction21(action, object); break;
        case 22: DoAction22(action, object); break;
        // неверный тип объекта
        default: diagn[0] |= 0x01; break;
    }
}
//---------------------------------------------------------------------------
//--Функция управления объектом с 1 дискретным выходом и 1 входом--//
//---------------------------------------------------------------------------
void DoAction11(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // если нужный результат достигнут - закончить работу
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // выставить/снять управляющий дискретный сигнал
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                // выставить норму
                norma = object->normaNmb[(int)(!action)];
                // завершить подпрограмму
                sh_ = 0;
            }
            else
            {
                // выставить/снять управляющий дискретный сигнал
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                // обнулить время реакции объекта
                object->ctObject = 0;
                // перейти на следующий шаг
                sh_++;
            }
        }; break;
        case 2:
        {
            // изучение реакции воздействия на объект
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // снять диагностику
                diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
                // обнулить время реакции объекта
                object->ctObject = 0;
                // перейти на следующий шаг
                sh_++;
            }
            // вышло время реакции объекта
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
                // выставить диагностику
                diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
        }; break;
        case 3:
        {
            // задержка после выполнения активных действий над объектом
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // выставить норму
                norma = object->normaNmb[(int)(!action)];
                // завершить подпрограмму
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--Функция управления объектом с 1 дискретным выходом и 2 входами--//
//---------------------------------------------------------------------------
void DoAction12(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // если нужный результат достигнут - закончить работу
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // выставить/снять управляющий дискретный сигнал
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                // выставить норму
                norma = object->normaNmb[(int)(!action)];
                // завершить подпрограмму
                sh_ = 0;
            }
            else
            {
                // выставить/снять управляющий дискретный сигнал
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                // обнулить время реакции объекта
                object->ctObject = 0;
                // перейти на следующий шаг
                sh_++;
            }
        }; break;
        case 2:
        {
            // снять диагностики
            diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
            diagn[object->diagnObject[2].x] &= (~object->diagnObject[2].y);
            diagn[object->diagnObject[3].x] &= (~object->diagnObject[3].y);
            // изучение реакции воздействия на объект
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // обнулить время реакции объекта
                object->ctObject = 0;
                // перейти на следующий шаг
                sh_++;
            }
            // вышло время реакции объекта
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
            {
                // положение объекта неопределено
                if (
                        ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 0 ) &&
                        ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 0 )
                    )
                // выставить диагностику
                diagn[object->diagnObject[2].x] |= object->diagnObject[2].y;
                // положение объекта неоднозначно
                else if (
                            ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 1 ) &&
                            ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 1 )
                        )
                    // выставить диагностику
                    diagn[object->diagnObject[3].x] |= object->diagnObject[3].y;
                else
                    // выставить диагностику об отсутствии реакции
                    diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
            }
        }; break;
        case 3:
        {
            // задержка после выполнения активных действий над объектом
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // выставить норму
                norma = object->normaNmb[(int)(!action)];
                // завершить подпрограмму
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--Функция управления объектом с 2 дискретными выходами и 1 входом--//
//---------------------------------------------------------------------------
void DoAction21(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // если нужный результат достигнут - закончить работу
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // выставить/снять управляющий дискретный сигнал
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                SetOut(!action, object->zinOut[1].x, object->zinOut[1].y);
                // выставить норму
                norma = object->normaNmb[(int)(!action)];
                // завершить подпрограмму
                sh_ = 0;
            }
            else
            {
                // выставить/снять управляющий дискретный сигнал
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                SetOut(!action, object->zinOut[1].x, object->zinOut[1].y);
                // обнулить время реакции объекта
                object->ctObject = 0;
                // перейти на следующий шаг
                sh_++;
            }
        }; break;
        case 2:
        {
            // изучение реакции воздействия на объект
            if ( (bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action )
            {
                // снять диагностику
                diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
                // обнулить время реакции объекта
                object->ctObject = 0;
                // перейти на следующий шаг
                sh_++;
            }
            // вышло время реакции объекта
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
                // выставить диагностику
                diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
        }; break;
        case 3:
        {
            // задержка после выполнения активных действий над объектом
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // выставить норму
                norma = object->normaNmb[(int)(!action)];
                // завершить подпрограмму
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--Функция управления объектом с 2 дискретными выходами и 2 входами--//
//---------------------------------------------------------------------------
void DoAction22(bool action, SKlapan *object)
{
    switch ( sh_ )
    {
        case 1:
        {
            // если нужный результат достигнут - закончить работу
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // выставить/снять управляющий дискретный сигнал
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                SetOut(!action, object->zinOut[1].x, object->zinOut[1].y);
                // выставить норму
                norma = object->normaNmb[(int)(!action)];
                // завершить подпрограмму
                sh_ = 0;
            }
            else
            {
                // выставить/снять управляющий дискретный сигнал
                SetOut(action, object->zinOut[0].x, object->zinOut[0].y);
                SetOut(!action, object->zinOut[1].x, object->zinOut[1].y);
                // обнулить время реакции объекта
                object->ctObject = 0;
                // перейти на следующий шаг
                sh_++;
            }
        }; break;
        case 2:
        {
            // снять диагностики
            diagn[object->diagnObject[(int)(!action)].x] &= (~object->diagnObject[(int)(!action)].y);
            diagn[object->diagnObject[2].x] &= (~object->diagnObject[2].y);
            diagn[object->diagnObject[3].x] &= (~object->diagnObject[3].y);
            // изучение реакции воздействия на объект
            if  (
                    ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == action ) &&
                    ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) != action )
                )
            {
                // обнулить время реакции объекта
                object->ctObject = 0;
                // перейти на следующий шаг
                sh_++;
            }
            // вышло время реакции объекта
            else if ( object->ctObject > object->tkAction[(int)(!action)] )
            {
                // положение объекта неопределено
                if (
                        ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 0 ) &&
                        ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 0 )
                    )
                // выставить диагностику
                diagn[object->diagnObject[2].x] |= object->diagnObject[2].y;
                // положение объекта неоднозначно
                else if (
                            ((bool)( zin[object->zinIn[0].x] & object->zinIn[0].y ) == 1 ) &&
                            ((bool)( zin[object->zinIn[1].x] & object->zinIn[1].y ) == 1 )
                        )
                    // выставить диагностику
                    diagn[object->diagnObject[3].x] |= object->diagnObject[3].y;
                else
                    // выставить диагностику об отсутствии реакции
                    diagn[object->diagnObject[(int)(!action)].x] |= object->diagnObject[(int)(!action)].y;
            }
        }; break;
        case 3:
        {
            // задержка после выполнения активных действий над объектом
            if ( object->ctObject >= object->tkAction[(int)(!action)+2] )
            {
                // выставить норму
                norma = object->normaNmb[(int)(!action)];
                // завершить подпрограмму
                sh_ = 0;
            }
        }; break;
        default: sh_ = 0; break;
    }
}
//---------------------------------------------------------------------------
//--Функция инкремента счетчиков--//
//---------------------------------------------------------------------------
void TimeKlapan()
{
    KlKn.Time();
    KlKam.Time();
    Fvn.Time();
	
}
//---------------------------------------------------------------------------
//--Функция инициализации объектов с дискретным управлением и дискретной обратной связью--//
//---------------------------------------------------------------------------
void InitObjectsKl()
{
	// описание клапана Кл-КН
	KlKn.type = 12;					// Кл-КН
	// нормы объекта
	KlKn.normaNmb[0] = 36;				// код нормы включения
	KlKn.normaNmb[1] = 37;				// код нормы отключения
	// контрольные времена объекта
	KlKn.tkAction[0] = 2;				// к.время открытия
	KlKn.tkAction[1] = 2;				// к.время закрытия
	KlKn.tkAction[2] = 2;				// к.время задержки после открытия
	KlKn.tkAction[3] = 2;				// к.время задержки после закрытия
	// диагностики объекта
	KlKn.diagnObject[0].x = 12;		// номер байта диагностики "не открылся"
	KlKn.diagnObject[0].y = 0x1;		// маска бита диагностики "не открылся"
	KlKn.diagnObject[1].x = 12;		// номер байта диагностики "не закрылся"
	KlKn.diagnObject[1].y = 0x2;		// маска бита диагностики "не закрылся"
	KlKn.diagnObject[2].x = 12;		// номер байта диагностики "не определено"
	KlKn.diagnObject[2].y = 0x4;		// маска бита диагностики "не определено"
	KlKn.diagnObject[3].x = 12;		// номер байта диагностики "неоднозначно"
	KlKn.diagnObject[3].y = 0x8;		// маска бита диагностики "неоднозначно"
	// дискретные управляющие выходы
	KlKn.zinOut[0].x = 0;				// номер байта упр. выходного сигнала на "открытие"
	KlKn.zinOut[0].y = 0x20;			// маска бита упр. выходного сигнала  на "открытие"
	KlKn.zinOut[1].x = 0;				// номер байта упр. выходного сигнала на "закрытие"
	KlKn.zinOut[1].y = 0x20;			// маска бита упр. выходного сигнала на "закрытие"
	// дискретные входы обратной связи
	KlKn.zinIn[0].x = 0;				// номер байта ответного входного сигнала об "открытии"
	KlKn.zinIn[0].y = 0x4000;			// маска бита ответного выходного сигнала об "открытии"
	KlKn.zinIn[1].x = 0;				// номер байта ответного входного сигнала о "закрытии"
	KlKn.zinIn[1].y = 0x8000;			// маска бита ответного выходного сигнала о "закрытии"

	// описание клапана Кл-КАМ
	KlKam.type = 12;					// ФК-КАМ
	// нормы объекта
	KlKam.normaNmb[0] = 34;				// код нормы включения
	KlKam.normaNmb[1] = 35;				// код нормы отключения
	// контрольные времена объекта
	KlKam.tkAction[0] = 2;				// к.время открытия
	KlKam.tkAction[1] = 2;				// к.время закрытия
	KlKam.tkAction[2] = 2;				// к.время задержки после открытия
	KlKam.tkAction[3] = 2;				// к.время задержки после закрытия
	// диагностики объекта
	KlKam.diagnObject[0].x = 13;		// номер байта диагностики "не открылся"
	KlKam.diagnObject[0].y = 0x01;		// маска бита диагностики "не открылся"
	KlKam.diagnObject[1].x = 13;		// номер байта диагностики "не закрылся"
	KlKam.diagnObject[1].y = 0x02;		// маска бита диагностики "не закрылся"
	KlKam.diagnObject[2].x = 13;		// номер байта диагностики "не определено"
	KlKam.diagnObject[2].y = 0x04;		// маска бита диагностики "не определено"
	KlKam.diagnObject[3].x = 13;		// номер байта диагностики "неоднозначно"
	KlKam.diagnObject[3].y = 0x08;		// маска бита диагностики "неоднозначно"
	// дискретные управляющие выходы
	KlKam.zinOut[0].x = 0;				// номер байта упр. выходного сигнала на "открытие"
	KlKam.zinOut[0].y = 0x10;			// маска бита упр. выходного сигнала  на "открытие"
	KlKam.zinOut[1].x = 0;				// номер байта упр. выходного сигнала на "закрытие"
	KlKam.zinOut[1].y = 0x10;			// маска бита упр. выходного сигнала на "закрытие"
	// дискретные входы обратной связи
	KlKam.zinIn[0].x = 0;				// номер байта ответного входного сигнала об "открытии"
	KlKam.zinIn[0].y = 0x1000;			// маска бита ответного выходного сигнала об "открытии"
	KlKam.zinIn[1].x = 0;				// номер байта ответного входного сигнала о "закрытии"
	KlKam.zinIn[1].y = 0x2000;			// маска бита ответного выходного сигнала о "закрытии"


	// описание форнасоса
	Fvn.type = 11;						// ФВН
	// нормы объекта
	Fvn.normaNmb[0] = 42;				// код нормы включения
	Fvn.normaNmb[1] = 43;				// код нормы отключения
	// контрольные времена объекта
	Fvn.tkAction[0] = 5;				// к.время открытия
	Fvn.tkAction[1] = 5;				// к.время закрытия
	Fvn.tkAction[2] = 15;				// к.время задержки после открытия
	Fvn.tkAction[3] = 2;				// к.время задержки после закрытия
	// диагностики объекта
	Fvn.diagnObject[0].x = 14;			// номер байта диагностики "не открылся"
	Fvn.diagnObject[0].y = 0x40;		// маска бита диагностики "не открылся"
	Fvn.diagnObject[1].x = 14;			// номер байта диагностики "не закрылся"
	Fvn.diagnObject[1].y = 0x80;		// маска бита диагностики "не закрылся"
	Fvn.diagnObject[2].x = 14;			// номер байта диагностики "не определено"
	Fvn.diagnObject[2].y = 0x80;		// маска бита диагностики "не определено"
	Fvn.diagnObject[3].x = 14;			// номер байта диагностики "неоднозначно"
	Fvn.diagnObject[3].y = 0x80;		// маска бита диагностики "неоднозначно"
	// дискретные управляющие выходы
	Fvn.zinOut[0].x = 0;				// номер байта упр. выходного сигнала на "открытие"
	Fvn.zinOut[0].y = 0x01;				// маска бита упр. выходного сигнала  на "открытие"
	Fvn.zinOut[1].x = 0;				// номер байта упр. выходного сигнала на "закрытие"
	Fvn.zinOut[1].y = 0x01;				// маска бита упр. выходного сигнала на "закрытие"
	// дискретные входы обратной связи
	Fvn.zinIn[0].x = 1;					// номер байта ответного входного сигнала об "открытии"
	Fvn.zinIn[0].y = 0x01;				// маска бита ответного выходного сигнала об "открытии"
	Fvn.zinIn[1].x = 1;					// номер байта ответного входного сигнала о "закрытии"
	Fvn.zinIn[1].y = 0x01;				// маска бита ответного выходного сигнала о "закрытии"
}
