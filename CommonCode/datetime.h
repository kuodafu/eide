#pragma once
#include <time.h>

enum DATE_TYPE
{
    DATE_TYPE_SEC,      // 秒
    DATE_TYPE_MIN,      // 分钟
    DATE_TYPE_HOUR,     // 小时
    DATE_TYPE_DAY,      // 日
    DATE_TYPE_MON,      // 月
    DATE_TYPE_YEAR,     // 年
};

#define _DATETIME_DEBUG_    0       // 改为1的话就不使用模版, 方便调试

#if _DATETIME_DEBUG_
typedef char value_type;
#else
template<typename value_type>
#endif
class datetime_base
{
private:
    typedef value_type*             pointer;
    typedef const value_type*       const_pointer;
    typedef value_type&             reference;
    typedef const value_type&       const_reference;

    time_t  m_time; // 当前对象记录的时间, 从1970年经过的秒数
    tm      m_tm;   
    pointer m_str;
    inline void _init_str() {
        memset(this, 0, sizeof(*this));
        m_str = (pointer)malloc(50 * sizeof(value_type));
        if (m_str) *m_str = 0;
    }
public:
    datetime_base() :m_time(0) {  _init_str(); }
    datetime_base(LPCSTR time) {
        _init_str();
        operator()(time);
    }

    datetime_base(LPCWSTR time) {
        _init_str();
        operator()(time);
    }

    datetime_base(time_t time) {
        _init_str();
        m_time = time;
        operator()(time);
    }

    datetime_base(int year, int month, int day, int hour, int minute, int second) {
        _init_str();
        operator()(year, month, day, hour, minute, second);
    }

    datetime_base(const LPSYSTEMTIME time) {
        _init_str();
        if (!time) return;
        operator()(time->wYear, time->wMonth, time->wDay, time->wHour, time->wMinute, time->wSecond);
    }
    datetime_base(const datetime_base& obj) {
        _init_str();
        m_time = obj.m_time;
        m_tm = obj.m_tm;
        if (obj.m_str) {
            size_t len = __lstrlen(obj.m_str) + 1;
            if (len > 49)len = 49;
            memcpy(m_str, obj.m_str, sizeof(wchar_t) * len);
        }
    }
#if _MSC_VER > 1200
    datetime_base(datetime_base&& obj) noexcept {
        memset(this, 0, sizeof(*this));
        m_time = obj.m_time;
        m_tm = obj.m_tm;
        m_str = obj.m_str;
        obj.m_str = 0;
    }
#endif
    ~datetime_base() {
        if (m_str) ::free(m_str);
        memset(this, 0, sizeof(*this));
    }

public:
    inline operator const pointer ()    { return m_str; }
    inline operator time_t()            { return m_time; }
    inline operator tm()                { return m_tm; }
    //inline operator double()            { return (double)m_time; }
    inline operator SYSTEMTIME() {
        SYSTEMTIME _t;
        _t.wYear        = m_tm.tm_year + 1900;
        _t.wMonth       = m_tm.tm_mon + 1;
        _t.wDayOfWeek   = m_tm.tm_wday;
        _t.wDay         = m_tm.tm_mday;
        _t.wHour        = m_tm.tm_hour;
        _t.wMinute      = m_tm.tm_min;
        _t.wSecond      = m_tm.tm_sec;
        _t.wMilliseconds = 0;
        return _t;
    }


    // 清空当前类的时间
    inline datetime_base& operator()() {
        m_time = 0;
        m_tm = { 0 };
        if (m_str) m_str[0] = 0;
        return *this;
    }
    // 从时间文本里赋值
    inline datetime_base& operator()(LPCWSTR time) {
        m_time = make_time(time, &m_tm, m_str);
        return *this;
    }
    // 从时间文本里赋值
    inline datetime_base& operator()(LPCSTR time) {
        m_time = make_time(time, &m_tm, m_str);
        return *this;
    }
    // 从指定时间里赋值
    inline datetime_base& operator()(time_t time) {
        m_time = time;
        make_tm(m_time, &m_tm, m_str);
        return *this;
    }
    // 从指定年月日时分秒里赋值
    inline datetime_base& operator()(int year, int month, int day, int hour = 0, int minute = 0, int second = 0) {
        m_time = make_time(year, month, day, hour, minute, second, &m_tm, m_str);
        return *this;
    }
    // 从系统时间结构里赋值
    inline datetime_base& operator()(const LPSYSTEMTIME time) {
        if (!time) return;
        m_time = make_time(time->wYear, time->wMonth, time->wDay, time->wHour, time->wMinute, time->wSecond, &m_tm, m_str);
        return *this;
    }


    // 判断两个时间是否相等
    inline bool operator==(time_t               time) const { return m_time == time; }
    inline bool operator==(LPCWSTR              time) const { return m_time == make_time(time); }
    inline bool operator==(LPCSTR               time) const { return m_time == make_time(time); }
    inline bool operator==(const tm&            time) const { return m_time == make_time(time); }
    inline bool operator==(const tm*            time) const { return m_time == make_time(time); }
    inline bool operator==(const SYSTEMTIME&    time) const { return m_time == make_time(time); }
    inline bool operator==(const SYSTEMTIME*    time) const { return m_time == make_time(time); }

    // 判断当前对象的时间是否大于传递进来的时间
    inline bool operator>(time_t                time) const { return m_time > time; }
    inline bool operator>(LPCWSTR               time) const { return m_time > make_time(time); }
    inline bool operator>(LPCSTR                time) const { return m_time > make_time(time); }
    inline bool operator>(const tm&             time) const { return m_time > make_time(time); }
    inline bool operator>(const tm*             time) const { return m_time > make_time(time); }
    inline bool operator>(const SYSTEMTIME&     time) const { return m_time > make_time(time); }
    inline bool operator>(const SYSTEMTIME*     time) const { return m_time > make_time(time); }
                                                      
    // 判断当前对象的时间是否小于传递进来的时间              
    inline bool operator<(time_t                time) const { return m_time < time; }
    inline bool operator<(LPCWSTR               time) const { return m_time < make_time(time); }
    inline bool operator<(LPCSTR                time) const { return m_time < make_time(time); }
    inline bool operator<(const tm&             time) const { return m_time < make_time(time); }
    inline bool operator<(const tm*             time) const { return m_time < make_time(time); }
    inline bool operator<(const SYSTEMTIME&     time) const { return m_time < make_time(time); }
    inline bool operator<(const SYSTEMTIME*     time) const { return m_time < make_time(time); }

    inline time_t time() { return m_time; }
public:
    // 取指定时间, 参数分别是 年月日时分秒周几, 在一年中的第几天
    inline bool Getdatetime(int* year = 0, int* month = 0, int* day = 0, int* hour = 0, int* minute = 0, int* second = 0, int* wday = 0, int* yday = 0) const {
        if (m_time <= 0)return false;
        const tm* _tm = &m_tm;
        if (year    ) *year   = _tm->tm_year + 1900;  // 年, 由于tm结构体存储的是从1900年开始的时间, 所以临时变量int为tm_year加上1900
        if (month   ) *month  = _tm->tm_mon + 1;      // 月, 由于tm结构体的月份存储范围为0-11, 所以临时变量int为tm_mon加上1
        if (day     ) *day    = _tm->tm_mday;
        if (hour    ) *hour   = _tm->tm_hour;
        if (minute  ) *minute = _tm->tm_min;
        if (second  ) *second = _tm->tm_sec;
        if (wday    ) *wday   = _tm->tm_wday;
        if (yday    ) *yday   = _tm->tm_yday;
        return true;
    }

    inline int year()   const { return m_time ? m_tm.tm_year + 1900 : 0; }   // 年 
    inline int month()  const { return m_time ? m_tm.tm_mon + 1 : 0; }       // 月 1-12
    inline int day()    const { return m_time ? m_tm.tm_mday : 0; }          // 日 1-31
    inline int yday()   const { return m_time ? m_tm.tm_yday : 0; }          // 一年里的第几天, 1-365
    inline int wday()   const { return m_time ? m_tm.tm_wday + 1 : 0; }      // 周几 1-7
    inline int hour()   const { return m_time ? m_tm.tm_hour : 0; }          // 小时 0-23
    inline int minute() const { return m_time ? m_tm.tm_min : 0; }           // 分钟 0-59
    inline int second() const { return m_time ? m_tm.tm_sec : 0; }           // 秒 0-60

public:
    // 添加时间
    inline time_t add(int _off, DATE_TYPE type = DATE_TYPE_SEC) {
        m_time = add(m_time, _off, type);
        update_data(m_time);
        return m_time;
    }
    // 减少时间
    inline time_t sub(int _off, DATE_TYPE type = DATE_TYPE_SEC) {
        m_time = sub(m_time, _off, type);
        update_data(m_time);
        return m_time;
    }

    // 添加时间
    inline static time_t add(time_t time, int _off, DATE_TYPE type = DATE_TYPE_SEC) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        switch (type)
        {
        case DATE_TYPE_SEC   : _tm.tm_sec  += _off; break; // 秒
        case DATE_TYPE_MIN   : _tm.tm_min  += _off; break; // 分钟
        case DATE_TYPE_HOUR  : _tm.tm_hour += _off; break; // 小时
        case DATE_TYPE_DAY   : _tm.tm_mday += _off; break; // 日
        case DATE_TYPE_MON   : _tm.tm_mon  += _off; break; // 月
        case DATE_TYPE_YEAR  : _tm.tm_year += _off; break; // 年
        default:
            return time;
        }
        return make_time(&_tm);
    }

    // 减少时间
    inline static time_t sub(time_t time, int _off, DATE_TYPE type = DATE_TYPE_SEC) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        switch (type)
        {
        case DATE_TYPE_SEC   : _tm.tm_sec  -= _off; break; // 秒
        case DATE_TYPE_MIN   : _tm.tm_min  -= _off; break; // 分钟
        case DATE_TYPE_HOUR  : _tm.tm_hour -= _off; break; // 小时
        case DATE_TYPE_DAY   : _tm.tm_mday -= _off; break; // 日
        case DATE_TYPE_MON   : _tm.tm_mon  -= _off; break; // 月
        case DATE_TYPE_YEAR  : _tm.tm_year -= _off; break; // 年
        default:
            return time;
        }
        return make_time(&_tm);
    }

public:

    // 释放 to_string 申请的内存
    inline static void free(pointer buffer) {
        if (buffer) ::free(buffer);
    }

    // 把当前时间转换成文本, 如果参数1为空, 则内部申请空间存储, 调用放需要调用本对象的 free() 进行释放
    // 如果参数1不为空, 则参数1最少需要20个字符的尺寸, 返回值是返回这个地址
    // 参数1为空时需要调用 本对象的 free() 释放, 参数1至少20个字符
    inline static pointer to_string(pointer buffer = 0) {
        return to_string(::time(0), buffer);
    }

    // 参数2为空时需要调用 本对象的 free() 释放, 参数2至少20个字符
    // 如果参数2为空, 则内部申请空间存储, 调用放需要调用本对象的 free() 进行释放
    // 如果参数2不为空, 则参数2最少需要20个字符的尺寸, 返回值是返回这个地址
    inline static pointer to_string(time_t time, pointer buffer = 0) {
        tm _tm;
        make_tm(time, &_tm);
        return to_string(&_tm, buffer);
        // 2020-20-20 20-20-20
    }

    // 参数2为空时需要调用 本对象的 free() 释放, 参数2至少20个字符
    // 如果参数2为空, 则内部申请空间存储, 调用放需要调用本对象的 free() 进行释放
    // 如果参数2不为空, 则参数2最少需要20个字符的尺寸, 返回值是返回这个地址
    inline static pointer to_string(const tm& _tm, pointer buffer = 0) {
        return to_string(&_tm, buffer);
    }
    // 参数2为空时需要调用 本对象的 free() 释放, 参数2至少20个字符
    // 如果参数2为空, 则内部申请空间存储, 调用放需要调用本对象的 free() 进行释放
    // 如果参数2不为空, 则参数2最少需要20个字符的尺寸, 返回值是返回这个地址
    inline static pointer to_string(const tm* _tm, pointer buffer = 0) {
        if (!_tm)return 0;
        if (buffer == 0) buffer = new value_type[20];
        memset(buffer, 0, 20 * sizeof(value_type));
        int year = _tm->tm_year + 1900;
        if ( year > 9999 )
            year = 9999;
        if (sizeof(value_type) == 1)
        {
            const char* const fmt = "%04d-%02d-%02d %02d:%02d:%02d";
#if _MSC_VER > 1200
            sprintf_s((char*)buffer, 20, fmt,
                      year, _tm->tm_mon + 1, _tm->tm_mday,
                      _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
#else
            sprintf((char*)buffer, fmt,
                    year, _tm->tm_mon + 1, _tm->tm_mday,
                    _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
#endif
            return buffer;

        }
        const wchar_t* const fmt = L"%04d-%02d-%02d %02d:%02d:%02d";
#if _MSC_VER > 1200
        swprintf_s((wchar_t*)buffer, 20, fmt,
                   year, _tm->tm_mon + 1, _tm->tm_mday,
            _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
#else
        swprintf((wchar_t*)buffer, fmt,
                 year, _tm->tm_mon + 1, _tm->tm_mday,
                 _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
#endif
        return buffer;
    }


    // 参数2为空时需要调用 本对象的 free() 释放, 参数2至少20个字符
    // 如果参数2为空, 则内部申请空间存储, 调用放需要调用本对象的 free() 进行释放
    // 如果参数2不为空, 则参数2最少需要20个字符的尺寸, 返回值是返回这个地址
    inline static pointer to_string(const SYSTEMTIME* time, pointer buffer = 0) {
        if (!time) return 0;
        make_time(time->wYear, time->wMonth, time->wDay, time->wHour, time->wMinute, time->wSecond, 0, buffer);
        return buffer;
    }

    // 参数2为空时需要调用 本对象的 free() 释放, 参数2至少20个字符
    // 如果参数2为空, 则内部申请空间存储, 调用放需要调用本对象的 free() 进行释放
    // 如果参数2不为空, 则参数2最少需要20个字符的尺寸, 返回值是返回这个地址
    inline static pointer to_string(const SYSTEMTIME& time, pointer buffer = 0) {
        return to_string(&time, buffer);
    }

    // 参数2为空时需要调用 本对象的 free() 释放, 参数2至少20个字符
    // 如果参数2为空, 则内部申请空间存储, 调用放需要调用本对象的 free() 进行释放
    // 如果参数2不为空, 则参数2最少需要20个字符的尺寸, 返回值是返回这个地址
    inline static pointer to_string(const FILETIME* time, pointer buffer = 0) {
        if (!time) return 0;
        SYSTEMTIME st = { 0 };
        FileTimeToSystemTime(time, &st);
        return to_string(&st, buffer);
    }

    // 参数2为空时需要调用 本对象的 free() 释放, 参数2至少20个字符
    // 如果参数2为空, 则内部申请空间存储, 调用放需要调用本对象的 free() 进行释放
    // 如果参数2不为空, 则参数2最少需要20个字符的尺寸, 返回值是返回这个地址
    inline static pointer to_string(const FILETIME& time, pointer buffer = 0) {
        return to_string(&time, buffer);
    }


    // make_time 转换成 time_t, 从1970年到现在的秒数
public:

    // 把文本转换成 time_t 类型, 返回这个时间从1970年经过的秒数
    // time = 时间文本, 格式是 2021-01-01 11:22:33
    // ref = 是否接收这个时间对应的tm结构, 为0则不接收
    // timeStr = 是否转换成文本, 如果不为0, 最少需要20个字符的缓冲区, 则把时间转换成 2021-01-01 11:22:33 这种格式
    inline static time_t make_time(LPCWSTR time, tm* ref = 0, pointer timeStr = 0) {
        int year = -1, month = -1, day = -1, hour = -1, minute = -1, second = -1;
#if _MSC_VER < 1201
        int l = swscanf(time, L"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
#else
        int l = swscanf_s(time, L"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
#endif
        return make_time(year, month, day, hour, minute, second, ref, timeStr);
    }
    // 把文本转换成 time_t 类型, 返回这个时间从1970年经过的秒数
    // time = 时间文本, 格式是 2021-01-01 11:22:33
    // ref = 是否接收这个时间对应的tm结构, 为0则不接收
    // timeStr = 是否转换成文本, 如果不为0, 最少需要20个字符的缓冲区, 则把时间转换成 2021-01-01 11:22:33 这种格式
    inline static time_t make_time(LPCSTR time, tm* ref = 0, pointer timeStr = 0) {
        int year = -1, month = -1, day = -1, hour = -1, minute = -1, second = -1;
#if _MSC_VER < 1201
        sscanf(time, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
#else
        sscanf_s(time, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
#endif

        return make_time(year, month, day, hour, minute, second, ref, timeStr);
    }

    // 把SYSTEMTIME结构转换成 time_t 类型, 返回这个时间从1970年经过的秒数
    // time = SYSTEMTIME 结构
    // ref = 是否接收这个时间对应的tm结构, 为0则不接收
    // timeStr = 是否转换成文本, 如果不为0, 最少需要20个字符的缓冲区, 则把时间转换成 2021-01-01 11:22:33 这种格式
    inline static time_t make_time(const SYSTEMTIME& time, tm* ref = 0, pointer timeStr = 0) {
        return make_time(&time, ref, timeStr);
    }
    // 把SYSTEMTIME结构转换成 time_t 类型, 返回这个时间从1970年经过的秒数
    // time = SYSTEMTIME 结构指针
    // ref = 是否接收这个时间对应的tm结构, 为0则不接收
    // timeStr = 是否转换成文本, 如果不为0, 最少需要20个字符的缓冲区, 则把时间转换成 2021-01-01 11:22:33 这种格式
    inline static time_t make_time(const SYSTEMTIME* time, tm* ref = 0, pointer timeStr = 0) {
        if (!time)return 0;
        return make_time(time->wYear, time->wMonth, time->wDay, time->wHour, time->wMinute, time->wSecond, ref, timeStr);
    }
    // 把tm结构转换成 time_t 类型, 返回这个时间从1970年经过的秒数
    // time = tm 结构指针
    // ref = 是否接收这个时间对应的tm结构, 为0则不接收
    // timeStr = 是否转换成文本, 如果不为0, 最少需要20个字符的缓冲区, 则把时间转换成 2021-01-01 11:22:33 这种格式
    inline static time_t make_time(const tm* time, tm* ref = 0, pointer timeStr = 0) {
        if (!time)return 0;
        return make_time(time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec, ref, timeStr);

    }
    // 把tm结构转换成 time_t 类型, 返回这个时间从1970年经过的秒数
    // time = tm 结构
    // ref = 是否接收这个时间对应的tm结构, 为0则不接收
    // timeStr = 是否转换成文本, 如果不为0, 最少需要20个字符的缓冲区, 则把时间转换成 2021-01-01 11:22:33 这种格式
    inline static time_t make_time(const tm& time, tm* ref = 0, pointer timeStr = 0) {
        return make_time(&time, ref, timeStr);
    }

    // 参数2为空时需要调用 本对象的 free() 释放, 参数2至少20个字符
    // 如果参数2为空, 则内部申请空间存储, 调用放需要调用本对象的 free() 进行释放
    // 如果参数2不为空, 则参数2最少需要20个字符的尺寸, 返回值是返回这个地址
    inline static time_t make_time(const FILETIME* time, tm* ref = 0, pointer timeStr = 0) {
        if (!time) return 0;
        SYSTEMTIME st = { 0 };
        FileTimeToSystemTime(time, &st);
        return make_time(st.wYear, st.wMonth, st.wDay, st.wMonth, st.wMinute, st.wSecond, ref, timeStr);
    }

    // 参数2为空时需要调用 本对象的 free() 释放, 参数2至少20个字符
    // 如果参数2为空, 则内部申请空间存储, 调用放需要调用本对象的 free() 进行释放
    // 如果参数2不为空, 则参数2最少需要20个字符的尺寸, 返回值是返回这个地址
    inline static time_t make_time(const FILETIME& time, tm* ref = 0, pointer timeStr = 0) {
        return make_time(&time, ref, timeStr);
    }

    // 把年月日时分秒转换成 time_t 类型, 返回这个时间从1970年经过的秒数
    // year, month, day, hour, minute, second = 年月日时分秒
    // ref = 是否接收这个时间对应的tm结构, 为0则不接收
    // timeStr = 是否转换成文本, 如果不为0, 最少需要20个字符的缓冲区, 则把时间转换成 2021-01-01 11:22:33 这种格式
    inline static time_t make_time(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, tm* ref = 0, pointer timeStr = 0) {
        if (year <= 0) return 0;
        tm _tm = { 0 };
        _tm.tm_year = year - 1900;          // 年
        if (month   >= 0) _tm.tm_mon = month - 1;       // 月
        if (day     >= 0) _tm.tm_mday = day;            // 日
        if (hour    >= 0) _tm.tm_hour = hour;           // 时
        if (minute  >= 0) _tm.tm_min = minute;          // 分
        if (second  >= 0) _tm.tm_sec = second;          // 秒

        if (_tm.tm_mday == 0) _tm.tm_mday = 1;
        time_t _t = mktime(&_tm);
        if (ref) make_tm(_t, ref);
        if (timeStr) to_string(&_tm, timeStr);
        return _t;
    }


    // make_tm, 转换成 tm结构, c里面时间相关的函数大多都是使用这个结构
public:
    // 从time_t里把时间转换成 tm 结构
    // _time = 1900年到现在经过的秒数
    // _tm = _time 转换的tm结构, 为0则调用失败
    // timeStr = 是否转换成文本, 如果不为0, 最少需要20个字符的缓冲区, 则把时间转换成 2021-01-01 11:22:33 这种格式
    inline static void make_tm(time_t _time, tm* _tm, pointer timeStr = 0) {
        if (!_tm)return;
        memset(_tm, 0, sizeof(tm));
#if _MSC_VER > 1200
        localtime_s(_tm, &_time);   // 将time_t格式转换为tm结构体
#else
        tm* tmp = localtime(&_time);    // 将time_t格式转换为tm结构体
        if (tmp)  memcpy(_tm, tmp, sizeof(tm));
#endif
        if (timeStr) to_string(_tm, timeStr);
    }


    // 获取时间, 获取指定的时间部分, 年月日时分秒周几
public:

    // 取指定时间, 参数分别是 年月日时分秒周几, 在一年中的第几天
    inline static bool Getdatetime(time_t time, int* year = 0, int* month = 0, int* day = 0, int* hour = 0, int* minute = 0, int* second = 0, int* wday = 0, int* yday = 0) {
        if (time <= 0)return false;
        tm _tm;;
        make_tm(time, &_tm);
        if (year)   *year   = _tm.tm_year + 1900;   // 年, 由于tm结构体存储的是从1900年开始的时间, 所以临时变量int为tm_year加上1900
        if (month)  *month  = _tm.tm_mon + 1;       // 月, 由于tm结构体的月份存储范围为0-11, 所以临时变量int为tm_mon加上1
        if (day)    *day    = _tm.tm_mday;
        if (hour)   *hour   = _tm.tm_hour;
        if (minute) *minute = _tm.tm_min;
        if (second) *second = _tm.tm_sec;
        if (wday)   *wday   = _tm.tm_wday;
        if (yday)   *yday   = _tm.tm_yday;
        return true;
    }
    // 年 
    inline static int year(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_year + 1900;
    }   
     // 月 1-12
    inline static int month(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_mon + 1;
    }
     // 日 1-31
    inline static int day(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_mday;
    }
    // 一年里的第几天, 1-365
    inline static int yday(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_yday;
    }
    // 周几 1-7
    inline static int wday(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_wday + 1;
    }
    // 小时 0-23
    inline static int hour(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_hour;
    }
    // 分钟 0-59
    inline static int minute(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_min;
    }
    // 秒 0-60
    inline static int second(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_sec;
    }

public:

    //比较两个时间, 相等返回0, 时间1大于时间2返回1, 时间1小于时间2返回-1
    inline static int compare(time_t time1, time_t time2) {
        return time1 > time2 ? 1 : (time1 == time2 ? 0 : -1);
    }

    //比较两个时间, 相等返回0, 时间1大于时间2返回1, 时间1小于时间2返回-1
    template<typename T>
    inline static int compare(time_t time1, const T& time2) {
        return compare(time1, make_time(time2));
    }

    //比较两个时间, 相等返回0, 时间1大于时间2返回1, 时间1小于时间2返回-1
    template<typename T, typename R>
    inline static int compare(T time1, const R& time2) {
        return compare(make_time(time1), make_time(time2));
    }
private:

    // 取文本长度, 不包含结束标志
    inline static size_t __lstrlen(LPCSTR s) { return s ? strlen(s) : 0; }
    inline static size_t __lstrlen(LPCWSTR s) { return s ? wcslen(s) : 0; }
    inline static int __vsprintf(char* const _Buffer, const size_t _bufferSize, const char* const _Format, va_list _ArgList) { return ___vsnprintf_s(_Buffer, _bufferSize, _Format, _ArgList); }
    inline static int __vsprintf(wchar_t* const _Buffer, const size_t _bufferSize, const wchar_t* _Format, va_list _ArgList) { return ___vsnwprintf_s(_Buffer, _bufferSize, _Format, _ArgList); }


    inline static int ___vsnprintf_s(char* const _Buffer, size_t const _BufferCount, char const* const _Format, va_list _ArgList)
    {
#if _MSC_VER > 1200
        return _vsnprintf_s(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#else
        typedef int(__cdecl* fn_vsnprintf_s)(char* const _Buffer, size_t const _BufferCount, size_t const _MaxCount, char const* const _Format, va_list _ArgList);
        static fn_vsnprintf_s fnFun;
        if (fnFun)
            return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
        HMODULE hModule = GetModuleHandleA("msvcrt.dll");
        if (!hModule)
        {
            hModule = LoadLibraryW(L"msvcrt.dll");
            if (!hModule)
                return vsprintf(_Buffer, _Format, _ArgList);
        }
        fnFun = (fn_vsnprintf_s)GetProcAddress(hModule, "_vsnprintf_s");
        if (!fnFun)
            return vsprintf(_Buffer, _Format, _ArgList);
        return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#endif
    }

    inline static int ___vsnwprintf_s(wchar_t* const _Buffer, size_t const _BufferCount, wchar_t const* const _Format, va_list _ArgList)
    {
#if _MSC_VER > 1200
        return _vsnwprintf_s(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#else
        typedef int(__cdecl* fn_vsnwprintf_s)(wchar_t* const _Buffer, size_t const _BufferCount, size_t const _MaxCount, wchar_t const* const _Format, va_list _ArgList);
        static fn_vsnwprintf_s fnFun;
        if (fnFun)
            return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
        HMODULE hModule = GetModuleHandleA("msvcrt.dll");
        if (!hModule)
        {
            hModule = LoadLibraryW(L"msvcrt.dll");
            if (!hModule)
                return vswprintf(_Buffer, _Format, _ArgList);
        }

        fnFun = (fn_vsnwprintf_s)GetProcAddress(hModule, "_vsnwprintf_s");
        if (!fnFun)
            return vswprintf(_Buffer, _Format, _ArgList);
        return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#endif
    }


    inline void update_data(time_t _t) {
        m_time = _t;
        make_tm(m_time, &m_tm);
        to_string(&m_tm, m_str);
    }
    inline void update_data(tm& _t) {
        m_tm = _t;
        make_time(&m_tm, 0, m_str);
    }
};

static inline ULONG64 get_time()
{
    ULONG64 _t = 0;
    GetSystemTimeAsFileTime((LPFILETIME)&_t);
    _t -= 0x019db1ded53e8000;
    _t /= 10000;
    return _t;
}

#if _DATETIME_DEBUG_
    typedef datetime_base datetimeA;
    typedef datetime_base datetimeW;
    typedef datetime_base datetime;
#else
    typedef datetime_base<char> datetimeA;
    typedef datetime_base<wchar_t> datetimeW;
    typedef datetime_base<TCHAR> datetime;
#endif