#version 330 core

#define M_PI 3.141593

layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec3 vertTangent_in;
layout (location = 3) in vec2 vertTexture_in;

// transformation matrices
uniform mat4 modelMatrix;
uniform mat3 normalModelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 vertCoordinates;
out vec3 vertNormal;
out vec3 vertTangent;
out vec2 vertTexture;

// water uniforms
uniform float waterHeight;
uniform int clipSign;

void main() {

    vec4 worldSpaceCoordinates = modelMatrix * vec4(vertCoordinates_in, 1.0);
    gl_Position = projMatrix * viewMatrix * worldSpaceCoordinates;

    // set the clip distance
    if (clipSign == 0) {
        gl_ClipDistance[0] = 1;
    } else {
        gl_ClipDistance[0] = clipSign * (worldSpaceCoordinates.y - waterHeight);
    }

    // calculate the tangent vector
    vec4 tangent = modelMatrix * vec4(vertTangent_in, 0.0);

    // send the attributes to the fragment shader
    vertCoordinates = worldSpaceCoordinates.xyz;
    vertNormal = normalModelMatrix * vertNormal_in;
    vertTangent = tangent.xyz;
    vertTexture = vertTexture_in;
}
