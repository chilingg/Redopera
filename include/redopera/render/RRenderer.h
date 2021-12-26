#ifndef RRENDERER_H
#define RRENDERER_H

#include "RProgram.h"
#include "../RDefine.h"

namespace Redopera {

class RTexture;

class RRenderer
{
public:
    static RProgram createSimpleProgram();
    static void createPlaneVAO(GLuint &vao, GLuint &vbo);

    static void setBackgroundColor(RGBA rgba);

    RRenderer();
    RRenderer(const RProgram &program);

    ~RRenderer();

    RRenderer(const RRenderer&) = delete;
    RRenderer& operator=(const RRenderer&) = delete;

    const RProgram& program() const { return program_; }
    GLuint vao() const { return vao_; }

    void bindVAO() const;
    void unbindVAO() const;

    void draw() const;
    void drawLine() const;

    void setProgram(const RProgram &program);

protected:
    GLuint vao_ = 0, vbo_ = 0;
    RProgram program_;
};

} // ns Redopera

#endif // RRENDERER_H
