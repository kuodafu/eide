#pragma once
#include <time.h>

enum DATE_TYPE
{
    DATE_TYPE_SEC,      // ��
    DATE_TYPE_MIN,      // ����
    DATE_TYPE_HOUR,     // Сʱ
    DATE_TYPE_DAY,      // ��
    DATE_TYPE_MON,      // ��
    DATE_TYPE_YEAR,     // ��
};

#define _DATETIME_DEBUG_    0       // ��Ϊ1�Ļ��Ͳ�ʹ��ģ��, �������

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

    time_t  m_time; // ��ǰ�����¼��ʱ��, ��1970�꾭��������
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


    // ��յ�ǰ���ʱ��
    inline datetime_base& operator()() {
        m_time = 0;
        m_tm = { 0 };
        if (m_str) m_str[0] = 0;
        return *this;
    }
    // ��ʱ���ı��︳ֵ
    inline datetime_base& operator()(LPCWSTR time) {
        m_time = make_time(time, &m_tm, m_str);
        return *this;
    }
    // ��ʱ���ı��︳ֵ
    inline datetime_base& operator()(LPCSTR time) {
        m_time = make_time(time, &m_tm, m_str);
        return *this;
    }
    // ��ָ��ʱ���︳ֵ
    inline datetime_base& operator()(time_t time) {
        m_time = time;
        make_tm(m_time, &m_tm, m_str);
        return *this;
    }
    // ��ָ��������ʱ�����︳ֵ
    inline datetime_base& operator()(int year, int month, int day, int hour = 0, int minute = 0, int second = 0) {
        m_time = make_time(year, month, day, hour, minute, second, &m_tm, m_str);
        return *this;
    }
    // ��ϵͳʱ��ṹ�︳ֵ
    inline datetime_base& operator()(const LPSYSTEMTIME time) {
        if (!time) return;
        m_time = make_time(time->wYear, time->wMonth, time->wDay, time->wHour, time->wMinute, time->wSecond, &m_tm, m_str);
        return *this;
    }


    // �ж�����ʱ���Ƿ����
    inline bool operator==(time_t               time) const { return m_time == time; }
    inline bool operator==(LPCWSTR              time) const { return m_time == make_time(time); }
    inline bool operator==(LPCSTR               time) const { return m_time == make_time(time); }
    inline bool operator==(const tm&            time) const { return m_time == make_time(time); }
    inline bool operator==(const tm*            time) const { return m_time == make_time(time); }
    inline bool operator==(const SYSTEMTIME&    time) const { return m_time == make_time(time); }
    inline bool operator==(const SYSTEMTIME*    time) const { return m_time == make_time(time); }

    // �жϵ�ǰ�����ʱ���Ƿ���ڴ��ݽ�����ʱ��
    inline bool operator>(time_t                time) const { return m_time > time; }
    inline bool operator>(LPCWSTR               time) const { return m_time > make_time(time); }
    inline bool operator>(LPCSTR                time) const { return m_time > make_time(time); }
    inline bool operator>(const tm&             time) const { return m_time > make_time(time); }
    inline bool operator>(const tm*             time) const { return m_time > make_time(time); }
    inline bool operator>(const SYSTEMTIME&     time) const { return m_time > make_time(time); }
    inline bool operator>(const SYSTEMTIME*     time) const { return m_time > make_time(time); }
                                                      
    // �жϵ�ǰ�����ʱ���Ƿ�С�ڴ��ݽ�����ʱ��              
    inline bool operator<(time_t                time) const { return m_time < time; }
    inline bool operator<(LPCWSTR               time) const { return m_time < make_time(time); }
    inline bool operator<(LPCSTR                time) const { return m_time < make_time(time); }
    inline bool operator<(const tm&             time) const { return m_time < make_time(time); }
    inline bool operator<(const tm*             time) const { return m_time < make_time(time); }
    inline bool operator<(const SYSTEMTIME&     time) const { return m_time < make_time(time); }
    inline bool operator<(const SYSTEMTIME*     time) const { return m_time < make_time(time); }

    inline time_t time() { return m_time; }
public:
    // ȡָ��ʱ��, �����ֱ��� ������ʱ�����ܼ�, ��һ���еĵڼ���
    inline bool Getdatetime(int* year = 0, int* month = 0, int* day = 0, int* hour = 0, int* minute = 0, int* second = 0, int* wday = 0, int* yday = 0) const {
        if (m_time <= 0)return false;
        const tm* _tm = &m_tm;
        if (year    ) *year   = _tm->tm_year + 1900;  // ��, ����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ʱ��, ������ʱ����intΪtm_year����1900
        if (month   ) *month  = _tm->tm_mon + 1;      // ��, ����tm�ṹ����·ݴ洢��ΧΪ0-11, ������ʱ����intΪtm_mon����1
        if (day     ) *day    = _tm->tm_mday;
        if (hour    ) *hour   = _tm->tm_hour;
        if (minute  ) *minute = _tm->tm_min;
        if (second  ) *second = _tm->tm_sec;
        if (wday    ) *wday   = _tm->tm_wday;
        if (yday    ) *yday   = _tm->tm_yday;
        return true;
    }

    inline int year()   const { return m_time ? m_tm.tm_year + 1900 : 0; }   // �� 
    inline int month()  const { return m_time ? m_tm.tm_mon + 1 : 0; }       // �� 1-12
    inline int day()    const { return m_time ? m_tm.tm_mday : 0; }          // �� 1-31
    inline int yday()   const { return m_time ? m_tm.tm_yday : 0; }          // һ����ĵڼ���, 1-365
    inline int wday()   const { return m_time ? m_tm.tm_wday + 1 : 0; }      // �ܼ� 1-7
    inline int hour()   const { return m_time ? m_tm.tm_hour : 0; }          // Сʱ 0-23
    inline int minute() const { return m_time ? m_tm.tm_min : 0; }           // ���� 0-59
    inline int second() const { return m_time ? m_tm.tm_sec : 0; }           // �� 0-60

public:
    // ���ʱ��
    inline time_t add(int _off, DATE_TYPE type = DATE_TYPE_SEC) {
        m_time = add(m_time, _off, type);
        update_data(m_time);
        return m_time;
    }
    // ����ʱ��
    inline time_t sub(int _off, DATE_TYPE type = DATE_TYPE_SEC) {
        m_time = sub(m_time, _off, type);
        update_data(m_time);
        return m_time;
    }

    // ���ʱ��
    inline static time_t add(time_t time, int _off, DATE_TYPE type = DATE_TYPE_SEC) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        switch (type)
        {
        case DATE_TYPE_SEC   : _tm.tm_sec  += _off; break; // ��
        case DATE_TYPE_MIN   : _tm.tm_min  += _off; break; // ����
        case DATE_TYPE_HOUR  : _tm.tm_hour += _off; break; // Сʱ
        case DATE_TYPE_DAY   : _tm.tm_mday += _off; break; // ��
        case DATE_TYPE_MON   : _tm.tm_mon  += _off; break; // ��
        case DATE_TYPE_YEAR  : _tm.tm_year += _off; break; // ��
        default:
            return time;
        }
        return make_time(&_tm);
    }

    // ����ʱ��
    inline static time_t sub(time_t time, int _off, DATE_TYPE type = DATE_TYPE_SEC) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        switch (type)
        {
        case DATE_TYPE_SEC   : _tm.tm_sec  -= _off; break; // ��
        case DATE_TYPE_MIN   : _tm.tm_min  -= _off; break; // ����
        case DATE_TYPE_HOUR  : _tm.tm_hour -= _off; break; // Сʱ
        case DATE_TYPE_DAY   : _tm.tm_mday -= _off; break; // ��
        case DATE_TYPE_MON   : _tm.tm_mon  -= _off; break; // ��
        case DATE_TYPE_YEAR  : _tm.tm_year -= _off; break; // ��
        default:
            return time;
        }
        return make_time(&_tm);
    }

public:

    // �ͷ� to_string ������ڴ�
    inline static void free(pointer buffer) {
        if (buffer) ::free(buffer);
    }

    // �ѵ�ǰʱ��ת�����ı�, �������1Ϊ��, ���ڲ�����ռ�洢, ���÷���Ҫ���ñ������ free() �����ͷ�
    // �������1��Ϊ��, �����1������Ҫ20���ַ��ĳߴ�, ����ֵ�Ƿ��������ַ
    // ����1Ϊ��ʱ��Ҫ���� ������� free() �ͷ�, ����1����20���ַ�
    inline static pointer to_string(pointer buffer = 0) {
        return to_string(::time(0), buffer);
    }

    // ����2Ϊ��ʱ��Ҫ���� ������� free() �ͷ�, ����2����20���ַ�
    // �������2Ϊ��, ���ڲ�����ռ�洢, ���÷���Ҫ���ñ������ free() �����ͷ�
    // �������2��Ϊ��, �����2������Ҫ20���ַ��ĳߴ�, ����ֵ�Ƿ��������ַ
    inline static pointer to_string(time_t time, pointer buffer = 0) {
        tm _tm;
        make_tm(time, &_tm);
        return to_string(&_tm, buffer);
        // 2020-20-20 20-20-20
    }

    // ����2Ϊ��ʱ��Ҫ���� ������� free() �ͷ�, ����2����20���ַ�
    // �������2Ϊ��, ���ڲ�����ռ�洢, ���÷���Ҫ���ñ������ free() �����ͷ�
    // �������2��Ϊ��, �����2������Ҫ20���ַ��ĳߴ�, ����ֵ�Ƿ��������ַ
    inline static pointer to_string(const tm& _tm, pointer buffer = 0) {
        return to_string(&_tm, buffer);
    }
    // ����2Ϊ��ʱ��Ҫ���� ������� free() �ͷ�, ����2����20���ַ�
    // �������2Ϊ��, ���ڲ�����ռ�洢, ���÷���Ҫ���ñ������ free() �����ͷ�
    // �������2��Ϊ��, �����2������Ҫ20���ַ��ĳߴ�, ����ֵ�Ƿ��������ַ
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


    // ����2Ϊ��ʱ��Ҫ���� ������� free() �ͷ�, ����2����20���ַ�
    // �������2Ϊ��, ���ڲ�����ռ�洢, ���÷���Ҫ���ñ������ free() �����ͷ�
    // �������2��Ϊ��, �����2������Ҫ20���ַ��ĳߴ�, ����ֵ�Ƿ��������ַ
    inline static pointer to_string(const SYSTEMTIME* time, pointer buffer = 0) {
        if (!time) return 0;
        make_time(time->wYear, time->wMonth, time->wDay, time->wHour, time->wMinute, time->wSecond, 0, buffer);
        return buffer;
    }

    // ����2Ϊ��ʱ��Ҫ���� ������� free() �ͷ�, ����2����20���ַ�
    // �������2Ϊ��, ���ڲ�����ռ�洢, ���÷���Ҫ���ñ������ free() �����ͷ�
    // �������2��Ϊ��, �����2������Ҫ20���ַ��ĳߴ�, ����ֵ�Ƿ��������ַ
    inline static pointer to_string(const SYSTEMTIME& time, pointer buffer = 0) {
        return to_string(&time, buffer);
    }

    // ����2Ϊ��ʱ��Ҫ���� ������� free() �ͷ�, ����2����20���ַ�
    // �������2Ϊ��, ���ڲ�����ռ�洢, ���÷���Ҫ���ñ������ free() �����ͷ�
    // �������2��Ϊ��, �����2������Ҫ20���ַ��ĳߴ�, ����ֵ�Ƿ��������ַ
    inline static pointer to_string(const FILETIME* time, pointer buffer = 0) {
        if (!time) return 0;
        SYSTEMTIME st = { 0 };
        FileTimeToSystemTime(time, &st);
        return to_string(&st, buffer);
    }

    // ����2Ϊ��ʱ��Ҫ���� ������� free() �ͷ�, ����2����20���ַ�
    // �������2Ϊ��, ���ڲ�����ռ�洢, ���÷���Ҫ���ñ������ free() �����ͷ�
    // �������2��Ϊ��, �����2������Ҫ20���ַ��ĳߴ�, ����ֵ�Ƿ��������ַ
    inline static pointer to_string(const FILETIME& time, pointer buffer = 0) {
        return to_string(&time, buffer);
    }


    // make_time ת���� time_t, ��1970�굽���ڵ�����
public:

    // ���ı�ת���� time_t ����, �������ʱ���1970�꾭��������
    // time = ʱ���ı�, ��ʽ�� 2021-01-01 11:22:33
    // ref = �Ƿ�������ʱ���Ӧ��tm�ṹ, Ϊ0�򲻽���
    // timeStr = �Ƿ�ת�����ı�, �����Ϊ0, ������Ҫ20���ַ��Ļ�����, ���ʱ��ת���� 2021-01-01 11:22:33 ���ָ�ʽ
    inline static time_t make_time(LPCWSTR time, tm* ref = 0, pointer timeStr = 0) {
        int year = -1, month = -1, day = -1, hour = -1, minute = -1, second = -1;
#if _MSC_VER < 1201
        int l = swscanf(time, L"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
#else
        int l = swscanf_s(time, L"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
#endif
        return make_time(year, month, day, hour, minute, second, ref, timeStr);
    }
    // ���ı�ת���� time_t ����, �������ʱ���1970�꾭��������
    // time = ʱ���ı�, ��ʽ�� 2021-01-01 11:22:33
    // ref = �Ƿ�������ʱ���Ӧ��tm�ṹ, Ϊ0�򲻽���
    // timeStr = �Ƿ�ת�����ı�, �����Ϊ0, ������Ҫ20���ַ��Ļ�����, ���ʱ��ת���� 2021-01-01 11:22:33 ���ָ�ʽ
    inline static time_t make_time(LPCSTR time, tm* ref = 0, pointer timeStr = 0) {
        int year = -1, month = -1, day = -1, hour = -1, minute = -1, second = -1;
#if _MSC_VER < 1201
        sscanf(time, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
#else
        sscanf_s(time, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
#endif

        return make_time(year, month, day, hour, minute, second, ref, timeStr);
    }

    // ��SYSTEMTIME�ṹת���� time_t ����, �������ʱ���1970�꾭��������
    // time = SYSTEMTIME �ṹ
    // ref = �Ƿ�������ʱ���Ӧ��tm�ṹ, Ϊ0�򲻽���
    // timeStr = �Ƿ�ת�����ı�, �����Ϊ0, ������Ҫ20���ַ��Ļ�����, ���ʱ��ת���� 2021-01-01 11:22:33 ���ָ�ʽ
    inline static time_t make_time(const SYSTEMTIME& time, tm* ref = 0, pointer timeStr = 0) {
        return make_time(&time, ref, timeStr);
    }
    // ��SYSTEMTIME�ṹת���� time_t ����, �������ʱ���1970�꾭��������
    // time = SYSTEMTIME �ṹָ��
    // ref = �Ƿ�������ʱ���Ӧ��tm�ṹ, Ϊ0�򲻽���
    // timeStr = �Ƿ�ת�����ı�, �����Ϊ0, ������Ҫ20���ַ��Ļ�����, ���ʱ��ת���� 2021-01-01 11:22:33 ���ָ�ʽ
    inline static time_t make_time(const SYSTEMTIME* time, tm* ref = 0, pointer timeStr = 0) {
        if (!time)return 0;
        return make_time(time->wYear, time->wMonth, time->wDay, time->wHour, time->wMinute, time->wSecond, ref, timeStr);
    }
    // ��tm�ṹת���� time_t ����, �������ʱ���1970�꾭��������
    // time = tm �ṹָ��
    // ref = �Ƿ�������ʱ���Ӧ��tm�ṹ, Ϊ0�򲻽���
    // timeStr = �Ƿ�ת�����ı�, �����Ϊ0, ������Ҫ20���ַ��Ļ�����, ���ʱ��ת���� 2021-01-01 11:22:33 ���ָ�ʽ
    inline static time_t make_time(const tm* time, tm* ref = 0, pointer timeStr = 0) {
        if (!time)return 0;
        return make_time(time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec, ref, timeStr);

    }
    // ��tm�ṹת���� time_t ����, �������ʱ���1970�꾭��������
    // time = tm �ṹ
    // ref = �Ƿ�������ʱ���Ӧ��tm�ṹ, Ϊ0�򲻽���
    // timeStr = �Ƿ�ת�����ı�, �����Ϊ0, ������Ҫ20���ַ��Ļ�����, ���ʱ��ת���� 2021-01-01 11:22:33 ���ָ�ʽ
    inline static time_t make_time(const tm& time, tm* ref = 0, pointer timeStr = 0) {
        return make_time(&time, ref, timeStr);
    }

    // ����2Ϊ��ʱ��Ҫ���� ������� free() �ͷ�, ����2����20���ַ�
    // �������2Ϊ��, ���ڲ�����ռ�洢, ���÷���Ҫ���ñ������ free() �����ͷ�
    // �������2��Ϊ��, �����2������Ҫ20���ַ��ĳߴ�, ����ֵ�Ƿ��������ַ
    inline static time_t make_time(const FILETIME* time, tm* ref = 0, pointer timeStr = 0) {
        if (!time) return 0;
        SYSTEMTIME st = { 0 };
        FileTimeToSystemTime(time, &st);
        return make_time(st.wYear, st.wMonth, st.wDay, st.wMonth, st.wMinute, st.wSecond, ref, timeStr);
    }

    // ����2Ϊ��ʱ��Ҫ���� ������� free() �ͷ�, ����2����20���ַ�
    // �������2Ϊ��, ���ڲ�����ռ�洢, ���÷���Ҫ���ñ������ free() �����ͷ�
    // �������2��Ϊ��, �����2������Ҫ20���ַ��ĳߴ�, ����ֵ�Ƿ��������ַ
    inline static time_t make_time(const FILETIME& time, tm* ref = 0, pointer timeStr = 0) {
        return make_time(&time, ref, timeStr);
    }

    // ��������ʱ����ת���� time_t ����, �������ʱ���1970�꾭��������
    // year, month, day, hour, minute, second = ������ʱ����
    // ref = �Ƿ�������ʱ���Ӧ��tm�ṹ, Ϊ0�򲻽���
    // timeStr = �Ƿ�ת�����ı�, �����Ϊ0, ������Ҫ20���ַ��Ļ�����, ���ʱ��ת���� 2021-01-01 11:22:33 ���ָ�ʽ
    inline static time_t make_time(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, tm* ref = 0, pointer timeStr = 0) {
        if (year <= 0) return 0;
        tm _tm = { 0 };
        _tm.tm_year = year - 1900;          // ��
        if (month   >= 0) _tm.tm_mon = month - 1;       // ��
        if (day     >= 0) _tm.tm_mday = day;            // ��
        if (hour    >= 0) _tm.tm_hour = hour;           // ʱ
        if (minute  >= 0) _tm.tm_min = minute;          // ��
        if (second  >= 0) _tm.tm_sec = second;          // ��

        if (_tm.tm_mday == 0) _tm.tm_mday = 1;
        time_t _t = mktime(&_tm);
        if (ref) make_tm(_t, ref);
        if (timeStr) to_string(&_tm, timeStr);
        return _t;
    }


    // make_tm, ת���� tm�ṹ, c����ʱ����صĺ�����඼��ʹ������ṹ
public:
    // ��time_t���ʱ��ת���� tm �ṹ
    // _time = 1900�굽���ھ���������
    // _tm = _time ת����tm�ṹ, Ϊ0�����ʧ��
    // timeStr = �Ƿ�ת�����ı�, �����Ϊ0, ������Ҫ20���ַ��Ļ�����, ���ʱ��ת���� 2021-01-01 11:22:33 ���ָ�ʽ
    inline static void make_tm(time_t _time, tm* _tm, pointer timeStr = 0) {
        if (!_tm)return;
        memset(_tm, 0, sizeof(tm));
#if _MSC_VER > 1200
        localtime_s(_tm, &_time);   // ��time_t��ʽת��Ϊtm�ṹ��
#else
        tm* tmp = localtime(&_time);    // ��time_t��ʽת��Ϊtm�ṹ��
        if (tmp)  memcpy(_tm, tmp, sizeof(tm));
#endif
        if (timeStr) to_string(_tm, timeStr);
    }


    // ��ȡʱ��, ��ȡָ����ʱ�䲿��, ������ʱ�����ܼ�
public:

    // ȡָ��ʱ��, �����ֱ��� ������ʱ�����ܼ�, ��һ���еĵڼ���
    inline static bool Getdatetime(time_t time, int* year = 0, int* month = 0, int* day = 0, int* hour = 0, int* minute = 0, int* second = 0, int* wday = 0, int* yday = 0) {
        if (time <= 0)return false;
        tm _tm;;
        make_tm(time, &_tm);
        if (year)   *year   = _tm.tm_year + 1900;   // ��, ����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ʱ��, ������ʱ����intΪtm_year����1900
        if (month)  *month  = _tm.tm_mon + 1;       // ��, ����tm�ṹ����·ݴ洢��ΧΪ0-11, ������ʱ����intΪtm_mon����1
        if (day)    *day    = _tm.tm_mday;
        if (hour)   *hour   = _tm.tm_hour;
        if (minute) *minute = _tm.tm_min;
        if (second) *second = _tm.tm_sec;
        if (wday)   *wday   = _tm.tm_wday;
        if (yday)   *yday   = _tm.tm_yday;
        return true;
    }
    // �� 
    inline static int year(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_year + 1900;
    }   
     // �� 1-12
    inline static int month(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_mon + 1;
    }
     // �� 1-31
    inline static int day(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_mday;
    }
    // һ����ĵڼ���, 1-365
    inline static int yday(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_yday;
    }
    // �ܼ� 1-7
    inline static int wday(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_wday + 1;
    }
    // Сʱ 0-23
    inline static int hour(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_hour;
    }
    // ���� 0-59
    inline static int minute(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_min;
    }
    // �� 0-60
    inline static int second(time_t time) {
        tm _tm = { 0 };
        make_tm(time, &_tm);
        return _tm.tm_sec;
    }

public:

    //�Ƚ�����ʱ��, ��ȷ���0, ʱ��1����ʱ��2����1, ʱ��1С��ʱ��2����-1
    inline static int compare(time_t time1, time_t time2) {
        return time1 > time2 ? 1 : (time1 == time2 ? 0 : -1);
    }

    //�Ƚ�����ʱ��, ��ȷ���0, ʱ��1����ʱ��2����1, ʱ��1С��ʱ��2����-1
    template<typename T>
    inline static int compare(time_t time1, const T& time2) {
        return compare(time1, make_time(time2));
    }

    //�Ƚ�����ʱ��, ��ȷ���0, ʱ��1����ʱ��2����1, ʱ��1С��ʱ��2����-1
    template<typename T, typename R>
    inline static int compare(T time1, const R& time2) {
        return compare(make_time(time1), make_time(time2));
    }
private:

    // ȡ�ı�����, ������������־
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