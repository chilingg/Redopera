#ifndef RENCODE_H
#define RENCODE_H

#include <locale>
#include <codecvt>
#include <cstring>

namespace Redopera {

class RConvt
{
public:
    static std::string unicodeToUTF8(const std::wstring &wstr) { return U8_STR_CVT.to_bytes(wstr); }
    static std::wstring utf8ToUnicode(const wchar_t wc) { return U8_STR_CVT.from_bytes(wc); }
    static std::wstring utf8ToUnicode(const std::string &str) { return U8_STR_CVT.from_bytes(str); }
    static std::wstring utf8ToUnicode(const char c) { return U8_STR_CVT.from_bytes(c); }

    static std::string unicodeToANSI(wchar_t wc)
    {
        return unicodeToANSI(std::wstring(1, wc));
    }

    static std::string unicodeToANSI(const std::wstring &wstr)
    {
        std::memset(&mbstate, 0, sizeof(mbstate));
        const wchar_t *data = wstr.data();
        size_t len = std::wcsrtombs(nullptr, &data, 0, &mbstate);

        if(len == static_cast<size_t>(-1))
            return {};

        std::string str(len, 'a');
        std::wcsrtombs(str.data(), &data, len, &mbstate);
        return str;
    }

    static std::wstring ansiToUnicode(char c)
    {
        return ansiToUnicode(std::string(1, c));
    }

    static std::wstring ansiToUnicode(const std::string &str)
    {
        std::memset(&mbstate, 0, sizeof(mbstate));
        const char *data = str.data();
        size_t len = std::mbsrtowcs(nullptr, &data, 0, &mbstate);

        if(len == static_cast<size_t>(-1))
            return {};

        std::wstring wstr(len, 'a');
        std::mbsrtowcs(wstr.data(), &data, len, &mbstate);
        return wstr;
    }

    static std::string utf8ToANSI(const std::string &str) { return unicodeToANSI(utf8ToUnicode(str)); }
    static std::string ansiToUTF8(const std::string &str) { return unicodeToUTF8(ansiToUnicode(str)); }

    RConvt() = delete;

private:
    static thread_local std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> U8_STR_CVT;
    static thread_local std::mbstate_t mbstate;
};

} // ns Redopera

#endif // RENCODE_H
