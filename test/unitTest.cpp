#include <RDebug.h>
#include <RColor.h>
#include <RPoint.h>
#include <RSize.h>
#include <RRect.h>
#include <RController.h>
#include <RKeeper.h>
#include <rsc/RResource.h>
#include <rsc/RImage.h>
#include <rsc/RLuaScript.h>
#include <rsc/RMp3.h>
#include <rsc/RFont.h>
#include <rsc/RPack.h>

#include <assert.h>
#include <functional>

using namespace Redopera;

int main()
{
    rDebug << "Testing class func:";

    // RColor ====================
    RColor color;
    assert(color.a() == 0xff && color.r() == 255 && color.g() == 255 && color.b() == 255);
    assert(color.rgba() == 0xffffffff && color.rgb() == 0xffffff);
    color.setA(0xF);
    color.setR(0x1E);
    color.setG(0x2D);
    color.setB(0x3C);
    assert(color.a() == 15 && color.r() == 30 && color.g() == 45 && color.b() == 60);
    assert(color.rgba() == 0x1e2d3c0f && color.rgb() == 0x1e2d3c);
    color.setRGBA(0xf0121314);
    assert(color.a() == 0x14 && color.r() == 0xf0 && color.g() == 0x12 && color.b() == 0x13);
    color.setRGBA(0xff222324);
    assert(color.a() == 0x24 && color.r() == 0xff && color.g() == 0x22 && color.b() == 0x23);

    // RPoint ====================
    RPoint pos1, pos2(0);
    assert(pos1 != pos2 && !pos1.isValid() && pos2.isValid() && !pos1.isOrigin() && pos2.isOrigin());
    pos1.set(10, 20, 30);
    pos2.setX(10);
    pos2.setY(20);
    pos2.setZ(30);
    assert(pos1 == pos2 && pos1.x() == 10 && pos1.y() == 20 && pos1.z() == 30);
    pos1 += pos2 + pos1;
    assert(pos1 - RPoint(10, 20, 30) == pos2 + RPoint(10, 20, 30));
    assert(pos2.mirrorH(20) == RPoint(30, 20) && pos2.mirrorV(-10) == RPoint(10, -40));

    // RSize ====================
    RSize size1, size2(10, 20);
    assert(size1 != size2 && size1.isEmpty() && size1.isInvalid() && size2.isValid());
    size1.set(20, 10);
    size2.expand(10, -10);
    assert(size1 == size2 && size1.width() == 20 && size1.height() == 10);
    size1.setWidth(30);
    size1.setHeight(15);
    size2 *= 3.f;
    assert(size2 == size1 *2. && size1 / 5 == RSize(6, 3));

    // RRect ====================
    RRect rect1, rect2(20, 20, 10, 10), rect3(RPoint2(10, 10), RPoint2(30, 30));
    assert(rect1.isEmpty() && rect1.isInvalid() && rect2.isValid() && rect2 == rect3 && rect1 != rect3);
    rect1.set(10, 10, 1, 1);
    rect2.setBottomLeft(1, 1);
    rect2.setTopRIght(11, 11);
    rect3.setPos(1, 1);
    rect3.setSize(10, 10);
    assert(rect1 == rect2 && rect2 == rect3 && rect1 == RRect(RSize(10, 10), RPoint2(1, 1)));
    rect1.setCenter(10, 10);
    rect2.move(4, 4);
    assert(rect1 == rect2 && rect1.topLeft() == RPoint2(5, 15) && rect1.bottomRight() == RPoint2(15, 5));
    assert(!rect1.contains(rect3) && rect1.contains(rect2) && rect1.contains(5, 5) && !rect1.contains(16, 15));
    assert(rect1.overlap(rect3));

    // RController ====================
    RController ctl;
    std::string order;
    ctl.setStartFunc([&order](StartEvent*){ order += '1'; });
    ctl.setUpdataFunc([&order, &ctl]{ order += '2'; ctl.breakLoop(); });
    ctl.setCloseFunc([&order](CloseEvent*){ order += '3'; });
    ctl.setFinishFunc([&order](FinishEvent*){ order += '4'; });
    ctl.exec();
    assert(order == "1234");

    // RKeeper
    int n = 0;
    RKeeper<int> keeper;
    {
        RKeeper<int> keeper2(1, [&n](int kn){ n += kn; });
    }
    keeper.reset(2, [&n](int kn){ n += kn; });
    keeper.reset();
    assert(n == 3);

    // RResource ====================
    RResource::setResourcePath("TestFile/");

    // RImage ====================
    RImage img(":/timg.png");
    assert(img.width() == 4 && img.height() == 2 && img.channel() == 3);
    std::array<RData, 24> data = { 200, 100, 50, 255, 0, 0, 0, 255, 0, 0, 0, 255,
                                   255, 255, 255, 0, 0, 0, 255, 255, 255, 0, 0, 0 };
    assert(std::equal(data.data(), data.data() + sizeof(data), img.data()));
    img.flipV();
    data = { 255, 255, 255, 0, 0, 0, 255, 255, 255, 0, 0, 0,
             200, 100, 50, 255, 0, 0, 0, 255, 0, 0, 0, 255 };
    assert(std::equal(data.data(), data.data() + sizeof(data), img.data()));
    img.load(data.data(), 4, 2, 3);
    img.flipH();
    data = { 0, 0, 0, 255, 255, 255, 0, 0, 0, 255, 255, 255,
             0, 0, 255, 0, 255, 0, 255, 0, 0, 200, 100, 50 };
    assert(std::equal(data.data(), data.data() + sizeof(data), img.data()));
    img.rotate90();
    data = { 0, 0, 255, 0, 0, 0, 0, 255, 0, 255, 255, 255,
             255, 0, 0, 0, 0, 0, 200, 100, 50, 255, 255, 255 };
    assert(std::equal(data.data(), data.data() + sizeof(data), img.data()));
    img.fill(20, 30, 40, 50); // 三通道图像忽略A参数
    data = { 20, 30, 40, 20, 30, 40, 20, 30, 40, 20, 30, 40,
             20, 30, 40, 20, 30, 40, 20, 30, 40, 20, 30, 40 };
    assert(std::equal(data.data(), data.data() + sizeof(data), img.data()));

    // RLuaScript ====================
    const char luaCode[] =
            "function max(num1, num2)\n"
                "if(num1 > num2) then\n"
                    "return num1;\n"
                "else\n"
                    "return num2;\n"
                "end\n"
            "end\n";

    RLuaScript scp(luaCode);
    assert(scp.isValid() && scp.call("max", {2, 5}, {}, 1) && scp.valueIsNumber() && scp.getInteger() == 5);
    scp.import(":/min.lua");
    assert(scp.isValid() && scp.call("min", {2, 5}, {}, 1) && scp.valueIsNumber() && scp.getInteger() == 2);

    // RMp3 ====================
    RMp3 mp3(":/bicycle_bell.mp3");
    assert(mp3.isValid() && mp3.hz() == 48000 && mp3.channel() == 2);

    // RFont ====================
    RFont font;
    const RFont::Glyph *glyoh = font.getFontGlyph(L'A');
    assert(font.isValid() && glyoh->width > 0 && glyoh->height > 0);

    // RPack ====================
    RPack pck;
    pck.packing(":/bicycle_bell.mp3", "mp3");
    assert(pck.getFileInfo("mp3")->size == 27018);
    pck.save(":/pck");
    RPack pck2(":/pck");
    const RPack::FInfo *info = pck2.getFileInfo("mp3");
    mp3.load(info->data.get(), info->size);
    assert(mp3.isValid() && mp3.hz() == 48000 && mp3.channel() == 2);

    rDebug << "End of test, No error occurred.";

    return 0;
}
