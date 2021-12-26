#include <RFormat.h>
#include <fmt/color.h>

#include <RColor.h>
#include <RPoint.h>
#include <RSize.h>
#include <RRect.h>
#include <RTimer.h>
#include <thread>
#include <RMath.h>
#include <RSigslot.h>
#include <RKeeper.h>
#include <RTextLoader.h>

#include <rsc/RImage.h>
#include <rsc/RFont.h>
#include <rsc/RMusic.h>
#include <rsc/RSoundEffect.h>

using namespace Redopera;

template<typename T, typename T2>
void equal(T&& v1, T2&& v2, unsigned line)
{
    if(!(v1 == v2))
        fmt::print(fg(fmt::color::red), "Line {}: {} != {}\n", line, v1, v2);
}

void require(bool b, unsigned line)
{
    if(!b)
        fmt::print(fg(fmt::color::red), "Line {}: Not a requirement!\n", line);
}

template<typename T, typename T2, typename F>
void require_of(T v1, T2 v2, F f, unsigned line)
{
    if(!f(v1, v2))
        fmt::print(fg(fmt::color::red), "Line {}: Unexpected of {} and {}\n", line, v1, v2);
}

#define EQUAL(v1, v2) equal(v1, v2, __LINE__)
#define REQUIRE(b) require(b, __LINE__)
#define REQUIRE_OF(v1, v2, f) require_of(v1, v2, f, __LINE__)

void testColor()
{
    fmt::print("RColor Testing...\n");

    bool b = true;
    RColor color1, color2(0x10111213), color3(20, 30, 40, 50);

    EQUAL(sizeof(RColor), 4);
    EQUAL(color1.rf(), 1);
    EQUAL(color1.gf(), 1.f);
    EQUAL(color2.bf(), 0x12/255.f);
    EQUAL(color3.af(), 50/255.f);

    EQUAL(color1.r(), 255);
    EQUAL(color2.g(), 17);
    EQUAL(color2.b(), 18);
    EQUAL(color3.a(), 50);

    EQUAL(color3.rgba(), 0x141e2832);
    EQUAL(color3.rgb(), 0x141e28);
    EQUAL(color3.bgr(), 0x281e14);
    EQUAL(color2.abgr(), 0x13121110);

    uint32_t d = color3.abgr();
    uint8_t *p = reinterpret_cast<uint8_t*>(&d);
    EQUAL(p[0]*1, 20);
    EQUAL(p[1]*1, 30);
    EQUAL(p[2]*1, 40);
    EQUAL(p[3]*1, 50);

    color1.setR(1);
    color1.setG(2);
    color1.setB(3);
    color1.setA(4);
    EQUAL(color1.data().rgba, 0x01020304);
    color2.setRGBA(1, 2, 3, 4);
    EQUAL(color1, color2);
    color3.setRGBA(0x01020304);
    EQUAL(color1, color3);
    color3.setRGB(0x50101010);
    EQUAL(color3.data().rgba, 0x10101004);
    color2.setRGB(16, 16, 16);
    EQUAL(color3.rgb(), color2.rgb());
}

void testPoint()
{
    fmt::print("RPoint Testing...\n");

    bool b = true;
    RPoint pos1(1, 2, 3), pos2;

    EQUAL(pos2 ,RPoint(0, 0, 0));
    REQUIRE(!pos1.isOrigin());
    REQUIRE(pos2.isOrigin());
    pos1.setPos(10, 20, 30);
    pos2.setX(10);
    pos2.setY(20);
    pos2.setZ(30);
    EQUAL(pos1, pos2);
    EQUAL(pos1.x(), 10);
    EQUAL(pos1.y(), 20);
    EQUAL(pos1.z(), 30);
    pos1 += pos2 + pos1;
    REQUIRE(pos1 - RPoint(10, 20, 30) == pos2 + RPoint(10, 20, 30));
    pos2.mirrorH(20);
    EQUAL(pos2, RPoint(30, 20, 30));
    pos2.mirrorV(-10);
    EQUAL(pos2, RPoint(30, -40, 30));

    int *p = reinterpret_cast<int*>(&pos2);
    EQUAL(p[0], 30);
    EQUAL(p[1], -40);
    EQUAL(p[2], 30);
}

void testSize()
{
    fmt::print("RSize Testing...\n");

    RSize size1, size2(10, 20);

    EQUAL(size1, RSize(0, 0));
    REQUIRE(size1.isEmpty());
    REQUIRE(size1.isInvalid());
    REQUIRE(size2.isValid());
    size1.setSize(20, 10);
    size2.expand(10, -10);
    EQUAL(size1, size2);
    EQUAL(size1.width(), 20);
    EQUAL(size1.height(), 10);
    size1.setWidth(30);
    size1.setHeight(15);
    size2 *= 3.f;
    EQUAL(size2, size1 *2.);
    EQUAL(size1 / 5, RSize(6, 3));

    int *p = reinterpret_cast<int*>(&size1);
    EQUAL(p[0], 30);
    EQUAL(p[1], 15);
}

void testRect()
{
    fmt::print("RRect Testing...\n");

    RRect rect1, rect2(20, 20, 10, 10), rect3(RPoint2(20, 20), RPoint2(30, 30));

    REQUIRE(rect1.isEmpty());
    REQUIRE(rect1.isInvalid());
    REQUIRE(rect2.isValid());
    EQUAL(rect2, rect3);
    REQUIRE(rect1 != rect3);
    rect1.setRect(1, 1, 10, 10);
    rect2.setBottomLeft(1, 1);
    rect2.setTopRIght(11, 11);
    rect3.setPos(1, 1);
    rect3.setSize(10, 10);
    EQUAL(rect1, rect2);
    EQUAL(rect2, rect3);
    EQUAL(rect1, RRect(RPoint2(1, 1), RSize(10, 10)));
    rect1.setCenter(10, 10);
    rect2.move(4, 4);
    EQUAL(rect1, rect2);
    EQUAL(rect1.topLeft(), RPoint2(5, 15));
    EQUAL(rect1.bottomRight(), RPoint2(15, 5));
    REQUIRE(!rect1.contains(rect3));
    REQUIRE(rect1.contains(rect2));
    REQUIRE(rect1.contains(5, 5));
    REQUIRE(!rect1.contains(16, 15));
    REQUIRE(rect1.overlap(rect3));

    int *p = reinterpret_cast<int*>(&rect1);
    EQUAL(p[0], 5);
    EQUAL(p[1], 5);
    EQUAL(p[2], 10);
    EQUAL(p[3], 10);
}

void testTimer()
{
    fmt::print("RTimer and RStopwatch Testing...\n");

    RStopwatch sw1;
    RStopwatchMS sw2;
    RStopwatchNS sw3;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE_OF(sw1.elapsed(), 99, std::greater<RStopwatch::rep>());
    REQUIRE_OF(sw2.elapsed(), 99999, std::greater<RStopwatchMS::rep>());
    REQUIRE_OF(sw3.elapsed(), 99999999, std::greater<RStopwatchNS::rep>());

    RTimerMS timer;
    REQUIRE(timer.isPaused());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EQUAL(timer.elapsed(), 0);
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE_OF(timer.elapsed(), 99, std::greater<RStopwatch::rep>());
    timer.pause();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE_OF(timer.elapsed(), 99, std::greater<RStopwatch::rep>());
    timer = RTimerMS(true);
    REQUIRE(timer.isStarted());
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    REQUIRE_OF(timer.elapsed(), 199, std::greater<RStopwatch::rep>());
}

void testMath()
{
    fmt::print("RMath Testing...\n");

    glm::mat4 mat1(1), mat2, mat3;
    EQUAL(mat1, math::unitize(mat2));
    REQUIRE(mat2 != mat3);

    mat1 = glm::scale(mat1, { 8, 4, 1 });
    mat3 = math::scale(RSizeF(8, 4));
    math::setScale(mat2, RSizeF(8, 4));
    EQUAL(mat1, mat2);
    EQUAL(mat1, mat3);

    mat3 = glm::translate(mat1, { 3, 6, 0 });
    math::setMove(mat2, RPointF(3, 6));
    EQUAL(mat2, mat3);

    RRectF rect(4.7f, 6.2f, 23.2f, 42.3f);
    mat1 = glm::translate(glm::mat4(1), { rect.x(), rect.y(), 0 });
    mat1 = glm::scale(mat1, { rect.width(), rect.height(), 1 });
    math::unitize(mat2);
    math::setMove(mat2, rect.pos());
    math::setScale(mat2, rect.size());
    EQUAL(mat1, mat2);
    rect.setCenter(rect.pos());
    math::setRectAs(mat3, rect);
    EQUAL(mat1, mat3);

    math::setMove(math::unitize(mat1), { 0, 5 });
    math::setRect(mat1, rect);
    rect.move(0, 5);
    math::setRectAs(mat3, rect);
    EQUAL(mat1, mat3);
}

void testImage()
{
    fmt::print("RImage Testing...\n");

    RImage img("TestFile/timg.png");
    EQUAL(img.width(), 4);
    EQUAL(img.height(), 2);
    EQUAL(img.channel(), 3);
    std::array<RData, 24> data = { 200, 100, 50, 255, 0, 0, 0, 255, 0, 0, 0, 255,
                                   255, 255, 255, 0, 0, 0, 255, 255, 255, 0, 0, 0 };
    REQUIRE(std::equal(data.data(), data.data() + sizeof(data), img.data()));
    img.flipV();
    data = { 255, 255, 255, 0, 0, 0, 255, 255, 255, 0, 0, 0,
             200, 100, 50, 255, 0, 0, 0, 255, 0, 0, 0, 255 };
    REQUIRE(std::equal(data.data(), data.data() + sizeof(data), img.data()));
    img.load(data.data(), 4, 2, 3);
    img.flipH();
    data = { 0, 0, 0, 255, 255, 255, 0, 0, 0, 255, 255, 255,
             0, 0, 255, 0, 255, 0, 255, 0, 0, 200, 100, 50 };
    REQUIRE(std::equal(data.data(), data.data() + sizeof(data), img.data()));
    img.rotate90();
    data = { 0, 0, 255, 0, 0, 0, 0, 255, 0, 255, 255, 255,
             255, 0, 0, 0, 0, 0, 200, 100, 50, 255, 255, 255 };
    REQUIRE(std::equal(data.data(), data.data() + sizeof(data), img.data()));
    img.fill(0x141e2832); // 20, 30, 40, 50) // 三通道图像忽略A参数
    data = { 20, 30, 40, 20, 30, 40, 20, 30, 40, 20, 30, 40,
             20, 30, 40, 20, 30, 40, 20, 30, 40, 20, 30, 40 };
    REQUIRE(std::equal(data.data(), data.data() + sizeof(data), img.data()));
}

void testSigSlot()
{
    fmt::print("RSignal and RSloter Testing...\n");

    RSignal<int, int&> signal;
    RSloter slot1, slot2;
    int i = 0;
    size_t slotID;

    {
        RSloter slot3;
        signal.connect(&slot1, [](int n, int &m){ ++n; ++m; });
        signal.connect(&slot2, [](int n, int &m){ ++n; ++m; });
        signal.connect(&slot3, [](int n, int &m){ ++n; ++m; });
        slotID = signal.connect(nullptr, [](int n, int &m){ ++n; ++m; });
    }

    signal.emit(i, i);
    EQUAL(i, 3);
    slot2.free();
    signal.emit(i, i);
    EQUAL(i, 5);
    REQUIRE(signal.disconnect(slotID));
    signal.emit(i, i);
    EQUAL(i, 6);
}

void testKeeper()
{
    fmt::print("RKeeper and RSloter Testing...\n");

    int n = 0;
    RKeeper<int, std::function<void(int)>> keeper;
    REQUIRE(!keeper.isValid());
    {
        decltype(keeper) keeper2(1, [&n](int kn){ n += kn; });
        REQUIRE(keeper2);
        REQUIRE(keeper2.isValid());
    }
    keeper.reset(2, [&n](int kn){ n += kn; });
    decltype(keeper) keeper2(1, [&n](int kn){ n += kn; });
    keeper2.release();
    keeper.free();
    EQUAL(n, 3);

}

void testFont()
{
    fmt::print("RFont and RSloter Testing...\n");

    RFont font;
    const RFont::Glyph glyoh = font.getGlyph(L'A');
    REQUIRE(font.isValid());
    REQUIRE_OF(glyoh.width, 0, std::not_equal_to<int>());
    REQUIRE_OF(glyoh.height, 0, std::not_equal_to<int>());

}

void testMusic()
{
    fmt::print("RMusic and RSoundEffect Testing...\n");

    RMusic music("TestFile/bicycle_bell.mp3");
    REQUIRE(music.isValid());
    RSoundEffect chunk("TestFile/bicycle_bell.mp3");
    REQUIRE(chunk.isValid());
    EQUAL(chunk.volume(), MIX_MAX_VOLUME);
}

void testTextLoader()
{
    fmt::print("RTextLoader Testing...\n");

    RTextLoader loader;
    EQUAL(loader.getTextBoxSize(L"s"), RSize(8, 16));
    EQUAL(loader.getTextBoxSize(L"s\n"), loader.getTextBoxSize(L"s"));
}

int main()
{
    testColor();
    testPoint();
    testSize();
    testRect();
    testTimer();
    testMath();
    testImage();
    testSigSlot();
    testKeeper();
    testFont();
    testMusic();
    testTextLoader();

    fmt::print("\nEnd of test.\n");
    return 0;
}
