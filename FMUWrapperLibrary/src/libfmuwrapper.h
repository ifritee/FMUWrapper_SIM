#ifndef LIBFMUWRAPPER_H
#define LIBFMUWRAPPER_H

#include <stddef.h>

#include "libfmuwrapper_global.h"

/**
 * @enum STATUS_CODE
 * @brief Код статуса отработки функции */
enum STATUS_CODE {
  CODE_WARNING = -2  ///< @brief завершено успешно, с предупреждением
, CODE_FAILED = -1 ///< @brief завершено с ошибкой
, CODE_OK = 0 ///< @brief завершено успешно
};

extern "C" {
/**
   * @brief unzipFMU Разархивация файла xxx.fmu в папку
   * @param file Имя файла с путём
   * @param dst Папка назначения (может быть tmp)
   * @return -1 - неудача, -2 - успех, но с предупреждением, >=0 - успех + номер текущей модели */
  FMUWRAPPER_EXPORT int createFMU(const char * path);
  /**
   * @brief removePath Удаляет папку с временными файлами FMU
   * @param number номер модели
   * @return 0 - успех, -1 - неудача, 1 - успех, но с предупреждением */
  FMUWRAPPER_EXPORT int removeTmp(int number);
  /**
   * @brief lastError Записываает последнюю ошибку
   * @param error указатель на массив чаров для записи текста ошибки
   * @param length размер массива */
  FMUWRAPPER_EXPORT void lastError(char * error, const size_t size);
  /**
   * @brief parsing Запуск парсинга файла дескриптора FMU модели
   * @param number номер модели
   * @return 0 - успех, -1 - неудача, 1 - успех, но с предупреждением */
  FMUWRAPPER_EXPORT int parsing(int number);

  //----- Работа с моделью --------------------------------------------
  /**
   * @brief initialize Инициализация модели FMU
   * @param number номер модели
   * @param entTime Время окончания моделирования
   * @param stepSize Размер шага
   * @return 0 - успех, -1 - неудача, 1 - успех, но с предупреждением */
  FMUWRAPPER_EXPORT int initialize(int number, double entTime, double stepSize);
  /**
   * @brief outputsQty Возвращает количество выходных переменных
   * @param number номер модели
   * @return Количество выходных переменных или -1, если ошибка */
  FMUWRAPPER_EXPORT int outputsQty(int number);
  /**
   * @brief outputVar Устанавливает имя и тип переменной по индексу
   * @param number номер модели
   * @param index индект переменной
   * @param nameBuffer Буфер для записи имени
   * @param length размер буфера
   * @param type номер типа ( в нумерации fmu sdk)
   * @return 0 - успех, -1 - неудача, 1 - успех, но с предупреждением */
  FMUWRAPPER_EXPORT int outputVar(int number, int index, char * nameBuffer, int length, int & type);
  /**
   * @brief step Шаг расчета модели
   * @param number номер модели
   * @return 0 - успех, -1 - неудача, 1 - успех, но с предупреждением */
  FMUWRAPPER_EXPORT int step(int number);
  /**
   * @brief getDouble Возвращает значение типа DOUBLE по имени
   * @param number номер модели
   * @param name Имя переменной
   * @return значение переменной */
  FMUWRAPPER_EXPORT double getDouble(int number, const char * name);
  /**
   * @brief getBool Возвращает значение типа Bool по имени
   * @param number номер модели
   * @param name Имя переменной
   * @return значение переменной */
  FMUWRAPPER_EXPORT bool getBool(int number, const char * name);
  /**
   * @brief getBool Возвращает значение типа Int по имени
   * @param number номер модели
   * @param name Имя переменной
   * @return значение переменной */
  FMUWRAPPER_EXPORT int getInt(int number, const char * name);
  /**
   * @brief getBool Возвращает значение типа String по имени
   * @param number номер модели
   * @param name Имя переменной
   * @param buffer Буфер для записи строки
   * @param length Длина буфера
   * @return Размер строки */
  FMUWRAPPER_EXPORT int getString(int number, const char * name, char * buffer, int length);

}

#endif // LIBFMUWRAPPER_H
