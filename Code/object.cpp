#include "object.h"

Object::Object(ModelDataPtr model, const std::vector<MaterialPtr>& materials) :
        model(model), materials(materials),
        translation(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1) {
}

Object::Object(ModelDataPtr model, const MaterialPtr& material) :
        model(model), materials({material}),
        translation(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1) {
}

QMatrix4x4 Object::getModelMatrix() const {
    QMatrix4x4 matrix;
    matrix.translate(translation);
    matrix.scale(scale);
    matrix.rotate(rotation.x(), { 1, 0, 0 });
    matrix.rotate(rotation.y(), { 0, 1, 0 });
    matrix.rotate(rotation.z(), { 0, 0, 1 });

    return matrix;
}
