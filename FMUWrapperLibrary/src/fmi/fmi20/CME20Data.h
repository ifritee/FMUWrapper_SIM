#pragma once

#include <string>
#include <map>

#include "fmi2FunctionTypes.h"
#include "fmi2TypesPlatform.h"

namespace fmuw
{
  namespace fmi20 
  {
    struct FMU;

    /**
       * @class CMEData
       * @brief Обработка модели, получение значений и отработка модели по шагам */
    class CME20Data
    {
    public:
      /** @brief Конструктор */
      explicit CME20Data(FMU* fmu, const std::string& path);
      /** @brief Деструктор */
      virtual ~CME20Data();
      /** @brief Освобождение ресурсов */
      void freeData();
      /** @brief создание компонентов */
      void createComponent();
      /**
       * @brief initialize Инициализация модели
       * @param tStart Время запуска
       * @param tEnd Время остановки
       * @param stepSize Размер шага */
      void Initialize_v(fmi2Real tStart, fmi2Real tEnd);
      /**
         * @brief setBoolData Задание значение входной переменной
         * @param name Имя переменной
         * @param value значение переменной
         * @param qty количество переменных */
      void setBoolData(const std::string& name, const int* value, size_t qty = 1);
      /**
         * @brief setIntData Задание значение входной переменной
         * @param name Имя переменной
         * @param value значение переменной
         * @param qty количество переменных */
      void setIntData(const std::string& name, const int* value, size_t qty = 1);
      /**
         * @brief setRealData Задание значение входной переменной
         * @param name Имя переменной
         * @param value значение переменной
         * @param qty количество переменных */
      void setRealData(const std::string& name, const double* value, size_t qty = 1);
      /**
         * @brief setStrData Задание значение входной переменной
         * @param name Имя переменной
         * @param value значение переменной
         * @param qty количество переменных */
      void setStrData(const std::string& name, const char* value[], size_t qty = 1);

      /** @brief Один шаг расчета */
      void step(double stepSize);
      /** @brief Передает набор выходных пинов */
      const std::map<std::string, int>& outVarNames() const;
      /** @brief Передает набор входных пинов */
      const std::map<std::string, int>& inVarNames() const;
      /** @brief Возвращает значение по имени */
      bool boolVar(const std::string& name) const;
      /** @brief Возвращает значение по имени */
      int intVar(const std::string& name) const;
      /** @brief Возвращает значение по имени */
      double doubleVar(const std::string& name) const;
      /** @brief Возвращает значение по имени */
      std::string stringVar(const std::string& name) const;

    private:
      fmi20::FMU* _Model_po; ///< @brief Структура данных модели
      std::string _ModelID_str; ///< @brief global unique id of the fmu
      std::string _InstanceName_str; ///< @brief Итендификатор модели
      fmi2Component _Component_o; ///< @brief Компонент FMU
      fmi2Real _StartTime_d = 0.0; ///< @brief Время запуска
      fmi2Real _EndTime_d = 0.0; ///< @brief Время останова
      fmi2Boolean _StepEvent_b = 0;
      std::string _ModulePath_str; ///< @brief Путь к файлам модели
      fmi2CallbackFunctions* _Callbacks_po;  ///< @brief Набор функций обратного вызова
      fmi2EventInfo _EventInfo_o; ///< @brief Обновление при вызове, инициализации и обновлении прерывания
      int nx;///< @brief number of state variables
      int nz;///< @brief number of state event indicators
      double* x = nullptr;    ///< @brief continuous states
      double* xdot = nullptr; ///< @brief the corresponding derivatives in same order
      double* z = nullptr;    ///< @brief state event indicators
      double* prez = nullptr; ///< @brief previous values of state event indicators
      double _CurrentTime_d = 0.0; ///< @brief Текущее время

      std::map<std::string, int> _OutVarNames_map; ///< @brief Набор всех выходных имен
      std::map<std::string, int> _InVarNames_map; ///< @brief Набор всех входных имен
      std::map<std::string, bool> _BooleansVar_map; ///< @brief Набор значений переменных типа BOOL
      std::map<std::string, int> _IntegersVar_map; ///< @brief Набор значений переменных типа INT
      std::map<std::string, double> _DoublesVar_map; ///< @brief Набор значений переменных типа double
      std::map<std::string, std::string> _StringsVar_map; ///< @brief Набор значений переменных типа String
      bool _IsSimulationEnd = true; ///< @brief Флаг завершения симуляции

      /** @brief Формирование типов переменных */
      void FormationVarTypes_v();
      /** @brief Заполнение данными */
      void FillVariables_v();
    };
  } //  namespace fmi20
} //  namespace fmuw::fmi20
