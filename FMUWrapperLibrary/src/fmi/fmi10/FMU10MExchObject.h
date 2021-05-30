#ifndef FMU10MEXCHOBJECT_H
#define FMU10MEXCHOBJECT_H

#include "FMU10ObjectAbstract.h"

namespace fmuw
{

  namespace fmi10 {
    struct FMU;
    struct ModelDescription;
  }

  namespace name {

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

  private:
    fmi10::FMU * _ModelUnit_po;  ///< @brief Указатель на модель

    /**
     * @brief Вывод данных о загружаемой модели
     * @param md Дескриптор модели */
    void printModelDescription(fmi10::ModelDescription* md);

    void* getFunctionAddress(const char* functionName);

  };

} // namespace fmuw

#endif // FMU10MEXCHOBJECT_H
