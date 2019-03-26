#include "texture.h"

#include <iostream>

TextureBase::TextureBase() : textureID(0) {
    initializeOpenGLFunctions();
}

TextureBase::~TextureBase() {}

Texture::Texture() {
    glGenTextures(1, &textureID);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureID);
}

TextureProxy::TextureProxy(GLuint id) {
    textureID = id;
}

TextureProxy::~TextureProxy() {}
