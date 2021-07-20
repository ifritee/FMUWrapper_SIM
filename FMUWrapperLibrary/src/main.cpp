#include <iostream>
#include <vector>

//#include "zip.h"
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
    const char * fmu = "/home/ifritee/aaa/fmusdk/dist/fmu20/me/values.fmu";
//    const char * fmu = "/home/ifritee/aaa/fmusdk/dist/fmu20/me/values.fmu";

  system("cd /home/ifritee/aaa/fmusdk/dist/fmu20/me; 7z x -y values.fmu -ovalues");
  int module = createFMU("/home/ifritee/aaa/fmusdk/dist/fmu20/me/values");
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
    std::vector<std::pair<std::string, int> > outVars, inVars;
    for(int i = 0; i < outputVarQty; ++i) {
      if( outputVar(module, i, buffer, sizeof(buffer), type) < CODE_OK) {
        break;
      }
      std::cout<<"OUT VAR >> "<<buffer<<" TYPE >> "<<type<<std::endl;
      outVars.push_back(std::pair<std::string, int>(std::string(buffer), type));
    }
    std::cout<<std::endl;
    // 2. Задание начальных значений свойств модели
    int inputVarQty = inputsQty(module);
    for(int i = 0; i < inputVarQty; ++i) {
      if( inputVar(module, i, buffer, sizeof(buffer), type) < CODE_OK) {
        break;
      }
      std::cout<<"IN VAR >> "<<buffer<<" TYPE >> "<<type<<std::endl;
      inVars.push_back(std::pair<std::string, int>(std::string(buffer), type));
    }
    // 3. Цикл по шагам симуляции модели
    for (int i = 0; i < 10; ++i) {
      // 4. Берем выходные данные
      for(auto p : outVars) {
        switch (p.second) {
        case fmuw::FTReal_en: {  // double
          double var = getDouble(module, p.first.c_str());
          std::cout<<"DOUBLE ::: "<<p.first<<" = "<<var<<std::endl;
        } break;
        case fmuw::FTInteger_en: case fmuw::FTEnumeration_en: {
          int var = getInt(module, p.first.c_str());
          std::cout<<"INTEGER ::: "<<p.first<<" = "<<var<<std::endl;
        } break;
        case fmuw::FTBoolean_en: {
          int var = getBool(module, p.first.c_str());
          std::cout<<"BOOLEAN ::: "<<p.first<<" = "<<var<<std::endl;
        } break;
        case fmuw::FTString_en: {
          char temp[1024];
          int length = getString(module, p.first.c_str(), temp, 1024);
          std::cout<<"STRING ::: "<<p.first<<" = "<<length<<" >> "<<temp<<std::endl;
        } break;
        default:
          break;
        }
      }
      int varInt = 5;
      bool varBool = 1;
      double varReal = 40.1;
      // 5. Устанавливаем входные данные
      for(auto p : inVars) {
        switch (p.second) {
        case fmuw::FTReal_en: {  // double
          setDouble(module, p.first.c_str(), varReal);
        } break;
        case fmuw::FTInteger_en: case fmuw::FTEnumeration_en: {
          setInt(module, p.first.c_str(), varInt);
        } break;
        case fmuw::FTBoolean_en: {
          setBool(module, p.first.c_str(), varBool);
        } break;
        case fmuw::FTString_en: {
          char temp_str[] = "AaAaA";
          setString(module, p.first.c_str(), temp_str);
        } break;
        }
      }
      // 6. Шаг
      if ( step(module) < CODE_OK) {

      }
    }
//    std::cout<<std::endl;
//    if ( initialize(module, time_end, oneStep) < CODE_OK) {

//    }
//    for (int i = 0; i < 10; ++i) {
//      if ( step(module) < CODE_OK) {

//        // 4. Задание очередных значений свойств модели
//        // 5. Шаг
//        // 6. Считывание состояния индикаторов модели
//      }
//      for(auto p : outVars) {
//        switch (p.second) {
//        case fmuw::FTReal_en: {  // double
//          double var = getDouble(module, p.first.c_str());
//          if (p.first == "h") {
//            std::cout<<p.first<<" = "<<var<<std::endl;
//          }
//        } break;
//        default:
//          break;
//        }
//      }
//    }

    // 7. Завершение работы с моделью
  }

  if ( freeFMU(module) < CODE_OK) {
    char error_buffer[1024] = {0};
    lastError(error_buffer, sizeof(error_buffer));
    std::cerr<<error_buffer<<std::endl;
  }
  return 0;
}

//  int errorState = 0;
//  struct zip_stat zipStat;
//  struct zip_file * zipFile = nullptr;
//  char buffer[1024];

//  zip * zipStream = zip_open(file, 0, &errorState);
//  if (zipStream == 0) {
//    zip_error_to_str(buffer, sizeof(buffer), errorState, errno);
//    last_error = buffer;
//    return CODE_FAILED;
//  }
//  std::filesystem::path filePath(file);
//  std::string baseName = filePath.stem().u8string();
//  std::string outDirectory = std::string(dst) + "/" + baseName;
//  if (std::filesystem::create_directory(outDirectory) == false) {
//    last_error = "Do not create out directory";
//    zip_close(zipStream);
//    return CODE_FAILED;
//  }
//  for (int i = 0; i < zip_get_num_entries(zipStream, 0); ++i) {
//    if(zip_stat_index(zipStream, i, 0, &zipStat) == 0) {
//      std::string name = outDirectory + "/" + zipStat.name;
//      std::cout<<"FILE: "<<name<<std::endl;
//      if (name[name.size() - 1] == '/') {
//        std::filesystem::create_directory(name);
//      } else {
//        zipFile = zip_fopen_index(zipStream, i, 0);
//        if (zipFile == 0) {
//          last_error = "Unzipped file index is crashed";
//          return CODE_FAILED;
//        }
//        std::ofstream osFile;
//        try {
//          osFile.open(name, std::ios::out | std::ios::app | std::ios::binary);
//          unsigned int sum = 0;
//          while (sum != zipStat.size) {
//            int length = zip_fread(zipFile, buffer, sizeof(buffer));
//            if (length < 0) {
//              last_error = "Unzipped file is crashed!";
//              return CODE_FAILED;
//            }
//            osFile.write(buffer, length);
//            sum += length;
//          }
//          osFile.close();
//        } catch(std::exception & e) {
//          osFile.close();
//        }
//      }
//    } else {
//      last_error = "ZIP file get status is failed!";
//      return CODE_FAILED;
//    }
//  }
//  zip_close(zipStream);
