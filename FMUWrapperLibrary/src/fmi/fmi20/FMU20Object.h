#ifndef FMU20OBJECT_H
#define FMU20OBJECT_H

#include "FMUAbstract.h"

namespace fmuw
{
  class FMU10ObjectAbstract;

  /**
   * @class FMU20Object
   * @brief Объект модели FMU с интерфейсом FMI 2.0
   * @author Никишин Е. В.
   * @date 22.05.2021 */
  class FMU20Object : public FMUAbstract
  {
  public:
    /** @brief Конструктор */
    explicit FMU20Object(const std::string & path);
    /** @brief Деструктор */
    virtual ~FMU20Object();
    /**
     * @brief Считывание данных из xml-файла
     * @param tree дерево элеентов из xml-файла */
    virtual void parse(boost::property_tree::ptree & tree) override;
    /** @brief initialize Инициализация модели */
    virtual void initialize(double entTime, double stepSize) override;
    /** @brief Шаг расчета модели */
    virtual void step() override;
    /** @brief Возвращает все входные переменные с типом */
    virtual const std::map<std::string, int> & inputVariables() override;
    /** @brief Возвращает все выходные переменные с типом */
    virtual const std::map<std::string, int> & outputVariables() override;
    /** @brief Возвращает значение типа DOUBLE по имени */
    virtual double doubleValue(const std::string &) override;
    /** @brief Возвращает значение типа BOOL по имени */
    virtual bool boolValue(const std::string &) override;
    /** @brief Возвращает значение типа INT по имени */
    virtual int intValue(const std::string &) override;
    /** @brief Возвращает значение типа STRING по имени */
    virtual std::string strValue(const std::string &) override;
    /** @brief Устанавливает значение типа DOUBLE по имени */
    virtual void setDoubleValue(const std::string &, double) override;
    /** @brief Устанавливает значение типа BOOL по имени */
    virtual void setBoolValue(const std::string &, bool) override;
    /** @brief Устанавливает значение типа INT по имени */
    virtual void setIntValue(const std::string &, int) override;
    /** @brief Устанавливает значение типа STRING по имени */
    virtual void setStringValue(const std::string &, const std::string &) override;

  private:
    FMU10ObjectAbstract * _Model_po = nullptr; ///< @brief Модель FMU
  };

} // namespace fmuw

#endif // FMU20OBJECT_H
