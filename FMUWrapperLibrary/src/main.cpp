#include <iostream>

#include "libfmuwrapper.h"

int main(int argc, char *argv[])
{
  // 1. Указываем файл с fmu моделью ../Demo/demo_001/BouncingBall.fmu
  // 2. Получаем информацию о модели
  // 3. В зависимости от версии модели подгружаем нужные функции
//  const char * fmu = "../Demo/demo_FMI_1_me/bouncingBall.fmu";
  const char * fmu = "/home/ifritee/aaa/fmusdk/dist/fmu10/me/values.fmu";
//  const char * fmu = "/home/ifritee/aaa/fmusdk/dist/fmu10/cs/bouncingBall.fmu";
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
    if ( initialize(module, 20.0, 0.1) < CODE_OK) {

    }

    // 2. Задание начальных значений свойств модели
    // 3. Цикл по шагам симуляции модели
    // 4. Задание очередных значений свойств модели
    // 5. Шаг
    // 6. Считывание состояния индикаторов модели
    // 7. Завершение работы с моделью
  }

  if ( removeTmp(module) < CODE_OK) {
    char error_buffer[1024] = {0};
    lastError(error_buffer, sizeof(error_buffer));
    std::cerr<<error_buffer<<std::endl;
  }
  return 0;
}

