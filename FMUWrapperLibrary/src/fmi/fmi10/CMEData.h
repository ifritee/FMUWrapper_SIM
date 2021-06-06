#ifndef CMEDATA_H
#define CMEDATA_H

#include <string>
#include <map>

#include "fmi_me.h"

namespace fmuw
{
  namespace fmi10
  {
    class FMU;

    /**
     * @class CMEData
     * @brief Обработка модели, получение значений и отработка модели по шагам */
    class CMEData
    {
    public:
      /** @brief Конструктор */
      explicit CMEData(const char * guid, FMU * fmu);
      /** @brief Деструктор */
      virtual ~CMEData();
      /** @brief Освобождение ресурсов */
      void freeData();
      /** @brief создание компонентов */
      void createComponent();
      /**
       * @brief initialize Инициализация модели
       * @param tStart Время запуска
       * @param tEnd Время остановки
       * @param stepSize Размер шага */
      void Initialize_v(fmiReal tStart, fmiReal tEnd);
      /**
       * @brief setVarData Задание значение входной переменной
       * @param name Имя переменной
       * @param value значение переменной
       * @param qty количество переменных */
      void setVarData(const std::string & name, const char *value, size_t qty = 1);
      /**
       * @brief setVarData Задание значение входной переменной
       * @param name Имя переменной
       * @param value значение переменной
       * @param qty количество переменных */
      void setVarData(const std::string & name, const int *value, size_t qty = 1);
      /**
       * @brief setVarData Задание значение входной переменной
       * @param name Имя переменной
       * @param value значение переменной
       * @param qty количество переменных */
      void setVarData(const std::string & name, const double *value, size_t qty = 1);
      /**
       * @brief setVarData Задание значение входной переменной
       * @param name Имя переменной
       * @param value значение переменной
       * @param qty количество переменных */
      void setVarData(const std::string & name, const char *value[], size_t qty = 1);

      /** @brief Один шаг расчета */
      void step( double stepSize);

    private:
      fmi10::FMU * _Model_po; ///< @brief Структура данных модели
      std::string _ModelID_str; ///< @brief global unique id of the fmu
      fmiComponent _Component_o; ///< @brief Компонент FMU
      fmiReal _StartTime_d = 0.0; ///< @brief Время запуска
      fmiReal _EndTime_d = 0.0; ///< @brief Время останова
      fmiEventInfo _EventInfo_o; ///< @brief Обновление при вызове, инициализации и обновлении прерывания
      fmiCallbackFunctions _Callbacks_o; ///< @brief Набор Callback-функций для работы с моделью
      int nx = 0; ///< @brief Количество состояний переменных
      int nz = 0; ///< @brief Количество состояний событий переменных
      double * x, * xdot, * z, * prez; ///< @brief Промежуточные состояния
      double _CurrentTime_d = 0.0; ///< @brief Текущее время

      std::map<std::string, int> _OutVarNames_map; ///< @brief Набор всех выходных имен
      std::map<std::string, int> _InVarNames_map; ///< @brief Набор всех входных имен
      std::map<std::string, bool> _BooleansVar_map; ///< @brief Набор значений переменных типа BOOL
      std::map<std::string, int> _IntagersVar_map; ///< @brief Набор значений переменных типа INT
      std::map<std::string, double> _DoublesVar_map; ///< @brief Набор значений переменных типа double
      std::map<std::string, std::string> _StringsVar_map; ///< @brief Набор значений переменных типа String

      /** @brief Формирование типов переменных */
      void FormationVarTypes_v();
      /** @brief Заполнение данными */
      void FillVariables_v();
    };

  } //  namespace fmi10
} // namespace fmuw

#endif // CMEDATA_H
