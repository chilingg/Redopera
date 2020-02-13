#ifndef RINFOWINDOW_H
#define RINFOWINDOW_H

#include "RWindow.h"
#include "RPlane.h"
#include "RTextbox.h"
#include "rsc/RCursor.h"

namespace Redopera {

class RInfoWIndow : public RWindow
{
public:
    constexpr static const char* defaultName() { return "InfoWindow"; }

    explicit RInfoWIndow(const RWindow::Format &format = {}, const std::string &name = defaultName());
    ~RInfoWIndow() override = default;

    void control() override;
    void translation(const TranslationInfo &info) override;

    void renderRscList();
    int updateTree(RController* node, RPoint2 pos);

protected:
    void startEvent(RStartEvent &event) override;
    void inputEvent(RInputEvent &event) override;

private:
    RTextsbxo rcTitle_;
    RPlane rcBack_;
    RPlane rcNumBack_;
    RTextsbxo rcCount_;
    RTextsbxo rcCountNum_;

    RTextsbxo rcID_;
    std::array<RTexture, 10> numbers_;
    std::map<RResource::Type, RTexture> rcIcons_;
    RPlane rcIcon_;
    RTextsbxo rcName_;

    RPlane rcScroll_;
    RPlane rcScrBack_;
    int sOff_ = 0;
    int itemNum_;
    int wheel_ = 0;

    RRect reArea_[2];

    RPlane trBack_;
    RTextsbxo trTitle_;
    RTextsbxo trView_;
    TreeList trList_;
    std::string trString_;

    RCursor cursor_;
    GLint pProLoc_ = -1, tProLoc_ = -1;
};

} // Redopera

#endif // RINFOWINDOW_H
