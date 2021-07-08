#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ios>
#include <filesystem>
#include <string.h>

#define _Nullable
#define _Nonnull

#include "libfmuwrapper.h"
#include "FMUWork.h"

static std::string last_error; ///< @brief Последняя ошибка
static std::vector<fmuw::FMUWork *> temp_data; ///< @brief Набор временных файлов

int createFMU(const char * path)
{
  fmuw::FMUWork * work = new fmuw::FMUWork(path);
  temp_data.push_back(work);
  return temp_data.size() - 1;
}

int freeFMU(int number)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    //std::filesystem::remove_all(work->fmuPath());
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

int initialize(int number, double entTime, double stepSize)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    try {
      work->modelInit(entTime, stepSize);
    } catch(...) {
      last_error = work->lastError();
      return CODE_FAILED;
    }
  }
  return CODE_OK;
}

int outputsQty(int number)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    try {
      return work->outputsQty();
    } catch(...) {
      last_error = work->lastError();
      return -1;
    }
  }
  return 0;
}

int step(int number)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    try {
      work->modelStep();
    } catch(...) {
      last_error = work->lastError();
      return CODE_FAILED;
    }
  }
  return CODE_OK;
}



int outputVar(int number, int index, char *nameBuffer, int length, int &type)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    try {
      std::string name = work->outputVar(index, type);
      if (name.size() > length) {
        return CODE_FAILED;
      }
      strcpy(nameBuffer, name.c_str());
    } catch(...) {
      last_error = work->lastError();
      return CODE_FAILED;
    }
  }
  return CODE_OK;
}

double getDouble(int number, const char *name)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    try {
      return work->doubleValue(name);

    } catch(...) {
      last_error = work->lastError();
      return 0.0;
    }
  }
  return 0.0;
}

bool getBool(int number, const char *name)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    try {
      return work->boolValue(name);

    } catch(...) {
      last_error = work->lastError();
      return false;
    }
  }
  return false;
}

int getInt(int number, const char *name)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    try {
      return work->intValue(name);

    } catch(...) {
      last_error = work->lastError();
      return 0;
    }
  }
  return 0;
}

int getString(int number, const char *name, char *buffer, int length)
{
  if (number < temp_data.size() && temp_data[number] != nullptr) {
    fmuw::FMUWork * work = temp_data[number];
    try {
      std::string value = work->strValue(name);
      if (value.size() > length) {
        last_error = "size buffer is too small";
        return -1;
      }
      strcpy(buffer, value.c_str());
    } catch(...) {
      last_error = work->lastError();
    }
  }
  return -1;
}
