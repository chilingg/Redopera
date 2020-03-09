#include "RRole.h"

using namespace Redopera;

RRole::RRole():
    RRole(RSize(), RPoint(0))
{

}

RRole::RRole(int width, int height, int x, int y, int z, const RArea::Format &area):
    RRole(RSize(width, height), RPoint(x, y, z), area)
{

}

RRole::RRole(int width, int height, const RPoint &pos, const RArea::Format &area):
    RRole(RSize(width, height), pos, area)
{

}

RRole::RRole(const RSize &size, const RPoint &pos, const RArea::Format &area):
    RPlane(size, pos, area)
{
    renderControl = std::bind(&RRole::roleRenderControl, this, std::placeholders::_1, std::placeholders::_2);
}

RRole::RRole(const RRect &rect, int z, const RArea::Format &area):
    RRole(rect.size(), RPoint(rect.bottomLeft(), z), area)
{

}

RRole::RRole(const RRole &role):
    RPlane(role),
    frames_(role.frames_),
    indexs_(role.indexs_),
    current_(role.current_),
    velocity_(role.velocity_),
    interval_(role.interval_)
{

}

RRole::RRole(const RRole &&role):
    RPlane(std::move(role)),
    frames_(std::move(role.frames_)),
    indexs_(std::move(role.indexs_)),
    current_(std::move(role.current_)),
    velocity_(std::move(role.velocity_)),
    interval_(std::move(role.interval_))
{

}

size_t RRole::frameCount() const
{
    return frames_.size();
}

const std::string &RRole::current() const
{
    return current_;
}

const glm::vec2 &RRole::velocity() const
{
    return velocity_;
}

void RRole::setInterval(int interval)
{
    interval_ = interval;
}

void RRole::setElement(const std::string &name, std::initializer_list<int> indexs)
{
    indexs_[name] = indexs;
}

void RRole::setCurrentElements(const std::string &name)
{
    current_ = name;
    index_ = 0;
}

void RRole::replace(size_t index, RTexture tex)
{
    frames_.at(index) = tex;
}

void RRole::clear()
{
    std::vector<RTexture> temp;
    frames_.swap(temp);
}

void RRole::add(const RTexture &frame)
{
    frames_.push_back(frame);
}

void RRole::add(const std::vector<RTexture> &texs)
{
    frames_.insert(frames_.end(), texs.begin(), texs.end());
}

void RRole::add(std::initializer_list<RTexture> texs)
{
    frames_.insert(frames_.end(), texs.begin(), texs.end());
}

void RRole::setVelocity(const glm::vec2 &vlc)
{
    velocity_ = vlc;
}

void RRole::setVelocity(float x, float y)
{
    velocity_ = { x, y };
}

void RRole::setVelocityX(float x)
{
    velocity_.x = x;
}

void RRole::setVelocityY(float y)
{
    velocity_.y = y;
}

void RRole::giveVelocity(glm::vec2 vlc)
{
    velocity_ += vlc;
}

void RRole::giveVelocity(float x, float y)
{
    velocity_ += glm::vec2{ x, y };
}

void RRole::giveVelocityX(float x)
{
    velocity_.x += x;
}

void RRole::giveVelocityY(float y)
{
    velocity_.y += y;
}

void RRole::multipleVelocity(float n)
{
    velocity_ *= n;
}

void RRole::stopVelocity()
{
    velocity_ += glm::vec2{ 0, 0 };
}

void RRole::roleRenderControl(const RShaderProg &shaders, GLuint mLoc)
{
    if(delta_ < interval_)
        ++delta_;
    else {
        delta_ = 0;
        index_ = ++index_ % indexs_[current_].size();
    }

    frames_[indexs_[current_][index_]].bind();

    auto inter = shaders.useInterface();
    inter.setUniformMatrix(mLoc, modelMat());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
