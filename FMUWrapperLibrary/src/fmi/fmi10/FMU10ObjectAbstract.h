#ifndef FMU10OBJECTABSTRACT_H
#define FMU10OBJECTABSTRACT_H

#include <string>
#include <map>

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
    virtual void initialize(double endTime, double stepSize) = 0;
    /** @brief Шаг расчета модели */
    virtual void step() = 0;
    /** @brief Возвращает все входные переменные с типом */
    virtual const std::map<std::string, int> & inputVariables() = 0;
    /** @brief Возвращает все выходные переменные с типом */
    virtual const std::map<std::string, int> & outputVariables() = 0;
    /** @brief Возвращает значение типа DOUBLE по имени */
    virtual double doubleValue(const std::string &) = 0;
    /** @brief Возвращает значение типа BOOL по имени */
    virtual bool boolValue(const std::string &) = 0;
    /** @brief Возвращает значение типа INT по имени */
    virtual int intValue(const std::string &) = 0;
    /** @brief Возвращает значение типа STRING по имени */
    virtual std::string strValue(const std::string &) = 0;

  protected:
    std::string _FilenameDLL_str; ///< @brief Полное имя файла с библиотекой модели
    std::string _PathToFMU_str; ///< @brief Путь к папке с FMU
  };
} // namespace fmuw

#endif // FMU10OBJECTABSTRACT_H
