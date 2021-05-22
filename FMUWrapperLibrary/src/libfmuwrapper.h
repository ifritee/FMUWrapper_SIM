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
  FMUWRAPPER_EXPORT int unzipFMU(const char * file, const char * dst);
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

}

#endif // LIBFMUWRAPPER_H
