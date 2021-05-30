#include <iostream>

#include "libfmuwrapper.h"

int main(int argc, char *argv[])
{
  // 1. Указываем файл с fmu моделью ../Demo/demo_001/BouncingBall.fmu
  // 2. Получаем информацию о модели
  // 3. В зависимости от версии модели подгружаем нужные функции
//  const char * fmu = "../Demo/demo_FMI_1_me/bouncingBall.fmu";
  const char * fmu = "/home/ifritee/aaa/fmusdk/dist/fmu10/cs/bouncingBall.fmu";
  int module = unzipFMU(fmu, ".");
  if (module < CODE_OK) {
    char error_buffer[1024] = {0};
    lastError(error_buffer, sizeof(error_buffer));
    std::cerr<<error_buffer<<std::endl;
  } else {
    if ( parsing(module) < CODE_OK) {

    }
  }

  if ( removeTmp(module) < CODE_OK) {
    char error_buffer[1024] = {0};
    lastError(error_buffer, sizeof(error_buffer));
    std::cerr<<error_buffer<<std::endl;
  }
  return 0;
}

