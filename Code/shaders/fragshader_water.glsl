#version 330 core

// Define constants
#define M_PI 3.141593

// model-specific variables
in vec3 vertCoordinates;
in vec2 vertTexture;
in vec4 position;

// lighting variables
uniform vec3 lightPosition;
uniform vec3 lightColor;

// camera variables
uniform vec3 cameraPosition;
uniform float near;
uniform float far;
uniform float scale;

// output color
out vec4 fColor;

// material uniforms
uniform sampler2D dudvMap;
uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D depthMap;
uniform vec4 material;

// time
uniform float time;

float getDepth(float f) {
    return 2.0 * near * far / (far + near - (2.0 * f - 1.0) * (far - near)) / scale;
}

void main() {
    vec2 coords = (position.xy / position.w + 1) / 2;

    // calculate the water depth
    float underwaterDistance = getDepth(texture2D(depthMap, coords).r);
    float waterDistance = getDepth(gl_FragCoord.z);
    float depth = underwaterDistance - waterDistance;

    // sample the dudv map
    vec2 direction = normalize(vertCoordinates.xz);
    vec2 move = mod(direction * time / 10, vec2(1));
    vec2 dudv = texture2D(dudvMap, vertTexture * 500 + move).rg * 2 - 1;
    float waveFactor = 0.015;
    float waveStrength = waveFactor * clamp(depth / 20.0, 0, 1);

    // get the sample positions
    vec2 reflectionCoords = vec2(coords.x, 1 - coords.y);
    reflectionCoords += dudv * waveStrength;
    reflectionCoords = clamp(reflectionCoords, 0.001, 0.999);

    vec2 refractionCoords = coords;
    refractionCoords += dudv * waveStrength;
    refractionCoords = clamp(refractionCoords, 0.001, 0.999);

    // sample the reflection and 'refraction' textures
    vec4 reflectionColor = texture2D(reflectionTexture, reflectionCoords);
    vec4 refractionColor = texture2D(refractionTexture, refractionCoords);

    // calculate the relevant vectors
    vec3 N = normalize(vec3(dudv.x, 10, dudv.y));
    vec3 L = normalize(lightPosition - vertCoordinates);
    vec3 R = 2 * (dot(N, L) * N) - L;
    vec3 V = normalize(cameraPosition - vertCoordinates);

    // calculate the fresnel factor
    float fresnel = dot(V, N);

    // mix the textures based on the fresnel factor
    fColor = mix(reflectionColor, refractionColor, fresnel);

    // mix in a bit of blue
    fColor = mix(fColor, vec4(0, 0.4, 0.8, 1.0), 0.2);

    // add specular highlights
    float s = max(0.0, dot(R, V));
    s = pow(s, material.w);

    vec4 Is = vec4(1.0) * s * material.z * waveStrength / waveFactor;
    fColor += Is * vec4(lightColor, 1.0);

    // apply alpha based on depth
    fColor.a = clamp(depth / 2, 0.0, 1.0);
}
