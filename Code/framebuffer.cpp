#include "framebuffer.h"

#include <iostream>

Framebuffer::Framebuffer(GLsizei width, GLsizei height) :
        width(width), height(height), colorAttachments(0), created(false) {

    initializeOpenGLFunctions();

    if (width < 1) {
        this->width = 1;
    }

    if (height < 1) {
        this->height = 1;
    }

    glGenFramebuffers(1, &framebufferID);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &framebufferID);
}

FramebufferPtr Framebuffer::getResizedCopy(GLsizei width, GLsizei height) {
    FramebufferPtr newBuffer(new Framebuffer(width, height));

    for (auto t : textureAddInfo) {
        newBuffer->addTexture(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t));
    }

    for (auto r : renderbufferAddInfo) {
        newBuffer->addRenderbuffer(std::get<0>(r), std::get<1>(r));
    }

    if (created) {
        newBuffer->create();
    }

    return newBuffer;
}

void Framebuffer::addTexture(GLint internalFormat, GLenum format, GLenum type, GLenum attachment) {
    assert(!created);

    // save the call parameters, so resizing works
    textureAddInfo.push_back(std::tuple<GLint, GLenum, GLenum, GLenum>{ internalFormat, format, type, attachment });

    // bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

    // generate and bind the texture
    TexturePtr texture(new Texture);
    texture->bind(GL_TEXTURE_2D);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // upload empty texture data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);

    // add the texture to the framebuffer
    if (attachment == 0) {
        attachment = GL_COLOR_ATTACHMENT0 + colorAttachments;
        colorAttachments++;
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->id(), 0);

    textures.push_back(texture);
}

void Framebuffer::addRenderbuffer(GLenum internalFormat, GLenum attachment) {
    assert(!created);

    // save the call paramters so resizing works
    renderbufferAddInfo.push_back(std::tuple<GLenum, GLenum>{ internalFormat, attachment });

    // bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

    // generate and bind renderbuffer
    RenderbufferPtr renderbuffer(new Renderbuffer);
    renderbuffer->bind();

    // initialize renderbuffer storage
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);

    // add the renderbuffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer->id());

    renderbuffers.push_back(renderbuffer);
}

void Framebuffer::create() {
    if (created) {
        return;
    }

    // collect the attachments
    std::vector<GLuint> attachments;
    for (unsigned int i = 0; i < colorAttachments; i++) {
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }

    // bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

    // add the attachments
    glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());

    // check if the framebuffer is finished
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    created = true;
}
