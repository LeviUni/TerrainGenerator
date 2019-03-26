#include "shaderprogram.h"

ShaderProgram::ShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) :
        vertexShader(vertexShader), fragmentShader(fragmentShader) {

    initializeOpenGLFunctions();

    program.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShader.c_str());
    program.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShader.c_str());
    program.link();
}

ShaderProgram::~ShaderProgram() {}

void ShaderProgram::bind() {
    program.bind();
}

void ShaderProgram::setUniform(const std::string &name, GLint value) {
    bind();
    glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string &name, GLfloat value) {
    bind();
    glUniform1f(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string &name, const QVector2D &vector) {
    bind();
    glUniform2f(getUniformLocation(name), vector.x(), vector.y());
}

void ShaderProgram::setUniform(const std::string &name, const QVector3D &vector) {
    bind();
    glUniform3f(getUniformLocation(name), vector.x(), vector.y(), vector.z());
}

void ShaderProgram::setUniform(const std::string &name, const QVector4D &vector) {
    bind();
    glUniform4f(getUniformLocation(name), vector.x(), vector.y(), vector.z(), vector.w());
}

void ShaderProgram::setUniform(const std::string &name, const QMatrix3x3 &matrix) {
    bind();
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, matrix.data());
}

void ShaderProgram::setUniform(const std::string &name, const QMatrix4x4 &matrix) {
    bind();
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix.data());
}

void ShaderProgram::setUniform(const std::string &name, const std::vector<GLint>& values) {
    bind();
    glUniform1iv(getUniformLocation(name), static_cast<GLsizei>(values.size()), values.data());
}

void ShaderProgram::setUniform(const std::string &name, const std::vector<QVector4D> &values) {
    bind();
    glUniform4fv(getUniformLocation(name), static_cast<GLsizei>(values.size()), reinterpret_cast<const GLfloat *>(values.data()));
}

GLint ShaderProgram::getUniformLocation(const std::string& name) {
    auto iter = uniforms.find(name);
    if (iter != uniforms.end()) {
        return iter->second;
    }

    GLint location = program.uniformLocation(name.c_str());
    uniforms[name] = location;
    return location;
}
