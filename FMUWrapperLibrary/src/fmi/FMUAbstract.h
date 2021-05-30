#ifndef FMUABSTRACT_H
#define FMUABSTRACT_H

#include <boost/property_tree/xml_parser.hpp>

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
    explicit FMUAbstract(const std::string & path);
    /** @brief Деструктор */
    virtual ~FMUAbstract();
    /**
     * @brief Считывание данных из xml-файла
     * @param tree дерево элеентов из xml-файла */
    virtual void parse(boost::property_tree::ptree & tree) = 0;
    /** @brief initialize Инициализация модели */
    virtual void initialize(double entTime) = 0;

  protected:
    std::string _FMUPath_str; ///< @brief Путь к папке с файлами FMU
    std::string _XMLFilename_str; ///< @brief Имя файла XML, который парсится

  };
} // namespace fmuw


#endif // FMUABSTRACT_H
