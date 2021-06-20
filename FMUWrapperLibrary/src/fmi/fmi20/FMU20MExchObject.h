#pragma once

#include "FMU10ObjectAbstract.h"
#include "XmlParserCApi.h"

namespace fmuw
{

  namespace fmi20 { struct FMU; class CME20Data; }

  class FMU20MExchObject : public FMU10ObjectAbstract
  {
  public:
    /** @brief Конструктор */
    explicit FMU20MExchObject(const std::string & path);
    /** @brief Деструктор */
    virtual ~FMU20MExchObject();
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
    fmi20::FMU * _ModelUnit_po;  ///< @brief Указатель на модели
    fmi20::CME20Data * _ModelData_po = nullptr; ///< @brief Данные текущей модели

    /**
     * @brief Вывод данных о загружаемой модели
     * @param md Дескриптор модели */
    void printModelDescription(ModelDescription* md);
    /**
     * @brief getFunctionAddress Ищет адрес функции по имени
     * @param functionName Имя функции
     * @return Указатель на функцию */
    void* getFunctionAddress(const char* functionName);
  };

}
