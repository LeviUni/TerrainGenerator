#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <QOpenGLFunctions_3_3_Core>

#include <memory>
#include <vector>
#include <cassert>

#include "texture.h"
#include "renderbuffer.h"

class Framebuffer;
typedef std::shared_ptr<Framebuffer> FramebufferPtr;

class Framebuffer : protected QOpenGLFunctions_3_3_Core {

public:
    Framebuffer(GLsizei width, GLsizei height);
    ~Framebuffer();

    FramebufferPtr getResizedCopy(GLsizei width, GLsizei height);

    inline void bind() {
        assert(created);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
    }

    void addTexture(GLint internalFormat, GLenum format, GLenum type, GLenum attachment = 0);
    void addRenderbuffer(GLenum internalFormat, GLenum attachment);

    void create();

    const std::vector<TexturePtr>& getTextures() const { return textures; }
    const std::vector<RenderbufferPtr>& getRenderbuffers() const { return renderbuffers; }

private:
    GLuint framebufferID;

    GLsizei width;
    GLsizei height;

    std::vector<TexturePtr> textures;
    std::vector< std::tuple<GLint, GLenum, GLenum, GLenum> > textureAddInfo;
    GLuint colorAttachments;

    std::vector<RenderbufferPtr> renderbuffers;
    std::vector< std::tuple<GLenum, GLenum> > renderbufferAddInfo;

    bool created;
};

typedef std::shared_ptr<Framebuffer> FramebufferPtr;

#endif
