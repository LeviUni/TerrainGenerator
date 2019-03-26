#ifndef VERTEX_H
#define VERTEX_H

#include <QOpenGLFunctions_3_3_Core>
#include <QVector2D>
#include <QVector3D>

struct vertex {
    // position
    GLfloat x, y, z;

    // normal
    GLfloat nx, ny, nz;

    // tangent
    GLfloat tx, ty, tz;

    // uv texture coordinate
    GLfloat u, v;

    inline QVector3D getPosition() const { return QVector3D( x,  y,  z); }
    inline QVector3D getNormal()   const { return QVector3D(nx, ny, nz); }
    inline QVector3D getTangent()  const { return QVector3D(tx, ty, tz); }
    inline QVector2D getUV()       const { return QVector2D(u, v); }
};

#endif
