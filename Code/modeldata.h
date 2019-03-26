#ifndef MODEL_DATA_H
#define MODEL_DATA_H

#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>

#include <vector>
#include <memory>

#include "vertex.h"
#include "material.h"

class ModelData : protected QOpenGLFunctions_3_3_Core {

public:
    ModelData(const std::vector<vertex>& vertices, const std::vector<GLuint>& indices, bool calculateTangents = false);
    ModelData(const std::string& objFile);
    ~ModelData();

    void draw();

private:
    void initializeBuffers(const std::vector<vertex>& vertices, const std::vector<GLuint>& indices);

    static void calculateTangents(std::vector<vertex>& vertices, const std::vector<GLuint>& indices);

    // This model's VAO, vertex VBO and index EAB
    GLuint vao, vbo, eab;

    // the amount of vertices in this model
    GLsizei vertexCount;

};

typedef std::shared_ptr<ModelData> ModelDataPtr;

#endif
