#ifndef RDEBUG_H
#define RDEBUG_H

#include <string>

namespace Redopera {

class RColor;
class RPoint2;
class RPoint3;
class RSize;
class RRect;
class RArea;

class RDebug
{
public:
    static RDebug create() { return {}; }
    #define rDebug Redopera::RDebug::create()

    ~RDebug();

    RDebug(const RDebug&) = delete;
    RDebug& operator=(const RDebug&) = delete;

    RDebug& operator<<(int value);
    RDebug& operator<<(long value);
    RDebug& operator<<(long long value);
    RDebug& operator<<(unsigned value);
    RDebug& operator<<(unsigned long value);
    RDebug& operator<<(unsigned long long value);
    RDebug& operator<<(double value);
    RDebug& operator<<(float value);
    RDebug& operator<<(bool b);

    RDebug& operator<<(unsigned char c);

    RDebug& operator<<(char c);
    RDebug& operator<<(const char *str);
    RDebug& operator<<(char *str);
    RDebug& operator<<(const std::string &str);

    RDebug& operator<<(wchar_t c);
    RDebug& operator<<(const wchar_t *str);
    RDebug& operator<<(wchar_t *str);
    RDebug& operator<<(const std::wstring &str);

    RDebug& operator<<(const RColor &color);
    RDebug& operator<<(const RPoint2 &pos);
    RDebug& operator<<(const RPoint3 &pos);
    RDebug& operator<<(const RSize &size);
    RDebug& operator<<(const RRect &rect);
    RDebug& operator<<(const RArea &area);

    template<typename T>
    RDebug& operator<<(T *ptr)
    {
        char str[64];
        int i = std::snprintf(str, sizeof(str), "(add: %p) ", ptr);
        str[i] = '\0';
        buf_ += str;
        return *this;
    }

    template<typename T>
    RDebug& operator<<(T input)
    {
        buf_ += toStdString(std::forward<T>(input));
        return *this;
    }

private:
    constexpr static unsigned DATA_SIZE = 512;

    RDebug();

    std::string buf_;
};

void prError(const std::string &err);
bool check(bool b, const std::string &err);

namespace EscCtl {

static constexpr char non[] = "\033[0m"; //关闭所有属性
static constexpr char bold[] = "\033[1m"; //高亮

static constexpr char black[] = "\033[30m"; //黑
static constexpr char red[] = "\033[31m"; //红
static constexpr char green[] = "\033[32m"; //绿
static constexpr char yellow[] = "\033[33m"; //黄
static constexpr char blue[] = "\033[34m"; //蓝色
static constexpr char purple[] = "\033[35m"; //紫色
static constexpr char cyan[] = "\033[36m"; //深绿
static constexpr char white[] = "\033[37m"; //白色

static constexpr char bblack[] = "\033[40m"; //黑色背景 //设置背景色
static constexpr char bred[] = "\033[41m"; //红色背景
static constexpr char bgreen[] = "\033[42m"; //绿色背景
static constexpr char bbrown[] = "\033[43m"; //棕色背景
static constexpr char bblue[] = "\033[44m"; //蓝色背景
static constexpr char bmagente[] = "\033[45m"; //品红背景
static constexpr char bcyan[] = "\033[46m"; //孔雀蓝背景
static constexpr char bwhite[] = "\033[47m"; //白色背景

}; // ns prcolor

} // ns Redopera

#endif // RDEBUG_H
