#ifndef FMU10MEXCHOBJECT_H
#define FMU10MEXCHOBJECT_H

#include <map>
//#include <any>

#include "FMU10ObjectAbstract.h"

namespace fmuw
{
  namespace fmi10 {
    class CMEData;
    struct FMU;
    struct ModelDescription;
  }

  class FMU10MExchObject : public FMU10ObjectAbstract
  {

  public:
    /** @brief Конструктор */
    explicit FMU10MExchObject(const std::string & path);
    /** @brief Деструктор */
    virtual ~FMU10MExchObject();
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
    /** @brief Задает значение */
    virtual void setDoubleValue(const std::string &, double) override;
    /** @brief Задает значение */
    virtual void setBoolValue(const std::string &, bool) override;
    /** @brief Задает значение */
    virtual void setIntValue(const std::string &, int) override;
    /** @brief Задает значение */
    virtual void setStringValue(const std::string &, const std::string &) override;

  private:
    fmi10::FMU * _ModelUnit_po;  ///< @brief Указатель на модель
    fmi10::CMEData * _ModelData_po = nullptr; ///< @brief Данные текущей модели
    double _StepSize_d = 0.1; ///< @brief Размер одного шага

    /**
     * @brief Вывод данных о загружаемой модели
     * @param md Дескриптор модели */
    void printModelDescription(fmi10::ModelDescription* md);

    void* getFunctionAddress(const char* functionName);

  };

} // namespace fmuw

#endif // FMU10MEXCHOBJECT_H
