#ifndef RTEXT_H
#define RTEXT_H

#include <RConvt.h>

#include <memory>
#include <functional>
#include <locale>
#include <codecvt>

namespace Redopera {

class RText
{
public:
    RText() = default;
    RText(const RText &) = default;
    RText(RText &&) = default;
    RText& operator=(const RText&) = default;
    RText& operator=(RText&&) = default;

    RText(const std::string &text):
        text_(RConvt::U8_STR_CVT.from_bytes(text))
    {}

    RText(const std::wstring &text):
        text_(text)
    {}

    RText(const char *text):
        text_(RConvt::U8_STR_CVT.from_bytes(text))
    {}

    RText(const wchar_t *text):
        text_(text)
    {}

    RText& operator=(const std::string &text) { text_ = RConvt::U8_STR_CVT.from_bytes(text); return *this; }
    RText& operator=(const std::wstring &text) { text_ = text; return *this; }
    RText& operator=(const char *text) { text_ = RConvt::U8_STR_CVT.from_bytes(text); return *this; }
    RText& operator=(const wchar_t *text) { text_ = text; return *this; }

    RText& operator=(std::wstring &&text) { text_ = std::move(text); return *this; }

    bool operator==(const RText& other) const { return text_ == other.text_; };

    std::string toString() const { return RConvt::U8_STR_CVT.to_bytes(text_); }

    std::wstring toWString() { return text_; }
    const std::wstring toWString() const { return text_; }

private:
    std::wstring text_;
};

} // ns Redopera

#endif // RTEXT_H
