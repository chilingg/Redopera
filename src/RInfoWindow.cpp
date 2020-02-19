#include "RInfoWindow.h"
#include "RDebug.h"
#include "rsc/RPack.h"

using namespace Redopera;

namespace  {
constexpr int LIST_W = 200;
constexpr int TITLE_H = 26;
constexpr int LIST_LEFT_ALIGN = 8;
constexpr int SCROLL_BAR_W = 8;

const RColor COLOR_ZERO(20, 20, 25);
const RColor COLOR_ONE(30, 30, 40);
const RColor COLOR_TOW(45, 45, 55);
const RColor COLOR_THREE(55, 55, 65);
const RColor COLOR_FOUR(65, 65, 80);
const RColor FONT_COLOR(210, 210, 230);
}

RInfoWIndow::RInfoWIndow(const RWindow::Format &format, const std::string &name):
    RWindow(format, nullptr, name)
{

}

void RInfoWIndow::control()
{
    renderRscList();

    rcTitle_.render();
    rcCount_.render();
    rcCountNum_.render();
    rcScrBack_.render();
    rcScroll_.render();

    if(!queryTreeList(trList_))
    {
        trString_.clear();
        for(auto &node : trList_)
            trString_ += node.second + '\n';
        trView_.setTexts(trString_);
    }
    trBack_.render();
    trTitle_.render();
    trView_.render();
}

void RInfoWIndow::renderRscList()
{
    if(timer_.elapsed() > 1000)
    {
        rcList_ = RResource::queryResourceList();
        timer_.start();
    }

    int n = rcList_.size();
    rcCountNum_.setTexts(std::to_wstring(n));
    rcScroll_.setHeight(n * LIST_W);

    int listLen = n * TITLE_H;
    if(listLen > rcScrBack_.height())
    {
        sOff_ = glm::clamp(sOff_, 0, listLen - rcScrBack_.height());

        double ratio = rcScrBack_.height()*1.0 / listLen;
        rcScroll_.setHeight(rcScrBack_.height() * ratio);
        ratio = sOff_*1.0 / listLen;
        rcScroll_.setY(height() - TITLE_H - rcScroll_.height());
        rcScroll_.move(0, -rcScrBack_.height() * ratio);
    }
    else {
        rcScroll_.setHeight(rcScrBack_.height());
        rcScroll_.setY(TITLE_H);
        sOff_ = 0;
    }

    rcID_.setY(rcTitle_.y() - TITLE_H + sOff_);
    rcIcon_.setY(rcID_.y());
    rcName_.setY(rcID_.y());

    rcBack_.render();
    rcNumBack_.render();

    for(auto &rc : rcList_)
    {
        if(rcID_.y() < 0)
            break;
        else if(rcID_.y() > height() - TITLE_H)
        {
            rcID_.move(0, -TITLE_H);
            rcIcon_.move(0, -TITLE_H);
            rcName_.move(0, -TITLE_H);
            continue;
        }

        // 百位
        int n = rc.first / 100 % 10;
        if(n)
        {
            rcID_.setTexture(numbers_[n]);
            rcID_.render();
        }
        // 十位
        rcID_.move(7, 0);
        n = rc.first / 10 % 10;
        if(n || rc.first > 99)
        {
            rcID_.setTexture(numbers_[n]);
            rcID_.render();
        }
        // 个位
        rcID_.move(7, 0);
        n = rc.first % 10;
        rcID_.setTexture(numbers_[n]);
        rcID_.render();

        rcIcon_.setTexture(rcIcons_[rc.second.type]);
        rcIcon_.render();
        //rcIcon_.edging(RColor(30, 30, 40));
        rcName_.setTexts(rc.second.name);
        rcName_.render();

        rcID_.setPos(LIST_LEFT_ALIGN, rcID_.y() - TITLE_H);
        rcIcon_.move(0, -TITLE_H);
        rcName_.move(0, -TITLE_H);
    }
}

void RInfoWIndow::startEvent(RStartEvent &)
{
    rcTitle_.setTextureName("__rcTitle");
    rcTitle_.setTexts(L"Resource List");
    rcTitle_.setSize(LIST_W, TITLE_H);
    rcTitle_.setFontSize(13);
    rcTitle_.setFontColor(FONT_COLOR);
    rcTitle_.setPadding(0, 0, LIST_LEFT_ALIGN, 0);
    rcTitle_.setBackColor(COLOR_ONE);
    rcTitle_.setAlign(RArea::Align::Mind, RArea::Align::Left);

    rcBack_.rename("__rcBack");
    rcBack_.setWidth(LIST_W);
    rcBack_.setPos(0, 0);
    rcBack_.setColorTexture(COLOR_TOW);

    rcCount_.setTextureName("__rcCount");
    rcCount_ = rcTitle_;
    rcCount_.setPos(0, 0);
    rcCount_.setTexts(L"Count:");
    rcCountNum_ = rcCount_;
    rcCountNum_.setBackColor(0x0);
    rcCountNum_.setTexts(L"0");
    rcCountNum_.setX(51);

    rcID_.setFontSize(13);
    rcID_.setX(LIST_LEFT_ALIGN);
    rcID_.setSize(15, TITLE_H);
    rcID_.setAlign(RArea::Align::Mind, RArea::Align::Left);
    rcID_.setFontColor(FONT_COLOR);
    for(int i = 0; i < 10; ++i)
    {
        rcID_.setTexts(std::to_wstring(i));
        rcID_.resetingNow();
        numbers_[i] = rcID_.textTexture();
        numbers_[i].rename("__rcNum " + std::to_string(i));
    }

    RPack pack(":/icon/info-icons");
    const RPack::FInfo *info;

    info = pack.getFileInfo(":/cursor-24.png");
    rcIcons_[RResource::Type::Cursor] = RTexture(RImage(info->data.get(), info->size, "", true), "__cursor-icon");
    info = pack.getFileInfo(":/font-24.png");
    rcIcons_[RResource::Type::Font] = RTexture(RImage(info->data.get(), info->size, "", true), "__font-icon");
    info = pack.getFileInfo(":/image-24.png");
    rcIcons_[RResource::Type::Image] = RTexture(RImage(info->data.get(), info->size, "", true), "__image-icon");
    info = pack.getFileInfo(":/script-24.png");
    rcIcons_[RResource::Type::Script] = RTexture(RImage(info->data.get(), info->size, "", true), "__script-icon");
    info = pack.getFileInfo(":/mp3-24.png");
    rcIcons_[RResource::Type::Mp3] = RTexture(RImage(info->data.get(), info->size, "", true), "__mp3-icon");
    info = pack.getFileInfo(":/pack-24.png");
    rcIcons_[RResource::Type::Pack] = RTexture(RImage(info->data.get(), info->size, "", true), "__pack-icon");
    info = pack.getFileInfo(":/shader-24.png");
    rcIcons_[RResource::Type::Shader] = RTexture(RImage(info->data.get(), info->size, "", true), "__shader-icon");
    info = pack.getFileInfo(":/shaderProg-24.png");
    rcIcons_[RResource::Type::ShaderProg] = RTexture(RImage(info->data.get(), info->size, "", true), "__shaderProg-icon");
    info = pack.getFileInfo(":/texture-24.png");
    rcIcons_[RResource::Type::Texture] = RTexture(RImage(info->data.get(), info->size, "", true), "__texture-icon");
    pack.release();

    rcIcon_.rename("__rcIcon");
    rcIcon_.setSize(TITLE_H, TITLE_H);
    rcIcon_.setX(LIST_LEFT_ALIGN + 30);

    rcNumBack_.rename("__rcNumBack");
    rcNumBack_.setPos(0, 0);
    rcNumBack_.setWidth(rcIcon_.x());
    rcNumBack_.setColorTexture(COLOR_THREE);

    rcName_ = rcID_;
    rcName_.setTextureName("__rcName");
    rcName_.setX(rcIcon_.x() + TITLE_H);
    rcName_.setWidth(LIST_W - rcName_.x());
    rcName_.setEllipsis(false);

    rcScroll_.rename("__rcScroll");
    rcScroll_.setWidth(SCROLL_BAR_W);
    rcScroll_.setColorTexture(COLOR_FOUR);
    rcScroll_.setX(rcBack_.width() - SCROLL_BAR_W);
    rcScroll_.setMinSize(0, 10);
    rcScrBack_.rename("__rcScrBack");
    rcScrBack_.setWidth(SCROLL_BAR_W);
    rcScrBack_.setColorTexture(COLOR_ZERO);
    rcScrBack_.setPos(rcScroll_.x(), TITLE_H);

    reArea_[0] = rcScrBack_.rect();
    reArea_[0].move(reArea_[0].width()/2, 0);

    trBack_.rename("__trBack");
    trBack_.setPos(rcBack_.width() + 1, 0);
    trBack_.setColorTexture(COLOR_TOW);

    trTitle_ = rcTitle_;
    trTitle_.setTextureName("__trTile");
    trTitle_.setX(trBack_.x());
    trTitle_.setTexts(L"Tree View");

    trView_.setTextureName("__trView");
    trView_.setFontSize(13);
    trView_.setlineSpacing(1.8f);
    trView_.setFontColor(FONT_COLOR);
    trView_.setPadding(LIST_LEFT_ALIGN, 0, LIST_LEFT_ALIGN, 0);
    trView_.setPos(trTitle_.x(), 0);

    pProLoc_ = RPlane::planeShader().getUniformLocation("projection");
    tProLoc_ = RTextsbxo::textboxShader().getUniformLocation("projection");

    rolled.connect([this](int direct){ wheel_ = direct; return true; });

    translation({ this, size(), RPoint(0) });

    cursor_.rename("__info-cursor");
}

void RInfoWIndow::translation(const RController::TranslationInfo &info)
{
    rcTitle_.setPos(0, info.size.height() - rcTitle_.height());
    rcBack_.setHeight(info.size.height() - TITLE_H);
    rcNumBack_.setHeight(rcBack_.height());
    rcScrBack_.setHeight(rcBack_.height() - TITLE_H);
    reArea_[0].setHeight(rcScrBack_.height());
    trBack_.setSize(info.size.width() - rcBack_.width() - 1, info.size.height());
    trTitle_.setWidth(trBack_.width());
    trTitle_.setY(trBack_.height() - TITLE_H);
    trView_.setSize(trBack_.width(), trBack_.height() - TITLE_H);

    {
    RInterface interface = RPlane::planeShader().useInterface();
    interface.setViewprot(pProLoc_, 0, width(), 0, height());
    }

    RInterface interface = RTextsbxo::textboxShader().useInterface();
    interface.setViewprot(tProLoc_, 0, width(), 0, height());
}

void RInfoWIndow::inputEvent(RInputEvent &event)
{
    RCursor::Shape shape = RCursor::Shape::Arrow;

    if(wheel_)
    {
        if(rcBack_.rect().contains(event.pos()))
            sOff_ += TITLE_H/2.0 * -wheel_;
        wheel_ = 0;
    }

    if(reArea_[0].contains(event.pos()))
    {
        shape = RCursor::Shape::Hresize;

        if(event.press(MouseButtons::MOUSE_BUTTON_LEFT))
        {
            std::swap(reArea_[0], reArea_[1]);
            reArea_[0].set(size(), RPoint2(0, 0));
        }

        if(event.status(MouseButtons::MOUSE_BUTTON_LEFT) == ButtonAction::PRESS && event.pos().x() > LIST_W/2)
        {
            reArea_[0].setCenter(event.pos());
            rcTitle_.setWidth(event.pos().x() + SCROLL_BAR_W);
            rcCount_.setWidth(rcTitle_.width());
            rcBack_.setWidth(rcTitle_.width());
            rcName_.setWidth(rcTitle_.width() - rcName_.x());
            rcScroll_.setX(rcBack_.width() - rcScroll_.width());
            rcScrBack_.setX(rcScroll_.x());
            trBack_.setX(rcBack_.width() + 1);
            trBack_.setWidth(width() - rcBack_.width() + 1);
            trTitle_.setX(trBack_.x());
            trTitle_.setWidth(trBack_.width());
            trView_.setX(trBack_.x());
            trView_.setWidth(trBack_.width());
        }

        if(event.release(MouseButtons::MOUSE_BUTTON_LEFT))
            std::swap(reArea_[0], reArea_[1]);
    }

    if(event.press(Keys::KEY_ESCAPE))
        breakLoop();

    if(cursor_.shape() != shape)
    {
        cursor_.load(shape);
        setCursor(&cursor_);
    }
}
