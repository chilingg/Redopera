#ifndef RENCODE_H
#define RENCODE_H

#include <locale>
#include <codecvt>

namespace Redopera {

class RConvt
{
public:
    static thread_local std::wstring_convert<std::codecvt_utf8<wchar_t>> U8_STR_CVT;

    RConvt() = delete;
};

} // ns Redopera

#endif // RENCODE_H
