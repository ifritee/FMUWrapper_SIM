#ifndef FMU10OBJECT_H
#define FMU10OBJECT_H

#include "FMUAbstract.h"

namespace fmuw
{
  /**
   * @class FMU10Object
   * @brief Объект модели FMU с интерфейсом FMI 1.0
   * @author Никишин Е. В.
   * @date 22.05.2021 */
  class FMU10Object : public FMUAbstract
  {
  public:
    /** @brief Конструктор */
    explicit FMU10Object();
    /** @brief Деструктор */
    virtual ~FMU10Object();
  };

} // namespace fmuw

#endif // FMU10OBJECT_H
