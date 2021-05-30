#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ios>
#include <boost/filesystem.hpp>
#include <string.h>

#define _Nullable
#define _Nonnull

#include "zip.h"
#include "libfmuwrapper.h"
#include "FMUWork.h"

static std::string last_error; ///< @brief Последняя ошибка
static std::vector<fmuw::FMUWork *> temp_data; ///< @brief Набор временных файлов

int unzipFMU(const char *file, const char *dst)
{
  int errorState = 0;
  struct zip_stat zipStat;
  struct zip_file * zipFile = nullptr;
  char buffer[1024];

  zip * zipStream = zip_open(file, 0, &errorState);
  if (zipStream == 0) {
    zip_error_to_str(buffer, sizeof(buffer), errorState, errno);
    last_error = buffer;
    return CODE_FAILED;
  }
  std::string baseName = boost::filesystem::basename(file);
  std::string outDirectory = std::string(dst) + "/" + baseName;
  if (boost::filesystem::create_directory(outDirectory) == false) {
    last_error = "Do not create out directory";
    zip_close(zipStream);
    return CODE_FAILED;
  }
  for (int i = 0; i < zip_get_num_entries(zipStream, 0); ++i) {
    if(zip_stat_index(zipStream, i, 0, &zipStat) == 0) {
      std::string name = outDirectory + "/" + zipStat.name;
      std::cout<<"FILE: "<<name<<std::endl;
      if (name[name.size() - 1] == '/') {
        boost::filesystem::create_directory(name);
      } else {
        zipFile = zip_fopen_index(zipStream, i, 0);
        if (zipFile == 0) {
          last_error = "Unzipped file index is crashed";
          return CODE_FAILED;
        }
        std::ofstream osFile;
        try {
          osFile.open(name, std::ios::out | std::ios::app | std::ios::binary);
          unsigned int sum = 0;
          while (sum != zipStat.size) {
            int length = zip_fread(zipFile, buffer, sizeof(buffer));
            if (length < 0) {
              last_error = "Unzipped file is crashed!";
              return CODE_FAILED;
            }
            osFile.write(buffer, length);
            sum += length;
          }
          osFile.close();
        } catch(std::exception & e) {
          osFile.close();
        }
      }
    } else {
      last_error = "ZIP file get status is failed!";
      return CODE_FAILED;
    }
  }
  zip_close(zipStream);
  fmuw::FMUWork * work = new fmuw::FMUWork(outDirectory);
  temp_data.push_back(work);
  return temp_data.size() - 1;
}

int removeTmp(int number)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    boost::filesystem::remove_all(work->fmuPath());
    delete work;
    temp_data[number] = nullptr;
  }
  return CODE_OK;
}

void lastError(char *error, const size_t size)
{
  if (size >= last_error.size()) {
    strcpy(error, last_error.c_str());
  } else {
    memcpy(error, last_error.c_str(), size);
    *(error + size - 1) = '\0';
  }
}

int parsing(int number)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    if ( work->DescriptionRead_b() == false) {
      last_error = work->lastError();
      return CODE_FAILED;
    }
  }
  return CODE_OK;
}

int initialize(int number, double entTime)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    try {
      work->modelInit(entTime);
    } catch(...) {
      last_error = work->lastError();
      return CODE_FAILED;
    }
  }
  return CODE_OK;
}
