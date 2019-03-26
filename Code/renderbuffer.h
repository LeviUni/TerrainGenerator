#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <QOpenGLFunctions_3_3_Core>

#include <memory>

class Renderbuffer : protected QOpenGLFunctions_3_3_Core {

public:
    Renderbuffer();
    ~Renderbuffer();

    inline void bind(GLenum target = GL_RENDERBUFFER) { glBindRenderbuffer(target, renderbufferID); }
    inline GLuint id() { return renderbufferID; }

private:
    GLuint renderbufferID;

};

typedef std::shared_ptr<Renderbuffer> RenderbufferPtr;

#endif
