#ifndef RWIDGET_H
#define RWIDGET_H

#include <REntity.h>
#include <RRect.h>
#include <RPlane.h>

namespace Redopera {

class RWidget
{
public:
    RWidget() = delete;

    static void horizontalLayout(RNode *sender, const RRect &info)
    {
        ;
    }

    static void verticalLayout(RNode *sender, const RRect &info)
    {
        ;
    }

    static void centerLayout(RNode *sender, const RRect &info)
    {
        ;
    }

    static REntity& addAttribute(REntity &entity)
    {
        entity.addComponent(Name::size, RSize(256, 256));
        entity.addComponent(Name::min_size, RSize(0, 0));
        entity.addComponent(Name::pos, RPoint2(0, 0));
        entity.addFunc(Name::rect, [&entity]{ return RRect(
                        entity.get<RPoint2>(Name::pos),
                        entity.get<RSize>(Name::size)); });

        entity.addComponent(Name::planes, std::vector<RPlane>());

        entity.addComponent(Name::layout, Name::vertical);

        return entity;
    }
};

} // ns Redopera

#endif // RWIDGET_H
