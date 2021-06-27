#include <iostream>
#include <vector>

#include "Cconstants.h"
#include "libfmuwrapper.h"

int main(int argc, char *argv[])
{
  // 1. Указываем файл с fmu моделью ../Demo/demo_001/BouncingBall.fmu
  // 2. Получаем информацию о модели
  // 3. В зависимости от версии модели подгружаем нужные функции
//  const char * fmu = "/home/ifritee/aaa/fmusdk/dist/fmu10/cs/bouncingBall.fmu";
//  const char * fmu = "../Demo/demo_FMI_1_me/bouncingBall.fmu";
//  const char * fmu = "/home/ifritee/aaa/fmusdk/dist/fmu10/me/values.fmu";
//  const char * fmu = "/home/ifritee/aaa/fmusdk/dist/fmu10/cs/bouncingBall.fmu";
//    const char * fmu = "/home/ifritee/tmp/FMUWrapper_SIM/Demo/demo_FMI_2_cs/bouncingBall.fmu";
//    const char * fmu = "/home/ifritee/tmp/FMUWrapper_SIM/Demo/demo_FMI_2_me/bouncingBall.fmu";
    const char * fmu = "/home/ifritee/tmp/FMUWrapper_SIM/Demo/demo_FMI_2_cs/bouncingBall.fmu";
//    const char * fmu = "/home/ifritee/aaa/fmusdk/dist/fmu20/me/values.fmu";

  int module = unzipFMU(fmu, ".");
  if (module < CODE_OK) {
    char error_buffer[1024] = {0};
    lastError(error_buffer, sizeof(error_buffer));
    std::cerr<<error_buffer<<std::endl;
  } else {
    if ( parsing(module) < CODE_OK) {
      char error_buffer[1024] = {0};
      lastError(error_buffer, sizeof(error_buffer));
      std::cerr<<error_buffer<<std::endl;
    }
    //----- Работа с моделью -----
    // 1. Инициализация модели
    double time_end = 20.0, oneStep = 0.1;
    if ( initialize(module, time_end, oneStep) < CODE_OK) {

    }
    char buffer[1024];
    int type = 0;
    int outputVarQty = outputsQty(module);
    std::vector<std::pair<std::string, int> > outVars;
    for(int i = 0; i < outputVarQty; ++i) {
      if( outputVar(module, i, buffer, sizeof(buffer), type) < CODE_OK) {
        break;
      }
      std::cout<<"OUT VAR >> "<<buffer<<" TYPE >> "<<type<<std::endl;
      outVars.push_back(std::pair<std::string, int>(std::string(buffer), type));
    }

    // 2. Задание начальных значений свойств модели
    // 3. Цикл по шагам симуляции модели
    for (int i = 0; i < 2000; ++i) {
      if ( step(module) < CODE_OK) {

        // 4. Задание очередных значений свойств модели
        // 5. Шаг
        // 6. Считывание состояния индикаторов модели
      }
      for(auto p : outVars) {
        switch (p.second) {
        case fmuw::FTReal_en: {  // double
          double var = getDouble(module, p.first.c_str());
          if (p.first == "h") {
            std::cout<<p.first<<" = "<<var<<std::endl;
          }
        } break;
        default:
          break;
        }
      }
    }

    // 7. Завершение работы с моделью
  }

  if ( removeTmp(module) < CODE_OK) {
    char error_buffer[1024] = {0};
    lastError(error_buffer, sizeof(error_buffer));
    std::cerr<<error_buffer<<std::endl;
  }
  return 0;
}

