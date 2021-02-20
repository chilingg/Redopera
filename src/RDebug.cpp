#include <RDebug.h>
#include <RConvt.h>

using namespace Redopera;

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
    buf_ += RConvt::U8_STR_CVT.to_bytes(c) + ' ';
    return *this;
}

const RDebug &RDebug::operator<<(const wchar_t *str) const
{
    buf_ += RConvt::U8_STR_CVT.to_bytes(str);
    return *this;
}

const RDebug &RDebug::operator<<(wchar_t *str) const
{
    buf_ += RConvt::U8_STR_CVT.to_bytes(str);
    return *this;
}

const RDebug &RDebug::operator<<(const std::wstring &str) const
{
    buf_ += RConvt::U8_STR_CVT.to_bytes(str);
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

RDebug::RDebug()
{
    buf_.reserve(DATA_SIZE);
}

void Redopera::rPrError(const std::string &err)
{
    rDebug << EscCtl::red << err << EscCtl::non;
}

bool Redopera::rCheck(bool b, const std::string &err)
{
    if(b)
        rDebug << EscCtl::red << err << EscCtl::non;

    return b;
}
