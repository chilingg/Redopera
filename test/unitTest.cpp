#include <RDebug.h>
#include <RColor.h>
#include <RPoint.h>
#include <RSize.h>
#include <RRect.h>
#include <RKeeper.h>
#include <RNode.h>
#include <RResource.h>
#include <rsc/RImage.h>
#include <rsc/RScript.h>
#include <rsc/RMp3.h>
#include <rsc/RFont.h>
#include <rsc/RPack.h>
#include <RModelMat.h>
#include <REntity.h>

#include <cassert>

using namespace Redopera;

int main()
{
    rDebug << "Testing class func:";

    // RColor ====================
    RColor color;
    assert(color.a() == 0xff && color.r() == 0 && color.g() == 0 && color.b() == 0);
    assert(color.rgba() == 0xff && color.rgb() == 0);
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
    RPoint pos1(1, 2, 3), pos2;
    assert(pos1 != pos2);
    assert(!pos1.isOrigin());
    assert(pos2.isOrigin());
    pos1.setPos(10, 20, 30);
    pos2.setX(10);
    pos2.setY(20);
    pos2.setZ(30);
    assert(pos1 == pos2 && pos1.x() == 10 && pos1.y() == 20 && pos1.z() == 30);
    pos1 += pos2 + pos1;
    assert(pos1 - RPoint(10, 20, 30) == pos2 + RPoint(10, 20, 30));
    pos2.mirrorH(20);
    assert(pos2 == RPoint(30, 20, 30));
    pos2.mirrorV(-10);
    assert(pos2 == RPoint(30, -40, 30));

    // RSize ====================
    RSize size1, size2(10, 20);
    assert(size1 != size2 && size1.isEmpty() && size1.isInvalid() && size2.isValid());
    size1.setSize(20, 10);
    size2.expand(10, -10);
    assert(size1 == size2 && size1.width() == 20 && size1.height() == 10);
    size1.setWidth(30);
    size1.setHeight(15);
    size2 *= 3.f;
    assert(size2 == size1 *2. && size1 / 5 == RSize(6, 3));

    // RRect ====================
    RRect rect1, rect2(20, 20, 10, 10), rect3(RPoint2(20, 20), RPoint2(30, 30));
    assert(rect1.isEmpty());
    assert(rect1.isInvalid());
    assert(rect2.isValid());
    assert(rect2 == rect3);
    assert(rect1 != rect3);
    rect1.setRect(1, 1, 10, 10);
    rect2.setBottomLeft(1, 1);
    rect2.setTopRIght(11, 11);
    rect3.setPos(1, 1);
    rect3.setSize(10, 10);
    assert(rect1 == rect2 && rect2 == rect3 && rect1 == RRect(RPoint2(1, 1), RSize(10, 10)));
    rect1.setCenter(10, 10);
    rect2.move(4, 4);
    assert(rect1 == rect2 && rect1.topLeft() == RPoint2(5, 15) && rect1.bottomRight() == RPoint2(15, 5));
    assert(!rect1.contains(rect3) && rect1.contains(rect2) && rect1.contains(5, 5) && !rect1.contains(16, 15));
    assert(rect1.overlap(rect3));

    // RKeeper
    int n = 0;
    RKeeper<int> keeper;
    {
        RKeeper<int> keeper2(1, [&n](int kn){ n += kn; });
    }
    keeper.reset(2, [&n](int kn){ n += kn; });
    keeper.reset();
    assert(n == 3);

    // RNode ====================
    RNode node1("Node-rect1", &rect1), node2;
    assert(node1.status() == RNode::Status::Normal && node1.status() == node2.status());
    assert(node1.name() == RName("node_rect1"));
    assert(node2.name() == "node");
    assert(node1.holder<RRect>()->isValid());
    std::string order;
    node1.setStartFunc([&order]{ order += '1'; });
    node2.setStartFunc([&order]{ order += '2'; });
    node1.setUpdateFunc([&order](RRenderSys *){ order += '3'; });
    node2.setUpdateFunc([&order, &node2](RRenderSys *){ order += '4'; node2.breakLooping(); });
    node1.setFinishFunc([&order]{ order += '6'; });
    node2.setFinishFunc([&order]{ order += '5'; });
    node1.addChild(&node2);
    node1.exec();
    assert(order == "123456");
    RNode node3;
    node3.changeParent(&node1);
    node2.rename("Node1");
    assert(node3.name() == "node1");
    assert(node2.name() == "node11");
    node3.addChild(&node2);
    assert(node3.path() == "/node_rect1/node1");
    assert(node2.path() == "/node_rect1/node1/node11");
    assert(node3.node("Node11") == &node2);

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

    RScript scp(luaCode);
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
    RPack pck2;
    pck2.load((":/pck"));
    const RPack::FInfo *info = pck2.getFileInfo("mp3");
    mp3.load(info->data.get(), info->size);
    assert(mp3.isValid() && mp3.hz() == 48000 && mp3.channel() == 2);

    // RModelMat ====================
    float x = 3.f;
    float y = 12.f;
    float z = -2.f;
    float width = 12.f;
    float height = 16.f;
    RModelMat model(x, y, z, width, height);
    glm::mat4 mat(1);
    mat = glm::translate(mat, { x + width/2.f, y + height/2.f, z});
    mat = glm::scale(mat, { width, height, 0.f });
    assert(model.model() == mat);
    assert(!model.isFlipH() && !model.isFlipV());
    model.flipH();
    assert(model.isFlipH() && !model.isFlipV());
    model.flipV();
    assert(model.isFlipH() && model.isFlipV());
    model.move(3.f, -6.f, -4.f);
    assert(model.pos() == RPointF(6.f, 6.f, -6.f));
    assert(model.size() == RSizeF(12.f, 16.f));
    model.setPos(0, 0);
    assert(model.pos() == RPointF(0.f, 0.f, -6.f));
    model.setPos(2.f, 3.f);
    assert(model.right() == 14.f);
    assert(model.top() == 19.f);
    assert(model.left() == 2.f && model.x() == 2.f);
    assert(model.bottom() == 3.f && model.y() == 3.f);
    assert(model.centerX() == 8.f);
    assert(model.centerY() == 11.f);
    assert(model.center() == RRectF(2.f, 3.f, 12.f, 16.f).center());

    // REntity ====================
    REntity entity("test", nullptr);
    entity.addComp("rect", rect1);
    entity.addComp("size", size1);
    entity.getR<RRect>("rect").setRect(2, 2, 4, 4);
    assert(entity.get<RRect>("rect") == RRect(2, 2, 4, 4) && entity.get<RRect>("rect") != rect1);

    entity.addFunc("add1", std::function<int(REntity&, int)>([](REntity&, int n){ return ++n; }));
    entity.addFunc<int, int>("add2", [](REntity&, int n, int m){ return n + m; });
    entity.addFunc<int&, int>("add3", [](REntity&, int& n, int m){ n += m; });

    assert(entity.func<int>("add1", 5) ==  entity.func<int>("add2", 3, 3));
    n = 3;
    entity.func<void>("add3", n, 2);
    assert(n == 5);

    entity.addSignal<int&, int>("signal");
    entity.sigal<int&, int>("signal").connect(entity.addComp<RSlot>("slot", {}), [](int &n1, int n2){ n1 += n2; return true; });
    entity.sigal<int&, int>("signal").emit(n, 5);
    assert(n == 10);
    entity.removeSignal("signal");
    entity.removeComp("slot");

    entity.addChild("test2");
    assert(entity.child("test2").name() == "test2");

    assert(entity.isComp("rect") && entity.compSize() == 2);
    assert(entity.funcSize() == 3 && entity.isFunc("add3") && !entity.isComp("add3"));
    assert(entity.signalSize() == 0);
    assert(entity.childrenSize() == 1);

    rDebug << "End of test, No error occurred.";

    return 0;
}
