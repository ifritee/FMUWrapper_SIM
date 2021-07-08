 //************************************************************************** //
 // Данный исходный код является заголовочным для библиотеки FMUWrapperLibrary//
 // Программист:        Никишин Е.В.                                          //
 //************************************************************************** //

unit FMUWrapperLibrary;

interface
uses
     Classes
   , Winapi.Windows
   ;
const
  FMULIB_DLL = 'FMUWrapperLibrary.dll';

type
  UINT8  =  Byte;
  PUINT8 = PByte;
  UINT16 =  Word;
  PUINT16 = PWord;
  UINT32 =  Cardinal;
  PUINT32 = PCardinal;
  INT8   =  ShortInt;
  INT16  =  SmallInt;
  INT32  =  LongInt;
  PINT8   = PShortInt;
  PINT16  = PSmallInt;
  PINT32 =  PLongint;

   // @brief unzipFMU Разархивация файла xxx.fmu в папку
   // @param file Имя файла с путём
   // @param dst Папка назначения (может быть tmp)
   // @return -1 - неудача, -2 - успех, но с предупреждением, >=0 - успех + номер текущей модели 
  function createFMU(path : PAnsiChar) : Integer; cdecl; external FMULIB_DLL;

   // @brief removePath Удаляет папку с временными файлами FMU
   // @param number номер модели
   // @return 0 - успех, -1 - неудача, 1 - успех, но с предупреждением
  function freeFMU(number : Integer) : Integer; cdecl; external FMULIB_DLL;

   // @brief lastError Записываает последнюю ошибку
   // @param error указатель на массив чаров для записи текста ошибки
   // @param length размер массива
  procedure lastError(error : PAnsiChar; size : UINT32); external FMULIB_DLL;

   // @brief parsing Запуск парсинга файла дескриптора FMU модели
   // @param number номер модели
   // @return 0 - успех, -1 - неудача, 1 - успех, но с предупреждением 
  function parsing(number : Integer) : Integer;  external FMULIB_DLL;

   // @brief initialize Инициализация модели FMU
   // @param number номер модели
   // @param entTime Время окончания моделирования
   // @param stepSize Размер шага
   // @return 0 - успех, -1 - неудача, 1 - успех, но с предупреждением 
  function initialize(number : Integer; entTime : Real; stepSize : Real) : Integer;  external FMULIB_DLL;

   // @brief outputsQty Возвращает количество выходных переменных
   // @param number номер модели
   // @return Количество выходных переменных или -1, если ошибка
  function outputsQty(number : Integer) : Integer; external FMULIB_DLL;

   // @brief outputVar Устанавливает имя и тип переменной по индексу
   // @param number номер модели
   // @param index индект переменной
   // @param nameBuffer Буфер для записи имени
   // @param length размер буфера
   // @param type номер типа ( в нумерации fmu sdk)
   // @return 0 - успех, -1 - неудача, 1 - успех, но с предупреждением */
  function outputVar(number : Integer; index : Integer; nameBuffer : PAnsiChar; length : Integer; var ttype : Integer) : Integer; external FMULIB_DLL;

   // @brief step Шаг расчета модели
   // @param number номер модели
   // @return 0 - успех, -1 - неудача, 1 - успех, но с предупреждением 
  function step(number : Integer) : Integer; external FMULIB_DLL; 

   // @brief getDouble Возвращает значение типа DOUBLE по имени
   // @param number номер модели
   // @param name Имя переменной
   // @return значение переменной 
  function getDouble(number : Integer; name : PAnsiChar) : Real; external FMULIB_DLL;

   // @brief getBool Возвращает значение типа Bool по имени
   // @param number номер модели
   // @param name Имя переменной
   // @return значение переменной 
  function getBool(number : Integer; name : PAnsiChar) : Boolean; external FMULIB_DLL;

   // @brief getBool Возвращает значение типа Int по имени
   // @param number номер модели
   // @param name Имя переменной
   // @return значение переменной
  function getInt(number : Integer; name : PAnsiChar) : Integer; external FMULIB_DLL;

   // @brief getBool Возвращает значение типа String по имени
   // @param number номер модели
   // @param name Имя переменной
   // @param buffer Буфер для записи строки
   // @param length Длина буфера
   // @return Размер строки
  function getString(number : Integer; name : PAnsiChar; buffer : PAnsiChar; length : Integer) : Integer; external FMULIB_DLL;
  
implementation

end.
