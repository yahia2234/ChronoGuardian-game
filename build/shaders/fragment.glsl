#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Material properties
uniform vec3 objectColor;
uniform float shininess;
uniform bool useTexture;
uniform sampler2D textureSampler;

// Lighting
#define MAX_LIGHTS 10
uniform int numLights;
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];

uniform vec3 viewPos;
uniform vec3 ambientLight;

void main()
{
    vec3 baseColor = useTexture ? texture(textureSampler, TexCoord).rgb : objectColor;
    
    // Ambient
    vec3 ambient = ambientLight * baseColor;
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = ambient;
    
    // Calculate lighting from all light sources
    for(int i = 0; i < numLights; i++)
    {
        vec3 lightDir = normalize(lightPositions[i] - FragPos);
        
        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColors[i] * baseColor;
        
        // Specular (Blinn-Phong)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
        vec3 specular = spec * lightColors[i];
        
        // Attenuation
        float distance = length(lightPositions[i] - FragPos);
        float attenuation = lightIntensities[i] / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        diffuse *= attenuation;
        specular *= attenuation;
        
        result += diffuse + specular;
    }
    
    FragColor = vec4(result, 1.0);
}
