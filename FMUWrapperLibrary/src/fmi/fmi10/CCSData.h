#ifndef CCSDATA_H
#define CCSDATA_H

#include <string>
#include <map>

#include "fmiModelTypes.h"
#include "fmiFunctions.h"

namespace fmuw
{
  namespace fmi10
  {

    class FMU;

    class CCSData
    {
    public:
      /** @brief Конструктор */
      explicit CCSData(const char * guid, FMU * fmu);
      /** @brief Деструктор */
      virtual ~CCSData();
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
      /**
       *  @brief Один шаг расчета */
      void step( double stepSize);
      /** @brief Передает набор выходных пинов */
      const std::map<std::string, int> & outVarNames() const { return _OutVarNames_map; }
      /** @brief Передает набор входных пинов */
      const std::map<std::string, int> & inVarNames() const { return _InVarNames_map; }
      /** @brief Возвращает значение по имени */
      bool boolVar(const std::string &name) const;
      /** @brief Возвращает значение по имени */
      int intVar(const std::string &name) const;
      /** @brief Возвращает значение по имени */
      double doubleVar(const std::string &name) const;
      /** @brief Возвращает значение по имени */
      std::string stringVar(const std::string &name) const;

    private:
      fmi10::FMU * _Model_po; ///< @brief Структура данных модели
      std::string _ModelID_str; ///< @brief global unique id of the fmu
      fmiComponent _Component_o; ///< @brief Компонент FMU
      fmiReal _StartTime_d = 0.0; ///< @brief Время запуска
      fmiReal _EndTime_d = 0.0; ///< @brief Время останова
      fmiEventInfo _EventInfo_o; ///< @brief Обновление при вызове, инициализации и обновлении прерывания
      fmiCallbackFunctions _Callbacks_o; ///< @brief Набор Callback-функций для работы с моделью
      double _CurrentTime_d = 0.0; ///< @brief Текущее время

      std::map<std::string, int> _OutVarNames_map; ///< @brief Набор всех выходных имен
      std::map<std::string, int> _InVarNames_map; ///< @brief Набор всех входных имен
      std::map<std::string, bool> _BooleansVar_map; ///< @brief Набор значений переменных типа BOOL
      std::map<std::string, int> _IntegersVar_map; ///< @brief Набор значений переменных типа INT
      std::map<std::string, double> _DoublesVar_map; ///< @brief Набор значений переменных типа double
      std::map<std::string, std::string> _StringsVar_map; ///< @brief Набор значений переменных типа String
      bool _IsSimulationEnd = true; ///< @brief Флаг завершения симуляции
      double _SaveStepSize_d = 0.0;

      /** @brief Формирование типов переменных */
      void FormationVarTypes_v();
      /** @brief Заполнение данными */
      void FillVariables_v();
    };

  } //fmi10
} // fmuw

#endif // CCSDATA_H
