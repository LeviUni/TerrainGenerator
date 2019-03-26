#ifndef MATERIAL_H
#define MATERIAL_H

#include <QOpenGLFunctions_3_3_Core>
#include <QVector4D>

#include <map>
#include <memory>

#include "shaderprogram.h"
#include "texture.h"

class Material : protected QOpenGLFunctions_3_3_Core {

public:
    Material(GLfloat Ka, GLfloat Kd, GLfloat Ks, GLuint n);
    ~Material();

    // support converting to QVector4D for material parameter uploading
    QVector4D getMaterialVector() const;

    void bindTextures(GLuint startSlot = 0);
    void addTexture(unsigned int slot, const std::string& location);
    void addTexture(unsigned int slot, GLuint textureID);

    void setCustomShader(const ShaderProgramPtr& shader) { customShader = shader; }
    const ShaderProgramPtr& getCustomShader() const { return customShader; }

private:
    // lighting paramters
    GLfloat Ka, Kd, Ks;
    GLuint n;

    // textures of this material
    std::map<unsigned int, TexturePtr> textures;

    // custom shader of this material
    ShaderProgramPtr customShader;

    void initializeTexture(const TexturePtr& textureID, const std::string& location);
    QVector<quint8> imageToBytes(const QImage& image);

};

typedef std::shared_ptr<Material> MaterialPtr;

#endif
