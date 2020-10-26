#ifndef RDEBUG_H
#define RDEBUG_H

#include <string>

namespace Redopera {

class RColor;
class RPoint2;
class RPoint3;
class RSize;
class RRect;
class RTime;

class RDebug
{
public:
    static RDebug create() { return {}; }
    #define rDebug Redopera::RDebug::create()

    ~RDebug();

    RDebug(const RDebug&) = delete;
    RDebug& operator=(const RDebug&) = delete;

    const RDebug& operator<<(int value) const;
    const RDebug& operator<<(long value) const;
    const RDebug& operator<<(long long value) const;
    const RDebug& operator<<(unsigned value) const;
    const RDebug& operator<<(unsigned long value) const;
    const RDebug& operator<<(unsigned long long value) const;
    const RDebug& operator<<(double value) const;
    const RDebug& operator<<(float value) const;
    const RDebug& operator<<(bool b) const;

    const RDebug& operator<<(unsigned char c) const;

    const RDebug& operator<<(char c) const;
    const RDebug& operator<<(const char *str) const;
    const RDebug& operator<<(char *str) const;
    const RDebug& operator<<(const std::string &str) const;

    const RDebug& operator<<(wchar_t c) const;
    const RDebug& operator<<(const wchar_t *str) const;
    const RDebug& operator<<(wchar_t *str) const;
    const RDebug& operator<<(const std::wstring &str) const;

    const RDebug& operator<<(void *ptr) const;

    const RDebug& operator<<(const RColor &color) const;
    const RDebug& operator<<(const RPoint2 &pos) const;
    const RDebug& operator<<(const RPoint3 &pos) const;
    const RDebug& operator<<(const RSize &size) const;
    const RDebug& operator<<(const RRect &rect) const;
    const RDebug& operator<<(const RTime &time) const;

private:
    constexpr static unsigned DATA_SIZE = 512;

    RDebug();

    mutable std::string buf_;
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
