#include <fmt/core.h> // Example 1
#include <fmt/chrono.h> // Example 2
#include <fmt/ranges.h> // Example 3
#include <vector>
#include <fmt/os.h> // Example 4
#include <fmt/color.h> // Example 5

#include <RFormat.h>
#include <RColor.h>
#include <RPoint.h>
#include <RSize.h>
#include <RRect.h>
#include <RTimer.h>
#include <thread>
#include <RFormat.h>

using namespace Redopera;

int main()
{
    // Example 1
    fmt::print("Hello world\n");

    std::string s1 = fmt::format("The answer is {}.", 42);
    std::string s2 = fmt::format("I'd rather be {1} than {0}.", "right", "happy");
    fmt::print("{}\n{}\n", s1, s2);

    fmt::print("{:.2f}\n", 0.123456);

    fmt::print("Hello, {name}! The answer is {number}. Goodbye, {name}.\n",
               fmt::arg("name", "World"), fmt::arg("number", 42));

    { // Example 2
        using namespace std::literals::chrono_literals;
        fmt::print("Default format: {} {}\n", 42s, 100ms);
        fmt::print("strftime-like format: {:%H:%M:%S}\n", 3h + 15min + 30s);
    }

    // Example 3
    std::vector<int> v = { 1, 2, 3 };
    fmt::print("{}\n", v);

    // Example 4
    auto out = fmt::output_file("test_fmt.txt");
    out.print("Don't {}", "Panic");

    // Example 5
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
               "Hello, {}!\n", "world");
    fmt::print(fg(fmt::color::floral_white) | bg(fmt::color::slate_gray) | fmt::emphasis::underline,
               "Hello, {}!\n", "world");
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "Hello, {}!\n", "world");

    // Testing R Class
    RColor color(0xa0b0c0dd);
    fmt::print("Color is RGB:{0}, hex:{0:x}, f{0:f}\n", color);

    RPoint p1(1, 2, 3);
    RPoint2 p2(4, 5);
    fmt::print("RPoint{}, RPoint2{}\n", p1, p2);
    RPointF p3(1.2, 2.3, 3);
    RPoint2F p4(4, 5.6);
    fmt::print("RPointF{}, RPoint2F{}\n", p3, p4);

    RSize size(10, 21);
    RSizeF sizef(10.1, 21.2);
    fmt::print("RSize{}, RSizeF{}\n", size, sizef);

    RRect rect(1, 2, 10, 21);
    RRectF rectf(1, 2, 10.1, 21.2);
    fmt::print("RRect{}, RRectF{}\n", rect, rectf);

    glm::vec3 v3(1, 2, 3);
    glm::vec4 v4(4, 5, 6, 7);
    glm::mat4 mat(1);
    mat = glm::translate(mat, { 5, 4, 2 });
    mat = glm::scale(mat, { 10.f, 20.f, 0.f });
    fmt::print("vec3{}, vec4{}\n{}mat\n", v3, v4, mat);

    RTimerMS timer(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    fmt::print("The program sleep for {}\n", timer);

    return 0;
}
