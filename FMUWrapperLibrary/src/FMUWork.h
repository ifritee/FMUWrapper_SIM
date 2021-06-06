#ifndef FMUWORK_H
#define FMUWORK_H

#include <string>

/**
 * @namespace fmuw
 * @brief Пространство имен для элементов работы с различными моделями FMU
 * @author Никишин Е.В.
 * @date 17.05.2021 */
namespace fmuw
{

  class FMUAbstract;

  /**
   * @class FMUWork
   * @brief Класс работы с моделью FMU
   * @author Никишин Е.В.
   * @date 17.05.2021
   *
   * Позволяет распарсить файл xml модели FMU, определить к какой версии относится модель и
   * подгрузить уже нужный модуль. */
  class FMUWork
  {
  public:
    /**
     * @brief FMUWork Конструктор с параметрами
     * @param path Путь к распакованной FMU модели */
    explicit FMUWork(const std::string & path);
    /** @brief Деструктор */
    virtual ~FMUWork();
    /**
     * @brief DescriptionRead_b парсинг основного файла xml (версия FMI и тип)
     * @return true - успешно, false - проблема */
    bool DescriptionRead_b();
    /** @brief Возвращает путь к распакованной FMU модели */
    std::string fmuPath() const;
    /** @brief Возвращает последнюю ошибку */
    std::string lastError() const;
    /** @brief Инициализация модели */
    void modelInit(double entTime, double stepSize);
    /** @brief Возвращает количество входных данных */
    int inputsQty();
    /** @brief Возвращает количество выходных данных */
    int outputsQty();
    /** @brief Шаг расчета модели */
    void modelStep();

    std::string outputVar(int index, int & type);
    std::string inputVar(int index, int & type);
    /** @brief Возвращает значение типа DOUBLE по имени */
    double doubleValue(const std::string &);
    /** @brief Возвращает значение типа DOUBLE по имени */
    bool boolValue(const std::string &);
    /** @brief Возвращает значение типа DOUBLE по имени */
    int intValue(const std::string &);
    /** @brief Возвращает значение типа DOUBLE по имени */
    std::string strValue(const std::string &);

  private:
    const std::string m_fmuPath;  ///< @brief Путь к распакованной FMU модели
    std::string m_lastError;  ///< @brief Последняя ошибка
    std::string m_modelName;  ///< @brief Имя модели
    std::string m_fmuVersion; ///< @brief Версия используемого FMI
    FMUAbstract * m_fmuObject = nullptr;  ///< @brief Указатель на текущий объект FMU
  };

} // namespace fmuw



#endif // FMUWORK_H
