#include "material.h"

#include <QImage>
#include <QDebug>

Material::Material(GLfloat Ka, GLfloat Kd, GLfloat Ks, GLuint n) :
            Ka(Ka), Kd(Kd), Ks(Ks), n(n), customShader(nullptr) {

    initializeOpenGLFunctions();
}

Material::~Material() {
    qDebug() << "Material destructor";
}

void Material::bindTextures(GLuint startSlot) {
    for (auto t : textures) {
        glActiveTexture(GL_TEXTURE0 + t.first + startSlot);
        t.second->bind(GL_TEXTURE_2D);
    }
}

QVector4D Material::getMaterialVector() const {
    return QVector4D(Ka, Kd, Ks, n);
}

void Material::addTexture(unsigned int slot, const std::string &location) {
    // generate the texture
    TexturePtr texture(new Texture);

    // initialize the texture
    initializeTexture(texture, location);

    // save the texture
    textures[slot] = texture;
}

void Material::addTexture(unsigned int slot, GLuint textureID) {
    textures[slot] = TexturePtr(new TextureProxy(textureID));
}

void Material::initializeTexture(const TexturePtr& texture, const std::string& location) {
    // bind the texture
    texture->bind(GL_TEXTURE_2D);

    // get the image data
    QImage image(location.c_str());
    QVector<quint8> bytes = imageToBytes(image);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // apply anisotropic filtering
    GLfloat f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, f);

    // upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes.data());
}

/**
 * Code from Nestor
 */
QVector<quint8> Material::imageToBytes(const QImage &image) {
    // needed since (0,0) is bottom left in OpenGL
    QImage im = image.mirrored();
    QVector<quint8> pixelData;
    pixelData.reserve(im.width()*im.height()*4);

    for (int i = 0; i != im.height(); ++i) {
        for (int j = 0; j != im.width(); ++j) {
            QRgb pixel = im.pixel(j,i);

            // pixel is of format #AARRGGBB (in hexadecimal notation)
            // so with bitshifting and binary AND you can get
            // the values of the different components
            quint8 r = static_cast<quint8>((pixel >> 16) & 0xFF); // Red component
            quint8 g = static_cast<quint8>((pixel >>  8) & 0xFF); // Green component
            quint8 b = static_cast<quint8>( pixel        & 0xFF); // Blue component
            quint8 a = static_cast<quint8>((pixel >> 24) & 0xFF); // Alpha component

            // Add them to the Vector
            pixelData.append(r);
            pixelData.append(g);
            pixelData.append(b);
            pixelData.append(a);
        }
    }
    return pixelData;
}
