#ifndef RFORMAT_H
#define RFORMAT_H

#include <fmt/format.h>
#include <fmt/color.h>

#include "RColor.h"
#include "RPoint.h"
#include "RSize.h"
#include "RRect.h"
#include "RTimer.h"
#include "RMath.h"

namespace Redopera {

template <typename... T>
void rError(std::string_view fmt, T&&... args)
{
    fmt::print(stderr, fg(fmt::color::red), fmt, std::forward<T>(args)...);
}

template <typename... T>
void rWarning(std::string_view fmt, T&&... args)
{
    fmt::print(stderr, fg(fmt::color::yellow), fmt, std::forward<T>(args)...);
}

template <typename... T>
bool rCheck(bool b, std::string_view fmt, T&&... args)
{
    if(b)
        fmt::print(stderr, fg(fmt::color::red), fmt, std::forward<T>(args)...);
    return b;
}

template <typename... T>
void rMessage(std::string_view fmt, T&&... args)
{
    fmt::print(fg(fmt::color::green), fmt, std::forward<T>(args)...);
}

}

template<>
struct fmt::formatter<Redopera::RColor>
{
    char presentation = 'd';

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();

        if(it != end)
        {
            switch(*it)
            {
            case '}':
                break;
            case 'd':
            case 'x':
            case 'f':
                presentation = *it++;
                break;
            default:
                throw  format_error("invalid format");
            }
        }

        return it;
    }

    template<typename  FormatContext>
    auto format(const Redopera::RColor &color, FormatContext& ctx) -> decltype(ctx.out())
    {
        switch(presentation)
        {
        case 'd':
            return format_to(ctx.out(),
                             fmt::runtime("({}, {}, {}, {})"),
                             color.r(), color.g(), color.b(), color.a());
        case 'x':
            return format_to(ctx.out(),
                             fmt::runtime("({:x})"),
                             color.rgba());
        case 'f':
            return format_to(ctx.out(),
                             fmt::runtime("({:.3f}, {:.3f}, {:.3f}, {:.3f})"),
                             color.rf(), color.gf(), color.bf(), color.af());
        }
        return ctx.out();
    }

}; // formatter RColor

template<typename Value>
struct fmt::formatter<Redopera::basic::RPoint2Basic<Value>>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();

        if(it != end && *it != '}')
            throw  format_error("invalid format");

        return it;
    }

    template<typename  FormatContext>
    auto format(const Redopera::basic::RPoint2Basic<Value> &pos, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "({}, {})", pos.x(), pos.y());
    }

}; // formatter RPoint2Basic

template<typename Value>
struct fmt::formatter<Redopera::basic::RPoint3Basic<Value>>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();

        if(it != end && *it != '}')
            throw  format_error("invalid format");

        return it;
    }

    template<typename  FormatContext>
    auto format(const Redopera::basic::RPoint3Basic<Value> &pos, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "({}, {}, {})", pos.x(), pos.y(), pos.z());
    }

}; // formatter RPoint3Basic

template<typename Value>
struct fmt::formatter<Redopera::basic::RSizeBasic<Value>>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();

        if(it != end && *it != '}')
            throw  format_error("invalid format");

        return it;
    }

    template<typename  FormatContext>
    auto format(const Redopera::basic::RSizeBasic<Value> &size, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "(w{}, h{})", size.width(), size.height());
    }

}; // formatter RSizeBasic

template<typename Value>
struct fmt::formatter<Redopera::basic::RRectBasic<Value>>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();

        if(it != end && *it != '}')
            throw  format_error("invalid format");

        return it;
    }

    template<typename  FormatContext>
    auto format(const Redopera::basic::RRectBasic<Value> &rect, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "({}, {} | w{}, h{})", rect.x(), rect.y(), rect.width(), rect.height());
    }

}; // formatter RRectBasic

template<>
struct fmt::formatter<Redopera::RTimer>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();

        if(it != end && *it != '}')
            throw  format_error("invalid format");

        return it;
    }

    template<typename  FormatContext>
    auto format(const Redopera::RTimer &timer, FormatContext& ctx) -> decltype(ctx.out())
    {
        auto t = timer.elapsed();
        return format_to(ctx.out(), "({}:{}:{})", t / (60*60), t / 60 % 60, t % 60);
    }

}; // formatter RTimer

template<>
struct fmt::formatter<Redopera::RTimerMS>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();

        if(it != end && *it != '}')
            throw  format_error("invalid format");

        return it;
    }

    template<typename  FormatContext>
    auto format(const Redopera::RTimerMS &timer, FormatContext& ctx) -> decltype(ctx.out())
    {
        auto t = timer.elapsed();
        return format_to(ctx.out(), "({}:{}:{})", t / 1000 / 60, t / 1000 % 60, t % 1000);
    }

}; // formatter RTimerMS

template<>
struct fmt::formatter<glm::vec3>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();

        if(it != end && *it != '}')
            throw  format_error("invalid format");

        return it;
    }

    template<typename  FormatContext>
    auto format(const glm::vec3 &v3, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f})", v3.x, v3.y, v3.z);
    }

}; // formatter glm::vec3

template<>
struct fmt::formatter<glm::vec4>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();

        if(it != end && *it != '}')
            throw  format_error("invalid format");

        return it;
    }

    template<typename  FormatContext>
    auto format(const glm::vec4 &v4, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f}, {:.3f})", v4.x, v4.y, v4.z, v4.w);
    }

}; // formatter glm::vec4

template<>
struct fmt::formatter<glm::mat4>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();

        if(it != end && *it != '}')
            throw  format_error("invalid format");

        return it;
    }

    template<typename  FormatContext>
    auto format(const glm::mat4 &mat, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(),
                         "\n({:7.3f}, {:7.3f}, {:7.3f}, {:7.3f})\n"
                         "({:7.3f}, {:7.3f}, {:7.3f}, {:7.3f})\n"
                         "({:7.3f}, {:7.3f}, {:7.3f}, {:7.3f})\n"
                         "({:7.3f}, {:7.3f}, {:7.3f}, {:7.3f})",
                         mat[0][0], mat[1][0], mat[2][0], mat[3][0],
                         mat[0][1], mat[1][1], mat[2][1], mat[3][1],
                         mat[0][2], mat[1][2], mat[2][2], mat[3][2],
                         mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
    }

}; // formatter glm::mat4

#endif // RFORMAT_H
