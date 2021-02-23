#include <RDebug.h>
#include <RColor.h>
#include <RPoint.h>
#include <RSize.h>
#include <RRect.h>
#include <RTime.h>
#include <RMath.h>
#include <RTransform.h>
#include <RName.h>
#include <RText.h>

using namespace Redopera;

class Test
{
public:
    std::string toStdString() const
    {
        return std::string("Test class" + std::to_string(x) + ", " + std::to_string(y));
    }
    int x = 9, y = 8;
};

const RDebug& operator<<(const RDebug &debug, const Test &test)
{
    return debug << test.toStdString();
}

int main()
{
    rPrError("\nTest RDebug error output");
    rPrError("Test prErrot()");
    rCheck(true, "Test check(true)");
    rCheck(false, "Test check(false)");

    rDebug << "\nTest RDebug output";
    rDebug << -123456 << -123456789l << -1234567890ll;
    rDebug << 123456u << 123456789lu << 1234567890llu;
    rDebug << 123456.789 << 123456.789f;
    bool b = false;
    rDebug << b << !b << &b;
    Test t;
    rDebug << t;

    rDebug << "\nTest RDebug str output";
    unsigned uc = 'u';
    const char chars[] = "chars";
    rDebug << std::string("std::string ") << 'c' << uc << chars;
    const char chars2[] = u8"中文";
    rDebug << std::string("中文 ") << chars2;

    rDebug << L"\n测试 RDebug wstr 输出";
    const wchar_t *wchars = L" 字符";
    rDebug << std::wstring(L"标准库::wstring ") << L'中' << wchars;

    rDebug << L"\n测试 RDebug RName 输出";
    RName name = "name";
    rDebug << name;

    rDebug << L"\n测试 RDebug RText 输出";
    RText text1 = "文本1";
    RText text2 = L"文本2";
    rDebug << "char init: " << text1;
    rDebug << "wchar_t init: " << text2 << '\n';

    rDebug << "Test color input";
    rDebug << EscCtl::bold << EscCtl::yellow << "Yellow" << EscCtl::non;

    rDebug << "Test class debug input";
    rDebug << "RColor(0xeeddccff)\t>>\t" << RColor(0xeeddccff);
    rDebug << "RPoint(2, 3)\t>>\t" << RPoint(2, 3);
    rDebug << "RSize(5, 6) \t>>\t" << RSize(5, 6);
    rDebug << "RRect(8, 8, 2, 3)\t>>\t" << RRect(8, 8, 2, 3);
    rDebug << "RTransform(10, 11, 12, 20, 30)\t>>\t" << RTransform(10, 11, 12, 20, 30);
    rDebug << RTime();

    rDebug << "Test glm debug input";
    rDebug << glm::vec3(1, 2, 3);
    rDebug << glm::vec4(1, 2, 3, 4);
    rDebug << glm::mat4(1);

    rDebug << "End of test.";

    return 0;
}
