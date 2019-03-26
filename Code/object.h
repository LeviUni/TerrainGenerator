#ifndef OBJECT_H
#define OBJECT_H

#include "modeldata.h"

#include <memory>

class Object {

public:
    Object(ModelDataPtr model, const std::vector<MaterialPtr>& materials);
    Object(ModelDataPtr model, const MaterialPtr& material);

    inline void setTranslation(const QVector3D& translation) { this->translation = translation; }
    inline void setRotation(const QVector3D& rotation) { this->rotation = rotation; }
    inline void setScale(const QVector3D& scale) { this->scale = scale; }

    inline const QVector3D& getTranslation() const { return translation; }
    inline const QVector3D& getRotation() const { return rotation; }
    inline const QVector3D& getScale() const { return scale; }

    inline void setSpeed(const QVector3D& speed) { this->speed = speed; }
    inline const QVector3D& getSpeed() const { return speed; }

    QMatrix4x4 getModelMatrix() const;

    const ModelDataPtr& getModel() const { return model; }
    const std::vector<MaterialPtr>& getMaterials() const { return materials; }
private:
    // this object's model
    ModelDataPtr model;

    // this object's material
    std::vector<MaterialPtr> materials;

    // this object's transformations
    QVector3D translation;
    QVector3D rotation;
    QVector3D scale;

    // animation variables
    QVector3D speed;

};

typedef std::shared_ptr<Object> ObjectPtr;

#endif
