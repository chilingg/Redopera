#include <RWindow.h>
#include <RGame.h>
#include <RWidget.h>
#include <RDebug.h>

using namespace Redopera;

int w = 800;
int h = 400;

int main()
{
    RGame game;
    RWindow::Format format;
    format.background = 0x101018ff;
    format.debug = false;
    RWindow window(w, h, "Layout", format);

    REntity layout("layout");
    RWidget::getCenterLayout(layout);

    REntity &limg = layout.entity(Name::left).addEntity("img1");
    RWidget::getLabel(limg);
    limg.func<void>("set_image", RImage::redoperaIcon());
    layout.entity(Name::left).func<void>("add_widget", (limg));
    REntity &ltext1 = layout.entity(Name::left).addEntity("text1");
    RWidget::getLabel(ltext1);
    layout.entity(Name::left).func<void>("add_widget", (ltext1));
    REntity &ltext2 = layout.entity(Name::left).addEntity("text2");
    RWidget::getLabel(ltext2);
    ltext2.func<void, const RText&>("set_text", "qwequihweihbaskj\nwetrwerterwsehybweuiotwerjkdfs");
    layout.entity(Name::left).func<void>("add_widget", (ltext2));

    REntity &rtext1 = layout.entity(Name::right).addEntity("text1");
    RWidget::getLabel(rtext1);
    layout.entity(Name::right).func<void>("add_widget", (rtext1));
    REntity &rimg = layout.entity(Name::right).addEntity("img1");
    RWidget::getLabel(rimg);
    rimg.func<void>("set_image", RImage::redoperaIcon());
    layout.entity(Name::right).func<void>("add_widget", (rimg));

    RSize size(64, 24);
    REntity &ttext1 = layout.entity(Name::top).addEntity("text1");
    RWidget::getLabel(ttext1);
    ttext1.func<void, const RSize&>("set_size_and_min", size);
    ttext1.func<void, const RText&>("set_text", "text1");
    layout.entity(Name::top).func<void>("add_widget", (ttext1));
    REntity &ttext2 = layout.entity(Name::top).addEntity("text2");
    RWidget::getLabel(ttext2);
    ttext2.func<void, const RSize&>("set_size_and_min", size);
    ttext2.func<void, const RText&>("set_text", "text2");
    layout.entity(Name::top).func<void>("add_widget", (ttext2));
    REntity &ttext3 = layout.entity(Name::top).addEntity("text3");
    RWidget::getLabel(ttext3);
    ttext3.func<void, const RSize&>("set_size_and_min", size);
    ttext3.func<void, const RText&>("set_text", "text3");
    layout.entity(Name::top).func<void>("add_widget", (ttext3));
    REntity &empty = layout.entity(Name::top).addEntity("empty");
    RWidget::getLabel(empty);
    empty.func<void, const RTexture&>("set_texture", RTexture::createTransTex());
    empty.getR<RSize>(Name::size).setSize(size);
    empty.getR<RSize>(Name::min_size).setSize(0, 0);
    layout.entity(Name::top).func<void>("add_widget", (empty));

    REntity &btext1 = layout.entity(Name::bottom).addEntity("text1");
    RWidget::getLabel(btext1);
    btext1.func<void, const RSize&>("set_size_and_min", size);
    btext1.func<void, const RText&>("set_text", "status bar");
    btext1.getR<RTextsLoader>(Name::loader).setAlign(RTextsLoader::Align::Mind, RTextsLoader::Align::Right);
    layout.entity(Name::bottom).func<void>("add_widget", (btext1));

    window.node.addChild(&layout.node);
    window.node.setTransformFunc([&window](RNode *sender, const RRect &info)
    {
        window.renderSys()->setViewport(0, info.width(), 0, info.height());
        window.node.transformEventToChild(sender, info);
    });

    window.show();
    return window.node.exec();
}
