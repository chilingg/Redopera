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
        text_(RConvt::utf8ToUnicode(text))
    {}

    RText(const std::wstring &text):
        text_(text)
    {}

    RText(const char *text):
        text_(RConvt::utf8ToUnicode(text))
    {}

    RText(const wchar_t *text):
        text_(text)
    {}

    RText& operator=(const std::string &text) { text_ = RConvt::utf8ToUnicode(text); return *this; }
    RText& operator=(const std::wstring &text) { text_ = text; return *this; }
    RText& operator=(const char *text) { text_ = RConvt::utf8ToUnicode(text); return *this; }
    RText& operator=(const wchar_t *text) { text_ = text; return *this; }

    RText& operator=(std::wstring &&text) { text_ = std::move(text); return *this; }

    RText& operator+=(const std::string &text) { text_ += RConvt::utf8ToUnicode(text); return *this; }
    RText& operator+=(const std::wstring &text) { text_ += text; return *this; }
    RText& operator+=(const char *text) { text_ += RConvt::utf8ToUnicode(text); return *this; }
    RText& operator+=(const wchar_t *text) { text_ += text; return *this; }
    RText& operator+=(const char c) { text_ += RConvt::utf8ToUnicode(c); return *this; }
    RText& operator+=(const wchar_t c) { text_ += c; return *this; }

    wchar_t operator[](const size_t index) const { return text_[index]; }

    bool operator==(const RText& other) const { return text_ == other.text_; };

    std::string toString() const { return RConvt::unicodeToANSI(text_); }

    std::wstring toWString() { return text_; }
    const std::wstring toWString() const { return text_; }

    bool empty() const { return text_.empty(); }
    size_t size() const { return text_.size(); }

    void clear() { text_.clear(); }

private:
    std::wstring text_;
};

} // ns Redopera

#endif // RTEXT_H
