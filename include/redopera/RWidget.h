#ifndef RWIDGET_H
#define RWIDGET_H

#include <REntity.h>
#include <RRect.h>
#include <RPlane.h>
#include <RRenderSys.h>
#include <RTextsLoader.h>

namespace Redopera {

using Planes = std::vector<std::pair<RName, RPlane>>;

class RWidget
{
public:
    RWidget() = delete;

    static REntity& addAttribute(REntity &entity);

    static REntity& getLabel(REntity &entity);
    static REntity& getVerticalLayout(REntity &entity);
    static REntity& getHrizontalLayout(REntity &entity);
    static REntity& getCenterLayout(REntity &entity);
};

} // ns Redopera

#endif // RWIDGET_H
