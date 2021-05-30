#ifndef FMU10COSIMOBJECT_H
#define FMU10COSIMOBJECT_H

#include "FMU10ObjectAbstract.h"

namespace fmuw
{

  namespace fmi10 {
    struct FMU;
    struct ModelDescription;
  }

  class FMU10CoSimObject : public FMU10ObjectAbstract
  {
  public:
    /** @brief Конструктор */
    explicit FMU10CoSimObject(const std::string & path);
    /** @brief Деструктор */
    virtual ~FMU10CoSimObject();
    /** @brief Распарсивание данных модели */
    virtual void parse(std::string & fileName) override;
    /** @brief Загрузка методов библиотеки */
    virtual void loadLibrary() override;
    /** @brief initialize Инициализация модели */
    virtual void initialize(double endTime) override;

  private:
    fmi10::FMU * _ModelUnit_po;  ///< @brief Указатель на модель

    /**
     * @brief Вывод данных о загружаемой модели
     * @param md Дескриптор модели */
    void printModelDescription(fmi10::ModelDescription* md);

    void* getFunctionAddress(const char* functionName);
  };

} // namespace fmuw

#endif // FMU10COSIMOBJECT_H
