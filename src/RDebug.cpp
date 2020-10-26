#include <RDebug.h>
#include <RColor.h>
#include <RPoint.h>
#include <RSize.h>
#include <RRect.h>
#include <RTime.h>

#include <locale>
#include <codecvt>

using namespace Redopera;

static std::wstring_convert<std::codecvt_utf8<wchar_t>> strcnv;

RDebug::~RDebug()
{
    std::printf("%s\n", buf_.c_str());
    fflush(stdout);
}

const RDebug &RDebug::operator<<(int value) const
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(long value) const
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(long long value) const
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(unsigned value) const
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(unsigned long value) const
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(unsigned long long value) const
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(double value) const
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(float value) const
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(unsigned char c) const
{
    buf_ += std::to_string(c*1) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(bool b) const
{
    buf_ += b ? "true " : "false ";
    return *this;
}

const RDebug &RDebug::operator<<(char c) const
{
    buf_ = buf_ + c + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(const char *str) const
{
    buf_ += str;
    return *this;
}

const RDebug &RDebug::operator<<(char *str) const
{
    buf_ += str;
    return *this;
}

const RDebug &RDebug::operator<<(const std::string &str) const
{
    buf_ += str;
    return *this;
}

const RDebug &RDebug::operator<<(wchar_t c) const
{
    buf_ += strcnv.to_bytes(c) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(const wchar_t *str) const
{
    buf_ += strcnv.to_bytes(str);
    return *this;
}

const RDebug &RDebug::operator<<(wchar_t *str) const
{
    buf_ += strcnv.to_bytes(str);
    return *this;
}

const RDebug &RDebug::operator<<(const std::wstring &str) const
{
    buf_ += strcnv.to_bytes(str);
    return *this;
}

const RDebug &RDebug::operator<<(void *ptr) const
{
    char str[64];
    int i = std::snprintf(str, sizeof(str), "(add: %p) ", ptr);
    str[i] = '\0';
    buf_ += str;
    return *this;
}

const RDebug &RDebug::operator<<(const RColor &color) const
{
    buf_ += "(r:" + std::to_string(color.r()) + " g:" + std::to_string(color.g()) + " b:" + std::to_string(color.b()) + ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const RPoint2 &pos) const
{
    buf_ += '(' + std::to_string(pos.x()) + ", " + std::to_string(pos.y()) + ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const RPoint3 &pos) const
{
    buf_ += '(' + std::to_string(pos.x()) + ", " + std::to_string(pos.y()) + ", " + std::to_string(pos.z()) + ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const RSize &size) const
{
    buf_ += "(w:" + std::to_string(size.width()) + " h:" + std::to_string(size.height()) + ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const RRect &rect) const
{
    buf_ += "(" + std::to_string(rect.left()) + ", " + std::to_string(rect.bottom())
            + " | w:" + std::to_string(rect.width()) + " h:" + std::to_string(rect.height()) + ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const RTime &time) const
{
    buf_ += time.toStdString();
    return *this;
}

RDebug::RDebug()
{
    buf_.reserve(256);
}

void Redopera::prError(const std::string &err)
{
    fprintf(stderr, "%s\n", err.c_str());
}

bool Redopera::check(bool b, const std::string &err)
{
    if(b)
        fprintf(stderr, "%s\n", err.c_str());

    return b;
}
