#include "noisegrid.h"

#include <random>
#include <chrono>

#include <cmath>

// on windows, cmath doesn't define M_PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

NoiseGrid::NoiseGrid(unsigned N) : n(N) {
    size = (1 << N) + 1;
    grid = new float*[size];

    for (unsigned i = 0; i < size; i++) {
        grid[i] = new float[size];
    }

    makeZero(grid);
}

NoiseGrid::~NoiseGrid() {
    for (unsigned i = 0; i < size; i++) {
        delete[] grid[i];
    }

    delete[] grid;
}

ModelDataPtr NoiseGrid::createModelData() const {
    std::vector<vertex> vertices;
    std::vector<GLuint> indices;

    // add the vertices to the vector
    for (unsigned x = 0; x < size; x++) {
        for (unsigned z = 0; z < size; z++) {
            vertices.push_back(createVertex(x, z));
        }
    }

    // add the indices to the vector
    for (unsigned x = 0; x < size - 1; x++) {
        for (unsigned z = 0; z < size - 1; z++) {
            // first triangle
            indices.push_back(x * size + z);
            indices.push_back(x * size + (z + 1));
            indices.push_back((x + 1) * size + z);

            // second triangle
            indices.push_back((x + 1) * size + z);
            indices.push_back(x * size + (z + 1));
            indices.push_back((x + 1) * size + (z + 1));
        }
    }

    return ModelDataPtr(new ModelData(vertices, indices, true));
}

vertex NoiseGrid::createVertex(unsigned x, unsigned z) const {
    // position of this vertex
    QVector3D position(static_cast<GLfloat>(x), grid[x][z], static_cast<GLfloat>(z));

    // calculate the normal at this vertex
    QVector3D normal(0, 0, 0);
    int count = 0;

    if (x > 0 && z > 0) {
        QVector3D xneg(static_cast<GLfloat>(x - 1), grid[x - 1][z], static_cast<GLfloat>(z));
        QVector3D zneg(static_cast<GLfloat>(x), grid[x][z - 1], static_cast<GLfloat>(z - 1));
        normal += QVector3D::crossProduct(xneg - position, zneg - position).normalized();
        count++;
    }

    if (x > 0 && z < size - 1) {
        QVector3D xneg(static_cast<GLfloat>(x - 1), grid[x - 1][z], static_cast<GLfloat>(z));
        QVector3D zpos(static_cast<GLfloat>(x), grid[x][z + 1], static_cast<GLfloat>(z + 1));
        normal += QVector3D::crossProduct(zpos - position, xneg - position).normalized();
        count++;
    }

    if (x < size - 1 && z < size - 1) {
        QVector3D xpos(static_cast<GLfloat>(x + 1), grid[x + 1][z], static_cast<GLfloat>(z));
        QVector3D zpos(static_cast<GLfloat>(x), grid[x][z + 1], static_cast<GLfloat>(z + 1));
        normal += QVector3D::crossProduct(xpos - position, zpos - position).normalized();
        count++;
    }

    if (x < size - 1 && z > 0) {
        QVector3D xpos(static_cast<GLfloat>(x + 1), grid[x + 1][z], static_cast<GLfloat>(z));
        QVector3D zneg(static_cast<GLfloat>(x), grid[x][z - 1], static_cast<GLfloat>(z - 1));
        normal += QVector3D::crossProduct(zneg - position, xpos - position).normalized();
        count++;
    }

    if (count > 0) {
        normal /= -count;
    }

    return {
        // position
        position.x() - size / 2.0f, position.y(), position.z() - size / 2.0f,

        // normal
        normal.x(), normal.y(), normal.z(),

        // tangent
        0, 0, 0,

        // uv
        position.x() / 10.0f, position.z() / 10.0f
    };
}

void NoiseGrid::addSpike(float min, float max, unsigned n) {
    unsigned quadSize = (size - 1) >> n;

    // initialize the temporary grid
    float ** temporaryGrid = new float*[size];
    for (unsigned i = 0; i < size; i++) {
        temporaryGrid[i] = new float[size];
    }

    makeZero(temporaryGrid);

    // initialize spike
    for (unsigned x = 0; x < size; x += quadSize) {
        for (unsigned z = 0; z < size; z += quadSize) {
            if (x == size / 2 && z == size / 2) {
                temporaryGrid[x][z] = max;
            } else {
                temporaryGrid[x][z] = min;
            }
        }
    }

    // add the spike
    addInterpolatedGrid(temporaryGrid, quadSize);

    // delete temporary grid
    for (unsigned i = 0; i < size; i++) {
        delete[] temporaryGrid[i];
    }
    delete[] temporaryGrid;
}

void NoiseGrid::addOctaves(unsigned octaves, float amplitude, unsigned n) {
    // initialize the random number generator
    std::mt19937_64 engine;

    auto start = std::chrono::high_resolution_clock::now();
    auto tm = std::chrono::duration_cast<std::chrono::nanoseconds>(start.time_since_epoch()).count();
    tm *= tm;
    engine.seed(static_cast<std::mt19937_64::result_type>(tm));

    std::uniform_real_distribution<float> dist(0, 1);

    auto rng = [&engine, &dist]() -> float {
            return dist(engine);
    };

    // initialize the temporary grid
    float ** temporaryGrid = new float*[size];
    for (unsigned i = 0; i < size; i++) {
        temporaryGrid[i] = new float[size];
    }

    // add octaves
    for (unsigned i = 0; i < octaves; i++) {
        unsigned quadSize = (size - 1) >> n;
        makeZero(temporaryGrid);

        // randomize
        for (unsigned x = 0; x < size; x += quadSize) {
            for (unsigned y = 0; y < size; y += quadSize) {
                temporaryGrid[x][y] = rng() * amplitude;
            }
        }

        addInterpolatedGrid(temporaryGrid, quadSize);

        amplitude /= 2.0f;
        n++;
    }

    // delete temporary grid
    for (unsigned i = 0; i < size; i++) {
        delete[] temporaryGrid[i];
    }
    delete[] temporaryGrid;
}

void NoiseGrid::makeZero(float **grid) const {
    for (unsigned i = 0; i < size; i++) {
        for (unsigned j = 0; j < size; j++) {
            grid[i][j] = 0;
        }
    }
}

void NoiseGrid::addInterpolatedGrid(float **temporaryGrid, unsigned quadSize) {
    auto interpolate = [](float left, float right, float x) -> float {
        x = x * x * (3 - 2 * x);
        return right * x + left * (1 - x);
    };

    for (unsigned x = 0; x < size; x++) {
        for (unsigned z = 0; z < size; z++) {
            unsigned xneg = (x / quadSize) * quadSize;
            unsigned zneg = (z / quadSize) * quadSize;

            unsigned xpos = xneg + quadSize;
            unsigned zpos = zneg + quadSize;

            float xt = (x - xneg) / float(quadSize);
            float zt = (z - zneg) / float(quadSize);

            // edge case
            if (x >= size - 1) {
                if (z >= size - 1) {
                    grid[x][z] += temporaryGrid[x][z];
                    continue;
                }

                grid[x][z] += interpolate(temporaryGrid[x][zneg], temporaryGrid[x][zpos], zt);
                continue;
            } else if (z >= size - 1) {
                grid[x][z] += interpolate(temporaryGrid[xneg][z], temporaryGrid[xpos][z], xt);
                continue;
            }

            // normal case
            float height1 = interpolate(temporaryGrid[xneg][zneg], temporaryGrid[xpos][zneg], xt);
            float height2 = interpolate(temporaryGrid[xneg][zpos], temporaryGrid[xpos][zpos], xt);

            grid[x][z] += interpolate(height1, height2, zt);
        }
    }
}
