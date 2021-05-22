#ifndef FMUABSTRACT_H
#define FMUABSTRACT_H

namespace fmuw
{
  /**
   * @class FMUAbstract
   * @brief Интерфейс для разных типов FMI
   * @author Никишин Е. В.
   * @date 22.05.2021 */
  class FMUAbstract
  {
  public:
    /** @brief Конструктор */
    explicit FMUAbstract();
    /** @brief Деструктор */
    virtual ~FMUAbstract();

  };
} // namespace fmuw


#endif // FMUABSTRACT_H
