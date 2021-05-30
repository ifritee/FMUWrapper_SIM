/* -------------------------------------------------------------------------
 * fmi_me.h
 * Function types for all function of the "FMI for Model Exchange 1.0"
 * and a struct with the corresponding function pointers.
 * Copyright QTronic GmbH. All rights reserved.
 * -------------------------------------------------------------------------*/

#pragma once

#ifdef _MSC_VER
#include <windows.h>
#define WINDOWS 1
#if (_MSC_VER >= 1900)
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif /* _MSC_VER >= 1900 */
#else /* _MSC_VER */
#include <errno.h>
#define WINDOWS 0
#define TRUE 1
#define FALSE 0
#define min(a,b) (a>b ? b : a)
#define HANDLE void *
  /* See http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html */
#include <dlfcn.h>
#endif /* _MSC_VER */

#include "fmiModelFunctions.h"
#include "xml_parser10.h"

namespace fmuw::fmi10
{
  typedef const char* (*fGetModelTypesPlatform)();
  typedef const char* (*fGetVersion)();
  typedef fmiComponent (*fInstantiateModel)(fmiString instanceName, fmiString GUID,
                                            fmiCallbackFunctions functions, fmiBoolean loggingOn);
  typedef void      (*fFreeModelInstance)  (fmiComponent c);
  /**
   * Этот метод устанавливает ведение журнала отладки на FMU. Если loggingOn истинно,
   * ведение журнала включено. категории - это массив, указывающий, какие категории
   * журналов должны регистрироваться. Записи этого массива могут быть любым из
   * идентификаторов, найденных в массиве Fmi2Import.logCategories. */
  typedef fmiStatus (*fSetDebugLogging)    (fmiComponent c, fmiBoolean loggingOn);
  /**
   * Устанавливает новый момент времени и повторно инициализируей кэширование переменных,
   * зависящих от времени (переменных, которые зависят исключительно от констант или
   * параметров, которые не нужно заново вычислять в дальнейшем, но ранее вычисленные
   * значения можно использовать повторно). */
  typedef fmiStatus (*fSetTime)            (fmiComponent c, fmiReal time);
  /**
   * Устанавливает новый (непрерывный) вектор состояния и повторно инициализирует
   * кеширование переменных, которые зависят от состояния. Аргумент nx представляет
   * собой длину вектора x и предоставляется для проверки (переменные которые зависят
   * исключительно от констант, параметров, времени и входных данных, не нужно заново
   * вычислять в продолжение, но ранее вычисленные значения можно использовать повторно).
   * Обратите внимание: fmiEventUpdate может также изменить непрерывные состояния.
   * Примечание: fmiStatus = fmi Сбросить можно. */
  typedef fmiStatus (*fSetContinuousStates)(fmiComponent c, const fmiReal x[], size_t nx);
  /**
   * Эта функция должна вызываться средой после каждого завершенного шага интегратора.
   * Если функция возвращается с callEventUpdate = fmiTrue, тогда среда должна вызвать
   * fmiEventUpdate (..), в противном случае никаких действий не требуется.
   * Когда этап интегратора завершен и состояния впоследствии изменены интегратором
   * (например, исправление методом BDF), тогда fmiSetContinuousStates (..) должен быть
   * вызван с обновленные состояния перед вызовом fmiCompletedIntegratorStep (..).
   * Эта функция может использоваться, например, для следующих целей:
   * 1. Задержки: Все переменные, которые используются в операторе задержки (..),
   * хранятся в соответствующем буфере, а функция возвращается с callEventUpdate = fmiFalse.
   * 2. Выбор динамического состояния: Проверяется, соответствуют ли динамически выбранные
   * состояния по-прежнему численно. Если да, функция возвращается с callEventUpdate = fmiFalse,
   *  иначе с fmiTrue. В последнем случае необходимо вызвать fmiEventUpdate (..),
   *  который динамически изменяет состояния. */
  typedef fmiStatus (*fCompletedIntegratorStep)(fmiComponent c, fmiBoolean* callEventUpdate);
  /**
   * Установите независимые параметры, входные данные, начальные значения и повторно
   * инициализируйте кеширование переменных, которые зависят от этих переменных.
   * Аргумент «vr» - это вектор дескрипторов значений «nvr», которые определяют переменные,
   * которые должны быть установлены. Аргумент «значение» - это вектор с фактическими
   * значениями этих переменных. Все строки, переданные в качестве аргументов функции
   * fmiSetString, должны быть скопированы внутри этой функции, потому что нет гарантии
   * времени жизни строк, когда эта функция вернется. Примечание: fmiStatus = fmiDiscard
   * можно использовать для fmiSetReal. Ограничения на использование функций
   * «fmiSetReal / Integer / Boolean / String» (см. также раздел 2.9):
   * 1. Эти функции могут быть вызваны на входах (ScalarVariable.Causality = «input») после
   * вызова fmiInstantiateModel и до меня FreeModel.
   * 2. Кроме того, эти функции можно вызывать для переменных, у которых есть
   * «ScalarVariable / <type> /. start »после вызова fmiInstantiateModel и перед
   * вызовом fmiInitialize. Если эти функции не вызываются для переменной с атрибутом
   * «start», тогда значение «start» этого переменная в C-функциях является этим «начальным»
   * значением (поэтому это начальное значение должно храниться как в xml-файл и в C-функциях).
   * 3. Если ссылка на значение появляется несколько раз в vr [], то будет установлено
   * последнее значение. [Сюда результат такой же, как при многократном вызове функции с
   * одной и той же ссылкой на значение.]
   * 4. Установка параметров с псевдонимом и входных переменных: последний вызов
   * fmiSetXXX () определит значение псевдонима переменной (ых). У перечисленных выше
   * функций есть небольшой недостаток, заключающийся в том, что значения всегда должны
   * копироваться, например, вызов «FmiSetContinuousStates» предоставит фактические
   * состояния в векторе, и эта функция должна скопировать значения во внутренней
   * структуре данных модели «c», чтобы последующие вызовы оценки могли использовать эти
   * значения. Если окажется, что это проблема эффективности, будущий выпуск FMI может
   * предоставить дополнительные функции для предоставления адреса области памяти, в
   * которой присутствуют значения переменных. */
  typedef fmiStatus (*fSetReal)   (fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal    value[]);
  typedef fmiStatus (*fSetInteger)(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[]);
  typedef fmiStatus (*fSetBoolean)(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[]);
  typedef fmiStatus (*fSetString) (fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString  value[]);
  /**
   * Инициализирует модель, т.е. вычисляет начальные значения для всех переменных. Перед вызовом этой функции,
   * fmiSetTime () должен быть вызван, и все переменные с атрибутом «ScalarVariable / <type> / start» или
   * параметр ScalarVariable.causality = «input» может быть установлен с помощью функций «fmiSetXXX» (
   * Атрибуты ScalarVariable определены в файле описания модели, см. Раздел 3). Установка других
   * переменные не разрешены (за исключением ScalarVariable.causality = «none»). Если
   * «capacityControlled = fmiTrue», то модель вызывается с численным интегрированием. Схема, в которой
   * размер шага контролируется с помощью «relativeTolerance» для оценки ошибки. В в таком случае все
   * численные алгоритмы, используемые внутри модели (например, для решения нелинейных алгебраических
   * уравнения) также должны работать с оценкой ошибки соответствующего меньшего относительного толерантность.
   * Функция возвращается после завершения инициализации (или при использовании в fmiEventUpdate, когда
   * найдено новое согласованное состояние), и интеграцию можно перезапустить. Функция возвращает с
   * eventInfo. Эта структура также используется как возвращаемое значение fmiEventUpdate. Переменные
   * конструкции имеют следующее значение:
   * Аргументы iterationConverged, stateValueReferencesChanged и stateValuesChanged имеют смысл только
   * при возврате из fmiEventUpdate. Когда возвращаясь из fmiInitialize, все три флага всегда fmiTrue.
   * Если stateValuesChanged = fmiTrue, когда iterationConverged = fmiTrue, то при по крайней мере один
   * элемент непрерывного вектора состояния изменил свое значение, например, поскольку в начальный
   * момент времени или из-за импульса. Новые значения состояний должны быть запрошены с помощью функции
   * fmiGetContinuousStates. Если stateValueReferencesChanged = fmiTrue, когда iterationConverged = fmiTrue,
   * затем значение состояний изменилось. Значения Ссылки новых состояний могут быть запрашивается с
   * помощью fmiGetStateValueReferences, и номинальные значения новых состояний могут быть запросил с
   * помощью fmiGetNominalContinuousStates. Если terminateSimulation = fmiTrue, моделирование должно быть
   * прекращено (успешно). это предполагается, что FMU напечатает соответствующее сообщение,
   * чтобы объяснить причину прекращение. Если upcomingTimeEvent = fmiTrue, то моделирование должно
   * интегрироваться максимум до time = nextEventTime и в этот момент должен вызвать fmiEventUpdate.
   * Если интеграция остановлена перед nextEventTime, например, из-за события состояния, определение
   * nextEventTime становится устаревший. [В настоящее время эту функцию можно вызвать только один раз
   * для одного экземпляра. Обратите внимание, даже если это может быть только вызывается один раз, событие
   * может быть инициировано, а затем возможна итерация события через fmiEventUpdate в начальное время.] */
  typedef fmiStatus (*fInitialize)(fmiComponent c, fmiBoolean toleranceControlled,
                                   fmiReal relativeTolerance, fmiEventInfo* eventInfo);
  /**
   * Вычислить производные состояния и индикаторы событий в текущий момент времени и для текущего
   * состояния. Производные возвращаются как вектор с элементами «nx». Срабатывает государственное
   * событие когда домен индикатора события изменяется с zj> 0 на zj ≤ 0 или наоборот (см. раздел
   * 2.1). FMU должен гарантировать, что при перезапуске события zj ≠ 0, например, сдвигая z j с небольшим
   * значение. Кроме того, z j следует масштабировать в FMU с его номинальным значением (см. Приложение B.2).
   * Индикаторы событий возвращаются в виде вектора с элементами «ni». Порядок элементов вектора производных
   * идентичен порядку состояния вектор (например, производные [2] - это производные от x [2]).
   * Индикаторы событий не обязательно связанных с переменными в файле описания модели.*/
  typedef fmiStatus (*fGetDerivatives)    (fmiComponent c, fmiReal derivatives[]    , size_t nx);
  typedef fmiStatus (*fGetEventIndicators)(fmiComponent c, fmiReal eventIndicators[], size_t ni);
  /**
   * Получите фактические значения переменных, предоставив дескрипторы переменных. Эти функции особенно
   * используется для получения фактических значений выходных переменных, если модель связана с другими
   * моделями. Более того, фактическое значение каждой переменной, определенной в файле описания модели,
   * может быть определяется в каждый момент времени. Строка, возвращаемая fmiGetString, должна быть
   * скопирована в целевая среда, потому что выделенная память для этой строки может быть освобождена
   * следующий вызов любой из функций интерфейса fmi, или это может быть внутренний строковый буфер,
   * который просто повторно используется. Примечание: fmiStatus = fmiDiscard можно использовать для
   * fmiGetReal (но не для fmiGetInteger, fmiGetBoolean, fmiGetString, потому что это дискретные переменные,
   * и их значения могут только изменить в момент события, когда fmiDiscard не имеет смысла). */
  typedef fmiStatus (*fGetReal)   (fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal    value[]);
  typedef fmiStatus (*fGetInteger)(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[]);
  typedef fmiStatus (*fGetBoolean)(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[]);
  typedef fmiStatus (*fGetString) (fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString  value[]);
  /**
   * Эта функция вызывается после того, как произошло событие времени, состояния или шага. Функция
   * возвращает eventInfo (подробнее см. функцию fmiInitialize). Если «intermediateResults =
   * fmiFalse », функция возвращается после того, как будет найдено новое согласованное состояние и
   * интеграция можно перезапустить. Если аргумент - fmiTrue, функция возвращает значение для каждой
   * итерации события. который выполняется внутри, чтобы позволить получать переменные результата после
   * каждой итерации с fmiGetXXX функционирует выше. Функция должна вызываться последовательно до тех пор,
   * пока «EventInfo-> iterationConverged = fmiTrue» и должен возвращать окончательный статус eventInfo->
   * stateValueReferencesChanged и eventInfo-> stateValuesChanged. */
  typedef fmiStatus (*fEventUpdate)               (fmiComponent c, fmiBoolean intermediateResults, fmiEventInfo* eventInfo);
  /**
   * Вернуть новый (непрерывный) вектор состояния x после завершения итерации события (включая
   * инициализация). Эта функция должна вызываться после инициализации, и если (непрерывный) вектор
   * состояния изменилось в момент события после вызова fmiEventUpdate (..) с
   * eventInfo-> iterationConverged = fmiTrue. */
  typedef fmiStatus (*fGetContinuousStates)       (fmiComponent c, fmiReal states[], size_t nx);
  /**
   * Возвращает номинальные значения непрерывных состояний. Эту функцию всегда следует вызывать после
   * fmiInitialize, а если eventInfo-> stateValueReferencesChanged = fmiTrue в fmiEventUpdate, с тех пор
   * связь непрерывных состояний с переменными изменилась. а, следовательно, и их номинальные значения.
   * Если FMU не имеет информации о номинальном значении непрерывного состояния i, должно быть возвращено
   * номинальное значение x_nominal [i] = 1.0. Обычно номинальные значения непрерывных состояний
   * используются для вычисления абсолютного допуск, требуемый интегратором, например:
   * absoluteTolerance[i] = 0.01*relativeTolerance*x_nominal[i]; */
  typedef fmiStatus (*fGetNominalContinuousStates)(fmiComponent c, fmiReal x_nominal[], size_t nx);
  /**
   * Возвращает ссылки на значения вектора состояния (например, используется для печати информационного
   * сообщения, которое переменная чаще всего ограничивает размер шага). В случае выбора динамического
   * состояния значение ссылки могут измениться после вызова fmiEventUpdate (..). В этом случае
   * fmiEventUpdate возвращает с eventInfo-> stateValueReferencesChanged = fmiTrue.
   * Если vrx [i] = fmiUndefinedValueReference (см. Раздел 2.2), модель скрывает значение состояния
   * и отсутствие ссылки на значение (fmiUndefinedValueReference) для этого состояния возвращается,
   * иначе vrx [i] должен быть действительной ссылкой на значение, объявленной в Элемент modelVariables
   * файла modelDescription.xml. */
  typedef fmiStatus (*fGetStateValueReferences)   (fmiComponent c, fmiValueReference vrx[], size_t nx);
  /**
   * Завершает оценку модели в конце моделирования или после желаемой остановки интеграция до
   * окончания моделирования. Освободите все ресурсы, которые были выделены с fmiInitialize
   * был вызван .. После вызова этой функции конечные значения всех переменных могут быть
   * запрошенным с помощью функций fmiGetXXX (..) выше. Нельзя вызывать эту функцию после
   * одна из функций, возвращенных с флагом состояния fmiError или fmiFatal. */
  typedef fmiStatus (*fTerminate)                 (fmiComponent c);

  /**
   * @struct FMU
   * @brief Набор данных модели */
  struct FMU {
    ModelDescription* modelDescription; ///< @brief Дескриптор модели (структура данных)
    HANDLE dllHandle; ///< @brief Дескриптор библиотеки
    fGetModelTypesPlatform getModelTypesPlatform; ///< @brief Тип платформы для которой собрана модель
    fGetVersion getVersion; ///< @brief Возвращает версию FMI
    fInstantiateModel instantiateModel; ///< @brief Создает экземпляр модели
    fFreeModelInstance freeModelInstance; ///< @brief Освобождает экземпляр модели
    fSetDebugLogging setDebugLogging; ///< @brief Устанавливает ведение журнала отладки на FMU
    fSetTime setTime; ///< @brief Устанавливает новый момент времени
    fSetContinuousStates setContinuousStates; ///< @brief Устанавливает новый (непрерывный) вектор состояния
    fCompletedIntegratorStep completedIntegratorStep; ///< @brief завершенного шага интегратора
    fSetReal setReal; ///< @brief Установка параметра с плавающей точкой
    fSetInteger setInteger; ///< @brief Установка параметра типа INT
    fSetBoolean setBoolean; ///< @brief Установка параметра типа BOOL
    fSetString setString; ///< @brief Установка параметра типа STRING
    fInitialize initialize; ///< @brief Инициализирует модель
    fGetDerivatives getDerivatives; ///< @brief Вычисляет производные состояния
    fGetEventIndicators getEventIndicators; ///< @brief Вычисляет индикаторы событий
    fGetReal getReal;
    fGetInteger getInteger;
    fGetBoolean getBoolean;
    fGetString getString;
    fEventUpdate eventUpdate; ///< @brief Обновление состояния
    fGetContinuousStates getContinuousStates; ///< @brief Вернуть новый (непрерывный) вектор состояния
    fGetNominalContinuousStates getNominalContinuousStates; ///< @brief Возвращает номинальные значения непрерывных состояний
    fGetStateValueReferences getStateValueReferences; ///< @brief Возвращает ссылки на значения вектора состояния
    fTerminate terminate; ///< @brief Завершает оценку модели в конце моделирования
  };

} // namespace fmuw::fmi10

