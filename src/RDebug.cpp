#include "RDebug.h"

#include <cstdio>
#include <cstring>
#include <codecvt>
#include <locale>
#include <functional>
#include <RColor.h>
#include <RPoint.h>
#include <RSize.h>
#include <RRect.h>
#include <RArea.h>

using namespace Redopera;

static std::wstring_convert<std::codecvt_utf8<wchar_t>> strcnv;

RDebug::~RDebug()
{
    std::printf("%s\n", buf_.c_str());
    fflush(stdout);
}

RDebug &RDebug::operator<<(int value)
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

RDebug &RDebug::operator<<(long value)
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

RDebug &RDebug::operator<<(long long value)
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

RDebug &RDebug::operator<<(unsigned value)
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

RDebug &RDebug::operator<<(unsigned long value)
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

RDebug &RDebug::operator<<(unsigned long long value)
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

RDebug &RDebug::operator<<(double value)
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

RDebug &RDebug::operator<<(float value)
{
    buf_ += std::to_string(value) + ' ';
    return *this;
}

RDebug &RDebug::operator<<(unsigned char c)
{
    buf_ += std::to_string(c*1) + ' ';
    return *this;
}

RDebug &RDebug::operator<<(bool b)
{
    buf_ += b ? "true " : "false ";
    return *this;
}

RDebug &RDebug::operator<<(char c)
{
    buf_ = buf_ + c + ' ';
    return *this;
}

RDebug &RDebug::operator<<(const char *str)
{
    buf_ += str;
    return *this;
}

RDebug &RDebug::operator<<(char *str)
{
    buf_ += str;
    return *this;
}

RDebug &RDebug::operator<<(const std::string &str)
{
    buf_ += str;
    return *this;
}

RDebug &RDebug::operator<<(wchar_t c)
{
    buf_ += strcnv.to_bytes(c);
    return *this;
}

RDebug &RDebug::operator<<(const wchar_t *str)
{
    buf_ += strcnv.to_bytes(str);
    return *this;
}

RDebug &RDebug::operator<<(wchar_t *str)
{
    buf_ += strcnv.to_bytes(str);
    return *this;
}

RDebug &RDebug::operator<<(const std::wstring &str)
{
    buf_ += strcnv.to_bytes(str);
    return *this;
}

RDebug &RDebug::operator<<(const RColor &color)
{
    buf_ += "(r:" + std::to_string(color.r()) + "g:" + std::to_string(color.g()) + "b:" + std::to_string(color.b()) + ") ";
    return *this;
}

RDebug &RDebug::operator<<(const RPoint2 &pos)
{
    buf_ += '(' + std::to_string(pos.x()) + ", " + std::to_string(pos.y()) + ") ";
    return *this;
}

RDebug &RDebug::operator<<(const RPoint3 &pos)
{
    buf_ += '(' + std::to_string(pos.x()) + ", " + std::to_string(pos.y()) + ", " + std::to_string(pos.z()) + ") ";
    return *this;
}

RDebug &RDebug::operator<<(const RSize &size)
{
    buf_ += "(w: " + std::to_string(size.width()) + " h: " + std::to_string(size.height()) + ") ";
    return *this;
}

RDebug &RDebug::operator<<(const RRect &rect)
{
    buf_ += "(" + std::to_string(rect.left()) + ", " + std::to_string(rect.bottom())
            + " | w: " + std::to_string(rect.width()) + " h: " + std::to_string(rect.height()) + ") ";
    return *this;
}

RDebug &RDebug::operator<<(const RArea &area)
{
    buf_ += "Area:( " + std::to_string(area.x()) + ", " + std::to_string(area.y()) + ", " + std::to_string(area.z())
            + " | W: " + std::to_string(area.width()) + " H: " + std::to_string(area.height())
            + " | M: " + std::to_string(area.areaFormat().margin.t) + ' ' + std::to_string(area.areaFormat().margin.b)
            + ' ' + std::to_string(area.areaFormat().margin.l) + ' ' + std::to_string(area.areaFormat().margin.r)
            + " | P: " + std::to_string(area.areaFormat().padding.t) + ' ' + std::to_string(area.areaFormat().padding.b)
            + ' ' + std::to_string(area.areaFormat().padding.l) + ' ' + std::to_string(area.areaFormat().padding.r) + ") ";
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
