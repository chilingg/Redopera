#include "RDebug.h"

using namespace Redopera;

std::mutex RDebug::mutex;
std::wstring_convert<std::codecvt_utf8<wchar_t>> RDebug::strcnv;

RDebug::RDebug():
    guard(mutex)
{

}

RDebug::~RDebug()
{
    std::cout << std::endl;
}

const RDebug &RDebug::operator<<(int value) const
{
    std::cout << value << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(long value) const
{
    std::cout << value << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(long long value) const
{
    std::cout << value << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(unsigned value) const
{
    std::cout << value << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(unsigned long value) const
{
    std::cout << value << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(unsigned long long value) const
{
    std::cout << value << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(double value) const
{
    std::cout << value << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(float value) const
{
    std::cout << value << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(void *p) const
{
    std::cout << p << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(bool b) const
{
    const char *str = b ? "true " : "fales ";
    std::cout << str;
    return *this;
}

const RDebug &RDebug::operator<<(char c) const
{
    std::cout << c;
    return *this;
}

const RDebug &RDebug::operator<<(unsigned char c) const
{
    std::cout << c;
    return *this;
}

const RDebug &RDebug::operator<<(const char *str) const
{
    std::cout << str;
    return *this;
}

const RDebug &RDebug::operator<<(const unsigned char *c) const
{
    std::cout << c;
    return *this;
}

const RDebug &RDebug::operator<<(const std::string &str) const
{
    std::cout << str;
    return *this;
}

const RDebug &RDebug::operator<<(wchar_t c) const
{
    std::string s = strcnv.to_bytes(c);
    std::cout << s;
    return *this;
}

const RDebug &RDebug::operator<<(const wchar_t *str) const
{
    std::string s = strcnv.to_bytes(str);
    std::cout << s;
    return *this;
}

const RDebug &RDebug::operator<<(const std::wstring &str) const
{
    std::string s = strcnv.to_bytes(str);
    std::cout << s;
    return *this;
}

const RDebug &RDebug::operator<<(std::ios_base &(*base)(std::ios_base &)) const
{
    std::cout << base;
    return *this;
}

const RDebug &RDebug::operator<<(std::thread::id id) const
{
    std::cout << id << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(const RColor &color) const
{
    std::cout << "RGBA:(" << color.r()*1u << ", " << color.g()*1u << ", " << color.b()*1u << ", " << color.a()*1u << ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const RPoint2 &p) const
{
    std::cout << '(' << p.x() << ", " << p.y() << ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const RPoint3 &p) const
{
    std::cout << '(' << p.x() << ", " << p.y() << ", " << p.z() << ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const RSize &size) const
{
    std::cout << "(W: " << size.width() << " H: " << size.height() << ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const RRect &rect) const
{
    std::cout << "(" << rect.left() << ", " << rect.bottom() << " | W: " << rect.width() << " H: " << rect.height() << ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const glm::vec3 &vec) const
{
    std::cout << '(' << vec.x << ", " << vec.y << ", " << vec.z << ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const glm::vec4 &vec) const
{
    std::cout << '(' << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ") ";
    return *this;
}

const RDebug &RDebug::operator<<(const glm::mat4 &mat) const
{
    //std::cout << std::setw(8);
    std::cout << std::left;
    std::cout << "mat:(" << std::setw(8) << mat[0][0] << " , " << std::setw(8) <<  mat[1][0] << " , "
                         << std::setw(8) << mat[2][0] << " , " << std::setw(8) <<  mat[3][0] << ")\n";
    std::cout << "    (" << std::setw(8) << mat[0][1] << " , " << std::setw(8) <<  mat[1][1] << " , "
                         << std::setw(8) << mat[2][1] << " , " << std::setw(8) <<  mat[3][1] << ")\n";
    std::cout << "    (" << std::setw(8) << mat[0][2] << " , " << std::setw(8) <<  mat[1][2] << " , "
                         << std::setw(8) << mat[2][2] << " , " << std::setw(8) <<  mat[3][2] << ")\n";
    std::cout << "    (" << std::setw(8) << mat[0][3] << " , " << std::setw(8) <<  mat[1][3] << " , "
                         << std::setw(8) << mat[2][3] << " , " << std::setw(8) <<  mat[3][3] << "). ";
    return *this;
}

const RDebug &RDebug::operator<<(const RTime &time) const
{
    std::cout << time.toString() << ' ';
    return *this;
}

const RDebug &RDebug::operator<<(const RArea &area) const
{
    std::cout << "Area:( " << area.x() << ", " << area.y() << ", " << area.z()
              << " | W: " << area.width() << " H: " << area.height()
              << " | M: " << area.area().margin.t << ' ' << area.area().margin.b
              << ' ' << area.area().margin.l << ' ' << area.area().margin.r
              << " | P: " << area.area().padding.t << ' ' << area.area().padding.b
              << ' ' << area.area().padding.l << ' ' << area.area().padding.r << ") ";
    return *this;
}
