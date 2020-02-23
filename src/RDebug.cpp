#include <RDebug.h>

#include <cstdio>
#include <cstring>
#include <codecvt>
#include <locale>
#include <thread>
#include <functional>

static std::wstring_convert<std::codecvt_utf8<wchar_t>> strcnv;

Redopera::RDebug::~RDebug()
{
    data_[index_] = '\0';
    printf("%s\n", data_);
}

Redopera::RDebug &Redopera::RDebug::append(const std::string &str)
{
    std::memcpy(data_ + index_, str.data(), str.size());
    index_ += str.size();
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(int value)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%d ", value);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(long value)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%ld ", value);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(long long value)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%lld ", value);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(unsigned value)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%u ", value);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(unsigned long value)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%lu ", value);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(unsigned long long value)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%llu ", value);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(double value)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%lf ", value);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(float value)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%f ", value);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(unsigned char c)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%u ", c);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(bool b)
{
    return append(b ? "true " : "false ");
}

Redopera::RDebug &Redopera::RDebug::operator<<(char c)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%c ", c);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(const char *str)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%s", str);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(char *str)
{
    index_ += std::snprintf(data_ + index_, DATA_SIZE - index_ - 1, "%s", str);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(const std::string &str)
{
    append(str);
    return *this;
}

Redopera::RDebug &Redopera::RDebug::operator<<(wchar_t c)
{
    return append(strcnv.to_bytes(c));
}

Redopera::RDebug &Redopera::RDebug::operator<<(const wchar_t *str)
{
    return append(strcnv.to_bytes(str));
}

Redopera::RDebug &Redopera::RDebug::operator<<(wchar_t *str)
{
    return append(strcnv.to_bytes(str));
}

Redopera::RDebug &Redopera::RDebug::operator<<(const std::wstring &str)
{
    return append(strcnv.to_bytes(str));
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
