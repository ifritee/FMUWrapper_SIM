#ifndef FMU10OBJECTABSTRACT_H
#define FMU10OBJECTABSTRACT_H

#include <string>

namespace fmuw
{
  class FMU10ObjectAbstract {

  public:
    /** @brief Конструктор */
    explicit FMU10ObjectAbstract(const std::string & path)
      : _PathToFMU_str(path) {}
    /** @brief Деструктор */
    virtual ~FMU10ObjectAbstract() = default;
    /** @brief Распарсивание данных модели */
    virtual void parse(std::string & fileName) = 0;
    /** @brief Загрузка библиотеки модели */
    virtual void loadLibrary() = 0;
    /** @brief initialize Инициализация модели */
    virtual void initialize(double endTime) = 0;

  protected:
    std::string _FilenameDLL_str; ///< @brief Полное имя файла с библиотекой модели
    std::string _PathToFMU_str; ///< @brief Путь к папке с FMU
  };
} // namespace fmuw

#endif // FMU10OBJECTABSTRACT_H
