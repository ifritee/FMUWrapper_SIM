#ifndef FMU10OBJECT_H
#define FMU10OBJECT_H

#include "FMUAbstract.h"

namespace fmuw
{

  class FMU10ObjectAbstract;

  /**
   * @class FMU10Object
   * @brief Объект модели FMU с интерфейсом FMI 1.0
   * @author Никишин Е. В.
   * @date 22.05.2021 */
  class FMU10Object : public FMUAbstract
  {
  public:
    /** @brief Конструктор */
    explicit FMU10Object(const std::string & path);
    /** @brief Деструктор */
    virtual ~FMU10Object();
    /**
     * @brief Считывание данных из xml-файла
     * @param tree дерево элеентов из xml-файла */
    virtual void parse(boost::property_tree::ptree & tree) override;
    /** @brief initialize Инициализация модели */
    virtual void initialize(double entTime) override;

  private:
    FMU10ObjectAbstract * _Model_po = nullptr; ///< @brief Модель FMU
  };

} // namespace fmuw

#endif // FMU10OBJECT_H
