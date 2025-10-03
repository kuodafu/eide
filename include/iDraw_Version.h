#pragma once

#define ___VER_ADD_L1(_t) L##_t
#define ___VER_ADD_L(_t) ___VER_ADD_L1(_t)
#define ___VER_1(_major, _minor, _build) #_major##"_"###_minor##"_"###_build
#define ___VER_2(_major, _minor, _build) #_major##"."###_minor##"."###_build
#define ___VER_3(_major, _minor, _build) ___VER_1(_major, _minor, _build)
#define ___VER_4(_major, _minor, _build) ___VER_2(_major, _minor, _build)


#define IDRAW_MAJORVERSION               1      // 支持库主版本号
#define IDRAW_MINORVERSION               5      // 支持库次版本号
#define IDRAW_BUILDVERSION               1003   // 支持库编译版本号
#define IDRAW_VERSION                    ___VER_ADD_L(___VER_4(IDRAW_MAJORVERSION, IDRAW_MINORVERSION, IDRAW_BUILDVERSION))     // 支持库版本号
#define IDRAW_VERSIONA                   ___VER_4(IDRAW_MAJORVERSION, IDRAW_MINORVERSION, IDRAW_BUILDVERSION)                   // 支持库版本号
#define IDRAW_VERSION_                   ___VER_ADD_L(___VER_3(IDRAW_MAJORVERSION, IDRAW_MINORVERSION, IDRAW_BUILDVERSION))     // 支持库版本号, 点换成下划线 6_1_101
#define IDRAW_VERSIONA_                  ___VER_3(IDRAW_MAJORVERSION, IDRAW_MINORVERSION, IDRAW_BUILDVERSION)                   // 支持库版本号, 点换成下划线 6_1_101






