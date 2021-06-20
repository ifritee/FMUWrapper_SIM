#pragma once

#include "FMU10ObjectAbstract.h"

namespace fmuw
{
  class FMU20CoSimObject : public FMU10ObjectAbstract
  {
  public:
    /** @brief Конструктор */
    explicit FMU20CoSimObject(const std::string & path);
    /** @brief Деструктор */
    virtual ~FMU20CoSimObject();
    /** @brief Распарсивание данных модели */
    virtual void parse(std::string & fileName) override;
    /** @brief Загрузка методов библиотеки */
    virtual void loadLibrary() override;
    /** @brief initialize Инициализация модели */
    virtual void initialize(double endTime, double stepSize) override;
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

  private:
    double _StepSize_d = 0.1; ///< @brief Размер одного шага
  };

} // namespace fmuw
