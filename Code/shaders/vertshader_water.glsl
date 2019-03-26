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
out vec2 vertTexture;
out vec4 position;

void main() {
    vec4 worldSpaceCoordinates = modelMatrix * vec4(vertCoordinates_in, 1.0);

    position = projMatrix * viewMatrix * worldSpaceCoordinates;
    gl_Position = position;

    // send the attributes to the fragment shader
    vertCoordinates = worldSpaceCoordinates.xyz;
    vertTexture = vertTexture_in;
}
