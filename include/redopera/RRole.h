#ifndef RROLE_H
#define RROLE_H

#include "RPlane.h"

namespace Redopera {

class RRole: public RPlane
{
public:
    RRole();
    RRole(int width, int height, int x, int y, int z = 0, const RArea::Format &area = RArea::getDefaultArea());
    RRole(int width, int height, const RPoint &pos, const RArea::Format &area = RArea::getDefaultArea());
    RRole(const RSize &size, const RPoint &pos, const RArea::Format &area = RArea::getDefaultArea());
    explicit RRole(const RRect &rect, int z = 0, const RArea::Format &area = RArea::getDefaultArea());
    RRole(const RRole &role);
    RRole(const RRole &&role);
    ~RRole() = default;

    size_t frameCount() const;
    const std::string& current() const;
    const glm::vec2& velocity() const;

    void setInterval(int interval = 20);
    void setElement(const std::string& name, std::initializer_list<int> indexs);
    void setCurrentElements(const std::string& name);
    void replace(size_t index, RTexture tex);
    void clear();

    void add(const RTexture &frame);
    void add(const std::vector<RTexture> &texs);
    void add(std::initializer_list<RTexture> texs);

    void setVelocity(const glm::vec2 &vlc);
    void setVelocity(float x, float y);
    void setVelocityX(float x);
    void setVelocityY(float y);

    void giveVelocity(glm::vec2 vlc);
    void giveVelocity(float x, float y);
    void giveVelocityX(float x);
    void giveVelocityY(float y);

    void multipleVelocity(float n);
    void stopVelocity();

protected:
    void roleRenderControl(const RShaderProg &shaders, GLuint mLoc);

private:
    std::vector<RTexture> frames_;
    std::map<std::string, std::vector<int>> indexs_;
    std::string current_;
    glm::vec2 velocity_;
    int interval_ = 20;

    int index_ = 0;
    int delta_ = 0;
};

}

#endif // RROLE_H
