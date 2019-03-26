#ifndef TEXTURE_H
#define TEXTURE_H

#include <QOpenGLFunctions_3_3_Core>

#include <memory>
#include <iostream>

class TextureBase : protected QOpenGLFunctions_3_3_Core {

protected:
    TextureBase();
    virtual ~TextureBase();

public:
    inline GLuint id() { return textureID; }
    inline void bind(GLenum target) { glBindTexture(target, textureID); }

protected:
    GLuint textureID;

};

class Texture : public TextureBase {

public:
    Texture();
    ~Texture();

};

class TextureProxy : public TextureBase {

public:
    TextureProxy(GLuint id);
    ~TextureProxy();

};

typedef std::shared_ptr<TextureBase> TexturePtr;

#endif
