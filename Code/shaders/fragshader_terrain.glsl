#version 330 core

// Define constants
#define M_PI 3.141593

// model-specific variables
in vec3 vertCoordinates;
in vec3 vertNormal;
in vec3 vertTangent;
in vec2 vertTexture;

// lighting variables
uniform vec3 lightPosition;
uniform vec3 lightColor;

// camera variables
uniform vec3 cameraPosition;

// output color
out vec4 fColor;

// material uniforms
uniform sampler2D diffuseTexture[3];
uniform sampler2D normalTexture[3];
uniform sampler2D specularTexture[3];
uniform vec4 material[3];

// water uniforms
uniform float waterHeight;

void main() {
    // initialize TBN
    vec3 normal = normalize(vertNormal);
    vec3 tangent = normalize(vertTangent);
    vec3 bitangent = cross(tangent, normal);

    mat3 TBN = mat3(tangent, bitangent, normal);

    // mix based on slope
    float min_map_source = 0.8;
    float max_map_source = 0.85;
    float min_map_dest = 0.2;
    float max_map_dest = 0.8;
    float mixFactorSlope = (normal.y - min_map_source) / (max_map_source - min_map_source);
    mixFactorSlope = (mixFactorSlope - min_map_dest) / (max_map_dest - min_map_dest);
    mixFactorSlope = clamp(mixFactorSlope, 0.0, 1.0);

    // mix based on height
    float mixFactorHeight = clamp(vertCoordinates.y - waterHeight - 0.5, 0, 1);

    // combine the mix factors
    vec3 mixFactors = vec3(mixFactorSlope * mixFactorHeight, 1 - mixFactorSlope, mixFactorSlope * (1 - mixFactorHeight));

    vec3 Ia = vec3(0.0);
    vec3 Id = vec3(0.0);
    vec3 Is = vec3(0.0);

    // constant vectors
    vec3 L = normalize(lightPosition - vertCoordinates);
    vec3 V = normalize(cameraPosition - vertCoordinates);

    // apply the materials

    if (mixFactors.x > 0) {
        // add the grass material colors

        // texture lookups
        vec3 Td = texture2D(diffuseTexture[0], vertTexture).rgb;
        vec3 Ts = texture2D(specularTexture[0], vertTexture).rgb;

        vec3 N = texture2D(normalTexture[0], vertTexture).xyz * 2 - 1;
        N = normalize(TBN * N);
        vec3 R = 2 * (dot(N, L) * N) - L;

        float s = max(0.0, dot(R, V));
        s = pow(s, material[0].w);

        Ia += mixFactors.x * Td * material[0].x;
        Id += mixFactors.x * Td * max(0, dot(N, L)) * material[0].y;
        Is += mixFactors.x * Ts * s * material[0].z;
    }

    if (mixFactors.y > 0) {
        // add the rock material colors

        // texture lookups
        vec3 Td = texture2D(diffuseTexture[1], vertTexture).rgb;
        vec3 Ts = texture2D(specularTexture[1], vertTexture).rgb;

        vec3 N = texture2D(normalTexture[1], vertTexture).xyz * 2 - 1;
        N = normalize(TBN * N);
        vec3 R = 2 * (dot(N, L) * N) - L;

        float s = max(0.0, dot(R, V));
        s = pow(s, material[1].w);

        Ia += mixFactors.y * Td * material[1].x;
        Id += mixFactors.y * Td * max(0, dot(N, L)) * material[1].y;
        Is += mixFactors.y * Ts * s * material[1].z;
    }

    if (mixFactors.z > 0) {
        vec3 Td = texture2D(diffuseTexture[2], vertTexture).rgb;
        vec3 Ts = texture2D(specularTexture[2], vertTexture).rgb;

        vec3 N = texture2D(normalTexture[2], vertTexture).xyz * 2 - 1;
        N = normalize(TBN * N);
        vec3 R = 2 * (dot(N, L) * N) - L;

        float s = max(0.0, dot(R, V));
        s = pow(s, material[2].w);

        Ia += mixFactors.z * Td * material[2].x;
        Id += mixFactors.z * Td * max(0, dot(N, L)) * material[2].y;
        Is += mixFactors.z * Ts * s * material[2].z;
    }

    fColor = vec4(Ia + (Id + Is) * lightColor, 1.0);
}
