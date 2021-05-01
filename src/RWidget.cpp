#include <RWidget.h>
#include <RInput.h>

using namespace Redopera;

REntity &RWidget::addAttribute(REntity &entity)
{
    entity.addComponent(Name::type, std::vector<RName>{ Name::widget });

    entity.addComponent(Name::size, RSize(0, 0));
    entity.addComponent(Name::min_size, RSize(0, 0));
    entity.addComponent(Name::pos, RPoint2(0, 0));

    entity.addFunc(Name::rect, [&entity]{ return RRect(
                    entity.get<RPoint2>(Name::pos),
                    entity.get<RSize>(Name::size)); });
    entity.addFunc<const RRect&>("set_rect", [&entity](const RRect &rect)
    {
        entity.getR<RPoint2>(Name::pos).setPos(rect.pos());
        auto &size = entity.getR<RSize>(Name::size);
        size.setSize(std::max(entity.get<RSize>(Name::min_size).width(), rect.width()),
                                               std::max(entity.get<RSize>(Name::min_size).height(), rect.height()));

        if(entity.isComponent(Name::max_size))
            entity.getR<RSize>(Name::size).setSize(std::min(entity.get<RSize>(Name::max_size).width(), size.width()),
                                                   std::min(entity.get<RSize>(Name::max_size).height(), size.height()));
    });
    entity.addFunc<const RSize&>("set_size_and_min", [&entity](const RSize &size)
    {
        entity.getR<RSize>(Name::size) = size;
        entity.getR<RSize>(Name::min_size) = size;
    });

    return entity;
}

REntity &RWidget::getLabel(REntity &entity)
{
    addAttribute(entity);
    entity.getR<std::vector<RName>>(Name::type).push_back(Name::label);
    entity.getR<RSize>(Name::size).setSize(256, 256);
    entity.getR<RSize>(Name::min_size).setSize(20, 20);

    entity.addComponent(Name::loader, RTextsLoader("Input text", entity.get<RSize>(Name::size)));
    entity.addComponent(Name::content, RPlane(RModelMat(entity.func<RRect>(Name::rect)), entity.get<RTextsLoader>(Name::loader).texture()));

    entity.addFunc<const RText&>("set_text", [&entity](const RText &text)
    {
        RTextsLoader &loader = entity.getR<RTextsLoader>(Name::loader);
        loader.setTexts(text);
        entity.getR<RPlane>(Name::content).setTexture(loader.texture());
    });

    entity.addFunc<const RImage&>("set_image", [&entity](const RImage &img)
    {
        auto &content = entity.getR<RPlane>(Name::content);
        content.setTexture(img);

        auto &size = entity.get<RSize>(Name::size);
        if(img.width() > img.height())
        {
            content.setHeight(img.height() * (static_cast<float>(size.width()) / img.width()));
        }
        else
        {
            content.setWidth(img.width() * (static_cast<float>(size.height()) / img.height()));
        }

        entity.getR<RTextsLoader>(Name::loader).setTexts("");
    });

    entity.addFunc<const RTexture&>("set_texture", [&entity](const RTexture &tex)
    {
        auto &content = entity.getR<RPlane>(Name::content);
        content.setTexture(tex);

        auto &size = entity.get<RSize>(Name::size);
        if(tex.width() > tex.height())
        {
            content.setHeight(tex.height() * (static_cast<float>(size.width()) / tex.width()));
        }
        else
        {
            content.setWidth(tex.width() * (static_cast<float>(size.height()) / tex.height()));
        }

        entity.getR<RTextsLoader>(Name::loader).setTexts("");
    });

    entity.node.setUpdateFunc([&entity](RRenderSys *sys)
    {
        auto &content = entity.get<RPlane>(Name::content);
        RRPI rpi = sys->shaders().use();

        sys->usingHueOut();
        sys->renderLine(content.rect());
        sys->usingTexColorOut();

        if(content.channel() == 1)
        {
            if(entity.isComponent(Name::font_color))
                sys->setHue(entity.get<RColor>(Name::font_color));
            else
                sys->setHue(glm::vec4(1.f, 1.f, 1.f, 1.f));

            sys->usingSingleTexOut();
            *sys << content;

            sys->setHue(glm::vec4(1.f, 1.f, 1.f, 1.f));
            sys->usingTexColorOut();
        }
        else
            *sys << content;
    });

    entity.node.setTransformFunc([&entity](RNode *, const RRect &info)
    {
        entity.func<void>("set_rect", info);
        RRect rect = entity.func<RRect>(Name::rect);

        auto &loader = entity.getR<RTextsLoader>(Name::loader);
        if(loader.texts().size())
        {
            loader.setSize(rect.size());
            auto &content = entity.getR<RPlane>(Name::content);
            content.setTexture(loader.texture());
            content.setModel(rect);
        }
        else
        {
            auto &content = entity.getR<RPlane>(Name::content);

            if(static_cast<float>(rect.width()) / rect.height() > content.width() / content.height())
                content.setSize((rect.height() / content.height()) * content.width(), rect.height());
            else
                content.setSize(rect.width(), (rect.width() / content.width()) * content.height());

            content.setCenter(rect.center());
        }
    });

    return entity;
}

REntity &RWidget::getVerticalLayout(REntity &entity)
{
    addAttribute(entity);
    entity.getR<std::vector<RName>>(Name::type).push_back(Name::layout);

    entity.addFunc<REntity&>("add_widget", [&entity](REntity &widget)
    {
        RRect rect = entity.func<RRect>(Name::rect);
        rect.setWidth(std::max(rect.width(), widget.get<RSize>(Name::size).width()));
        entity.node.addChild(&widget.node);

        entity.node.transform(&entity.node, rect);
    });

    entity.node.setTransformFunc([&entity](RNode *sender, const RRect &info)
    {
        entity.func<void>("set_rect", info);
        RRect rect = entity.func<RRect>(Name::rect);

        int value = 0;
        RSize size = rect.size();
        std::for_each(entity.node.children().begin(), entity.node.children().end(), [&value, &size](RNode *node)
        {
            REntity *e = node->holder<REntity>();
            value += e->get<RSize>(Name::size).height();
            size.setWidth(std::max(size.width(), e->get<RSize>(Name::min_size).width()));
        });

        if(value)
        {
            if(value < rect.height())
            {
                entity.node.children().back()->holder<REntity>()->getR<RSize>(Name::size).expand(0, rect.height() - value);
            }
            else if(value > rect.height())
            {
                int offset = value - rect.height();

                for(auto it = entity.node.children().rbegin(); it != entity.node.children().rend(); ++ it)
                {
                    REntity *e = (*it)->holder<REntity>();
                    if(e->get<RSize>(Name::size).height() - e->get<RSize>(Name::min_size).height() >= offset)
                    {
                        e->getR<RSize>(Name::size).expand(0, -offset);
                        break;
                    }
                    else
                    {
                        offset -= e->get<RSize>(Name::size).height() - e->get<RSize>(Name::min_size).height();
                        e->getR<RSize>(Name::size).setHeight(e->get<RSize>(Name::min_size).height());
                    }
                }
            }
        }

        int y = rect.top();
        std::for_each(entity.node.children().begin(), entity.node.children().end(), [&sender, &size, &rect, &y](RNode *node)
        {
            y -= node->holder<REntity>()->get<RSize>(Name::size).height();
            node->transform(sender, RRect(rect.x(), y, size.width(), node->holder<REntity>()->get<RSize>(Name::size).height()));
            size.expand(0, node->holder<REntity>()->get<RSize>(Name::size).height());
        });

        entity.getR<RSize>(Name::size) = size;
        entity.getR<RPoint2>(Name::pos).setPos(rect.x(), y);
    });

    entity.node.setProcessFunc([&entity](RNode *sender, RNode::Instructs *ins)
    {
        std::for_each(entity.node.children().begin(), entity.node.children().end(), [sender, ins](RNode *node)
        {
            REntity *e = node->holder<REntity>();
            if(e->func<RRect>(Name::rect).contains(RInput::cursorPos()))
                node->process(sender, ins);
        });
    });

    return entity;
}

REntity &RWidget::getHrizontalLayout(REntity &entity)
{
    addAttribute(entity);
    entity.getR<std::vector<RName>>(Name::type).push_back(Name::layout);

    entity.addFunc<REntity&>("add_widget", [&entity](REntity &widget)
    {
        RRect rect = entity.func<RRect>(Name::rect);
        rect.setHeight(std::max(rect.height(), widget.get<RSize>(Name::size).height()));

        entity.node.addChild(&widget.node);
        entity.node.transform(&entity.node, rect);
    });

    entity.node.setTransformFunc([&entity](RNode *sender, const RRect &info)
    {
        entity.func<void>("set_rect", info);
        RRect rect = entity.func<RRect>(Name::rect);

        int value = 0;
        RSize size = rect.size();
        std::for_each(entity.node.children().begin(), entity.node.children().end(), [&value, &size](RNode *node)
        {
            REntity *e = node->holder<REntity>();
            value += e->get<RSize>(Name::size).width();
            size.setHeight(std::max(size.height(), e->get<RSize>(Name::min_size).height()));
        });

        if(value)
        {
            if(value < rect.width())
            {
                entity.node.children().back()->holder<REntity>()->getR<RSize>(Name::size).expand(rect.width() - value, 0);
            }
            else if(value > rect.width())
            {
                int offset = value - rect.width();

                for(auto it = entity.node.children().rbegin(); it != entity.node.children().rend(); ++it)
                {
                    REntity *e = (*it)->holder<REntity>();
                    if(e->get<RSize>(Name::size).width() - e->get<RSize>(Name::min_size).width() >= offset)
                    {
                        e->getR<RSize>(Name::size).expand(-offset, 0);
                        break;
                    }
                    else
                    {
                        offset -= e->get<RSize>(Name::size).width() - e->get<RSize>(Name::min_size).width();
                        e->getR<RSize>(Name::size).setWidth(e->get<RSize>(Name::min_size).width());
                    }
                }
            }
        }

        int x = rect.left();
        std::for_each(entity.node.children().begin(), entity.node.children().end(), [&sender, &size, &rect, &x](RNode *node)
        {
            node->transform(sender, RRect(x, rect.y(), node->holder<REntity>()->get<RSize>(Name::size).width(), size.height()));
            size.expand(node->holder<REntity>()->get<RSize>(Name::size).width(), 0);
            x += node->holder<REntity>()->get<RSize>(Name::size).width();
        });

        entity.getR<RSize>(Name::size) = size;
        entity.getR<RPoint2>(Name::pos).setPos(rect.pos());
    });

    entity.node.setProcessFunc([&entity](RNode *sender, RNode::Instructs *ins)
    {
        std::for_each(entity.node.children().begin(), entity.node.children().end(), [sender, ins](RNode *node)
        {
            REntity *e = node->holder<REntity>();
            if(e->func<RRect>(Name::rect).contains(RInput::cursorPos()))
                node->process(sender, ins);
        });
    });

    return entity;
}

REntity &RWidget::getCenterLayout(REntity &entity)
{
    addAttribute(entity);
    entity.getR<std::vector<RName>>(Name::type).push_back(Name::layout);

    getVerticalLayout(entity.addEntity(Name::left)).node.changeParent(&entity.node);
    getVerticalLayout(entity.addEntity(Name::right)).node.changeParent(&entity.node);
    getHrizontalLayout(entity.addEntity(Name::bottom)).node.changeParent(&entity.node);
    getHrizontalLayout(entity.addEntity(Name::top)).node.changeParent(&entity.node);

    entity.node.setTransformFunc([&entity](RNode *sender, const RRect &info)
    {
        int lw = 0;
        int rw = 0;
        int th = 0;
        int bh = 0;

        RNode *left = entity.node.child(Name::left);
        if(left)
            lw = left->holder<REntity>()->get<RSize>(Name::size).width();
        RNode *right = entity.node.child(Name::right);
        if(right)
            rw = right->holder<REntity>()->get<RSize>(Name::size).width();
        RNode *top = entity.node.child(Name::top);
        if(top)
            th = top->holder<REntity>()->get<RSize>(Name::size).height();
        RNode *bottom = entity.node.child(Name::bottom);
        if(bottom)
            bh = bottom->holder<REntity>()->get<RSize>(Name::size).height();

        if(left)
            left->transform(sender, RRect(info.left(),
                                          info.bottom() + bh,
                                          lw,
                                          info.height() - bh - th));
        if(right)
            right->transform(sender, RRect(info.right() - rw,
                                           info.bottom() + bh,
                                           rw,
                                           info.height() - bh - th));

        if(top)
            top->transform(sender, RRect(info.left(),
                                         info.top() - th,
                                         info.width(),
                                         th));
        if(bottom)
            bottom->transform(sender, RRect(info.left(),
                                            info.bottom(),
                                            info.width(),
                                            bh));

        if(entity.node.isChild(Name::main))
            entity.node.child(Name::main)->transform(sender, RRect(info.left() + lw,
                                                                   info.bottom() + bh,
                                                                   info.width() - lw - rw,
                                                                   info.height() - th - bh));

        entity.func<void>("set_rect", info);
    });

    entity.node.setProcessFunc([&entity](RNode *sender, RNode::Instructs *ins)
    {
        std::for_each(entity.node.children().begin(), entity.node.children().end(), [sender, ins](RNode *node)
        {
            REntity *e = node->holder<REntity>();
            if(e->func<RRect>(Name::rect).contains(RInput::cursorPos()))
                node->process(sender, ins);
        });
    });

    return entity;
}
