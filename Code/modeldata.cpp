#include "modeldata.h"

#include "model.h"

/**
 * @brief ModelData::ModelData
 *
 * Constructor of ModelData. This puts vertices and indices into
 * OpenGL buffers.
 *
 * @param vertices
 * @param indices
 */
ModelData::ModelData(const std::vector<vertex>& vertices, const std::vector<GLuint>& indices, bool shouldCalculateTangents) {
    std::vector<vertex> v(vertices);

    if (shouldCalculateTangents) {
        calculateTangents(v, indices);
    }

    initializeOpenGLFunctions();
    initializeBuffers(v, indices);
}

ModelData::ModelData(const std::string& objFile) {
    Model model(objFile.c_str());
    model.unitize();

    QVector<QVector3D> vertices = model.getVertices_indexed();
    QVector<QVector3D> normals = model.getNormals_indexed();
    QVector<QVector2D> textureCoordinates = model.getTextureCoords_indexed();
    QVector<unsigned> indices = model.getIndices();

    std::vector<vertex> mesh_vertices;
    std::vector<GLuint> mesh_indices;

    for (int i = 0; i < vertices.size(); i++) {
        mesh_vertices.push_back({
            // position
            vertices[i].x(), vertices[i].y(), vertices[i].z(),

            // normal
            normals[i].x(), normals[i].y(), normals[i].z(),

            // tangent (calculated later)
            0, 0, 0,

            // texture coordinates
            textureCoordinates[i].x(), textureCoordinates[i].y()
        });
    }

    for (auto& i : indices) {
        mesh_indices.push_back(i);
    }

    // calculate the tangent vectors
    calculateTangents(mesh_vertices, mesh_indices);

    // initialize everything
    initializeOpenGLFunctions();
    initializeBuffers(mesh_vertices, mesh_indices);
}

void ModelData::initializeBuffers(const std::vector<vertex> &vertices, const std::vector<GLuint> &indices) {
    vertexCount = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &eab);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(vertex)), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) (3 * sizeof(GLfloat)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) (6 * sizeof(GLfloat)));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) (9 * sizeof(GLfloat)));
}

void ModelData::calculateTangents(std::vector<vertex>& vertices, const std::vector<GLuint>& indices) {
   for (unsigned i = 0; i < indices.size(); i += 3) {
        // get the indices of the triangle
        unsigned i0 = indices[i + 0];
        unsigned i1 = indices[i + 1];
        unsigned i2 = indices[i + 2];

        // get the vertex position and UV coordinate edges of the triangle
        QVector3D pEdge1 = vertices[i1].getPosition() - vertices[i0].getPosition();
        QVector3D pEdge2 = vertices[i2].getPosition() - vertices[i0].getPosition();
        QVector2D tEdge1 = vertices[i1].getUV() - vertices[i0].getUV();
        QVector2D tEdge2 = vertices[i2].getUV() - vertices[i0].getUV();

        // calculate UV edge matrix determinant
        float det = tEdge1.x() * tEdge2.y() - tEdge1.y() * tEdge2.x();

        // Had an issue where det would be 0 on the cat model. This code
        // should solve the issue of (+- inf, +- inf, +- inf) tangents
        if (det < 0.00001f && det > 0) {
            det = 0.00001f;
        } else if (det > -0.00001f && det < 0.00001f) {
            det = -0.00001f;
        }

        // calculate the tangent
        QVector3D tangent = (pEdge1 * tEdge2.y() - pEdge2 * tEdge1.y()) / det;
        tangent.normalize();

        // set the tangents of the vertices in the std::vector
        vertices[i0].tx = tangent.x();
        vertices[i0].ty = tangent.y();
        vertices[i0].tz = tangent.z();

        vertices[i1].tx = tangent.x();
        vertices[i1].ty = tangent.y();
        vertices[i1].tz = tangent.z();

        vertices[i2].tx = tangent.x();
        vertices[i2].ty = tangent.y();
        vertices[i2].tz = tangent.z();
    }
}

ModelData::~ModelData() {

    qDebug() << "ModelData destructor";

    glDeleteBuffers(1, &eab);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

/**
 * @brief ModelData::draw
 *
 * Draws the model
 */
void ModelData::draw() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, (GLvoid *) 0);
}
