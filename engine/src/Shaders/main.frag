#version 430 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

////////////////////
/// POINT LIGHT   //
////////////////////

uniform int pointLightCount;

struct PointLight {
    //16 Bytes
    vec3 position;
    int id;

    //16 Bytes
    vec3 color;
    float intensity;

    //16 Bytes
    vec3 reflectionColor;
    float radius;
};

uniform int spotLightCount;

struct SpotLight {
    //16 bytes
	vec3 position;
	int id;

    //16 bytes
	vec3 color;
	float intensity;

    //16 bytes
	vec3 reflectionColor;
	float radius;

    //16 bytes
	vec3 direction;
	float innerCutOff;

    //4 bytes
	float outerCutoff;

    //Works by chunks of 16 bytes so this totals 80 bytes
};

layout(std430, binding = 0) buffer PointLightBuffer
{
    PointLight pointLights[];
};

layout(std430, binding = 1) buffer SpotLightBuffer
{
    SpotLight spotLights[];
};

//////////////////////////
/// DIRECTIONAL LIGHT   //
//////////////////////////

struct DirectionalLight {
    vec3 color;
    float intensity;

    vec3 reflectionColor;
    vec3 direction;
};

uniform DirectionalLight directionalLight;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPosition;

uniform vec3 camPos;
uniform Material material;

vec3 modelColor = vec3(1.0, 1.0, 1.0);

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading

    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    spec *= diff;

    // attenuation

    float distance = length(light.position - fragPos);
    float attenuation = clamp(
        1.0 - distance / light.radius,
        0.0,
        1.0
    );

    // spotlight

    float theta = dot(lightDir, normalize(-light.direction));

    float innerCutoff = cos(light.innerCutOff);
    float outerCutoff = cos(light.outerCutoff);

    float epsilon = innerCutoff - outerCutoff;

    float spotIntensity = clamp(
        (theta - outerCutoff) / epsilon,
        0.0,
        1.0
    );

    // combine results

    vec3 radiance =
        light.color *
        light.intensity *
        attenuation *
        spotIntensity;

    vec3 albedo = texture(material.diffuse, TexCoords).rgb;
    vec3 specMap = texture(material.specular, TexCoords).rgb;

    vec3 diffuse = diff * albedo;
    vec3 specular = spec * specMap * light.reflectionColor;

    return (diffuse + specular) * radiance;
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    spec *= diff;

    vec3 radiance = light.color * light.intensity;

    vec3 albedo = texture(material.diffuse, TexCoords).rgb;
    vec3 specMap = texture(material.specular, TexCoords).rgb;

    vec3 diffuse = diff * albedo;
    vec3 specular = spec * specMap * light.reflectionColor;

    return (diffuse + specular) * radiance;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    spec *= diff;

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = clamp(
        1.0 - distance / light.radius,
        0.0,
        1.0
    ); 

    vec3 radiance = light.color * light.intensity * attenuation;


    // combine results
    vec3 albedo = texture(material.diffuse, TexCoords).rgb;
    vec3 specMap = texture(material.specular, TexCoords).rgb;

    vec3 diffuse = diff * albedo;
    vec3 specular = spec * specMap * light.reflectionColor;

    return (diffuse + specular) * radiance;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(camPos - FragPosition);

    //Use ambient by default.
    vec3 ambient = 0.2f * texture(material.diffuse, TexCoords).rgb;

    vec3 result = ambient;
    for(int i = 0; i < pointLightCount; i++)
        result += CalcPointLight(pointLights[i], norm, FragPosition, viewDir);  

    for(int i = 0; i < spotLightCount; i++)
        result += CalcSpotLight(spotLights[i], norm, FragPosition, viewDir);  

    result += CalcDirectionalLight(directionalLight, norm, viewDir);

    FragColor = vec4(result * modelColor, 1.0);
}

