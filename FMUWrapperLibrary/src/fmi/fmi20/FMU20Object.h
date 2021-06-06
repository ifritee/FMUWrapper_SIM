#ifndef FMU20OBJECT_H
#define FMU20OBJECT_H

#include "FMUAbstract.h"

namespace fmuw
{
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
  };

} // namespace fmuw

#endif // FMU20OBJECT_H
