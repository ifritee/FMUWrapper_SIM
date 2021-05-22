#ifndef LIBFMUWRAPPER_GLOBAL_H
#define LIBFMUWRAPPER_GLOBAL_H

/** @file libfmuwrapper_global.h
 *  @brief Нужен для нормального формирования основной библиотеки - прописывает EXPORT или IMPORT в библиотечных методах.
 *  @date 16.05.2021
 *  @author Никишин Е. В.
 */

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define Q_DECL_EXPORT __declspec(dllexport)
#  define Q_DECL_IMPORT __declspec(dllimport)
#else
#  define Q_DECL_EXPORT     __attribute__((visibility("default")))
#  define Q_DECL_IMPORT     __attribute__((visibility("default")))
#endif

#if defined(FMUWRAPPER_LIBRARY)
#  define FMUWRAPPER_EXPORT Q_DECL_EXPORT
#else
#  define FMUWRAPPER_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBFMUWRAPPER_GLOBAL_H
