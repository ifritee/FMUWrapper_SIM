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
    explicit FMU20Object();
    /** @brief Деструктор */
    virtual ~FMU20Object();
  };

} // namespace fmuw

#endif // FMU20OBJECT_H
