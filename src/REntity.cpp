#include <REntity.h>

using namespace Redopera;

REntity::REntity(const RName &name, REntity *parent):
    name_(name),
    parent_(parent)
{

}

REntity::REntity(const REntity &entity):
    name_(entity.name_),
    comps_(entity.comps_),
    funcs_(entity.funcs_),
    signals_(entity.signals_),
    children_(entity.children_)
{

}

REntity::REntity(REntity &&entity):
    name_(entity.name_),
    comps_(std::move(entity.comps_)),
    funcs_(std::move(entity.funcs_)),
    signals_(std::move(entity.signals_)),
    children_(std::move(entity.children_))
{

}

REntity &REntity::operator=(const REntity &entity)
{
    name_ = entity.name_;
    comps_ = entity.comps_;
    funcs_ = entity.funcs_;
    signals_ = entity.signals_;
    children_ = entity.children_;

    return *this;
}

REntity &REntity::operator=(REntity &&entity)
{
    name_ = entity.name_;
    comps_ = std::move(entity.comps_);
    funcs_ = std::move(entity.funcs_);
    signals_ = std::move(entity.signals_);
    children_ = std::move(entity.children_);

    return *this;
}

bool REntity::isChildren(const RName &name) const
{
    auto it = std::find_if(children_.begin(), children_.end(), [name](const REntity &child){ return child.name_ == name; });
    return it != children_.end();
}

void REntity::moveAttrTo(REntity &entity)
{
    entity.name_ = name_;
    entity.comps_.swap(comps_);
    entity.funcs_.swap(funcs_);
    entity.signals_.swap(signals_);
    entity.children_.swap(children_);
}
