#ifndef _CPPLANG_
#define _CPPLANG_

#if defined(__clang__) || defined(__GNUC__)
#   define __CPP_LANG __cplusplus
#elif defined(_MSC_VER)
#   define __CPP_LANG _MSVC_LANG
#endif

#define __CPP_LANG_03 199711L
#define __CPP_LANG_11 201103L
#define __CPP_LANG_14 201402L
#define __CPP_LANG_17 201703L
#define __CPP_LANG_20 202002L

//C++ pre - C++98: __cplusplus is 1
//C++98 : __cplusplus is 199711L
//C++11 : __cplusplus is 201103L
//C++14 : __cplusplus is 201402L
//C++17 : __cplusplus is 201703L
//C++20 : __cplusplus is 202002L

#if __CPP_LANG >= 199711L
#   define __CPP03 1
#endif
#if __CPP_LANG >= 201103L
#   define __CPP11 1
#endif
#if __CPP_LANG >= 201402L
#   define __CPP14 1
#endif
#if __CPP_LANG >= 201703L
#   define __CPP17 1
#endif
#if __CPP_LANG >= 202002L
#   define __CPP20 1
#endif



#endif