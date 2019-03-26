#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix>

#include <string>
#include <map>
#include <memory>

class ShaderProgram : protected QOpenGLFunctions_3_3_Core {

public:
    ShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
    ~ShaderProgram();

    void bind();

    void setUniform(const std::string& name, GLint value);
    void setUniform(const std::string& name, GLfloat value);
    void setUniform(const std::string& name, const QVector2D& vector);
    void setUniform(const std::string& name, const QVector3D& vector);
    void setUniform(const std::string& name, const QVector4D& vector);
    void setUniform(const std::string& name, const QMatrix3x3& matrix);
    void setUniform(const std::string& name, const QMatrix4x4& matrix);

    void setUniform(const std::string& name, const std::vector<GLint>& values);
    void setUniform(const std::string& name, const std::vector<QVector4D>& values);

private:
    GLint getUniformLocation(const std::string& name);

    QOpenGLShaderProgram program;
    std::map<std::string, GLint> uniforms;

    const std::string vertexShader;
    const std::string fragmentShader;

};

typedef std::shared_ptr<ShaderProgram> ShaderProgramPtr;

#endif // SHADERPROGRAM_H
