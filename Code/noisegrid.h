#ifndef NOISEGRID_H
#define NOISEGRID_H

#include "modeldata.h"

class NoiseGrid {

public:
    /**
     * @brief NoiseGrid
     *
     * The grid will be 2^N + 1 by 2^N + 1 vertices,
     * and 2^N by 2^N quads
     *
     * @param N
     */
    NoiseGrid(unsigned N);
    ~NoiseGrid();

    void addSpike(float min, float max, unsigned n);
    void addOctaves(unsigned octaves, float amplitude, unsigned n);
    ModelDataPtr createModelData() const;

private:
    vertex createVertex(unsigned x, unsigned z) const;

    void makeZero(float **grid) const;
    void addInterpolatedGrid(float **temporaryGrid, unsigned quadSize);

    unsigned size;
    float **grid;
    unsigned n;

};

#endif // NOISEGRID_H
