#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 objectColor;
uniform float shininess;
uniform float emissive;  // 0.0 = normal, 1.0 = fully emissive (glowing)

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Light lights[32];
uniform int numLights;
uniform vec3 ambientLight;

uniform float transparency;
uniform int materialType; // 0=None, 1=Brick, 2=Checkered, 3=Rock, 4=Organic
uniform float time; // For animations
uniform bool useTexture; // Whether to use texture
uniform sampler2D textureSampler; // Texture sampler

// Pseudo-random function
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// Noise function
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main() {
    // Transparency Dithering
    if (transparency < 0.95) {
        float px = mod(floor(gl_FragCoord.x), 2.0);
        float py = mod(floor(gl_FragCoord.y), 2.0);
        if (px == py) discard;
    }

    // EMISSIVE OBJECTS (light fixtures) - render as bright glowing, skip lighting
    if (emissive > 0.5) {
        vec3 glowColor = objectColor * 1.5;  // Bright glow
        // Add slight pulsing effect
        float pulse = 1.0 + 0.1 * sin(time * 3.0);
        glowColor *= pulse;
        FragColor = vec4(glowColor, 1.0);
        return;
    }

    // Start with base object color
    vec3 finalObjectColor = objectColor;
    
    // If texture is enabled, sample it and multiply with object color
    if (useTexture) {
        vec3 texColor = texture(textureSampler, TexCoord).rgb;
        finalObjectColor = texColor * objectColor;
    }
    
    // Procedural Textures (only if not using texture)
    if (!useTexture) {
        if (materialType == 1) { // Brick Wall - ENHANCED VISIBILITY
            float x = FragPos.x;
            float y = FragPos.y;
            
            // Offset every other row for brick pattern
            if (mod(floor(y * 2.0), 2.0) > 0.5) x += 0.5;
            
            float bx = fract(x);
            float by = fract(y * 2.0);
            
            // THICKER mortar lines for better visibility
            float mortarThickness = 0.12;
            
            if (bx < mortarThickness || by < mortarThickness) {
                // DARK mortar for strong contrast
                finalObjectColor = vec3(0.3, 0.3, 0.35);
            } else {
                // Brick color with MORE variation
                float n = noise(vec2(floor(x), floor(y * 2.0)));
                vec3 brickBase = objectColor;
                // Add significant color variation to each brick
                finalObjectColor = brickBase * (0.7 + 0.5 * n);
            }
        } else if (materialType == 2) { // Checkered Floor
            // Enhanced checkered pattern
            float n = noise(FragPos.xz * 5.0);
            finalObjectColor *= (0.85 + 0.15 * n);
        } else if (materialType == 3) { // Rock/Cave
            float n = noise(FragPos.xz * 2.0 + FragPos.y);
            finalObjectColor *= (0.5 + 0.5 * n); // More pronounced rock texture
        } else if (materialType == 4) { // Cave Walls - Enhanced rocky texture
            // Multi-octave noise for realistic cave walls and floors
            vec3 pos = FragPos * 0.5;
            
            // Use different coordinates for floors vs walls for better texture
            vec2 texCoord;
            if (abs(Normal.y) > 0.9) { // Horizontal surface (floor/ceiling)
                texCoord = pos.xz;
            } else { // Vertical surface (walls)
                texCoord = pos.xy + pos.z;
            }
            
            float n1 = noise(texCoord);
            float n2 = noise(texCoord * 2.0) * 0.5;
            float n3 = noise(texCoord * 4.0) * 0.25;
            float combined = n1 + n2 + n3;
            
            // Add cracks and crevices
            float cracks = noise(texCoord * 8.0);
            if (cracks < 0.3) {
                combined *= 0.4; // Dark cracks
            }
            
            // Apply variation to base color with more contrast
            finalObjectColor *= (0.3 + 0.7 * combined);
            
            // Add slight color variation for realism
            float colorVar = noise(texCoord * 0.3);
            finalObjectColor += vec3(0.08, 0.05, 0.03) * colorVar;
        } else if (materialType == 5) { // Muddy Ground
            // Create muddy, wet ground appearance
            vec2 mudPos = FragPos.xz * 1.5;
            float n1 = noise(mudPos);
            float n2 = noise(mudPos * 3.0) * 0.4;
            float n3 = noise(mudPos * 7.0) * 0.2;
            float mudNoise = n1 + n2 + n3;
            
            // Darker muddy patches
            float mudPatches = noise(mudPos * 0.8);
            if (mudPatches < 0.4) {
                mudNoise *= 0.5; // Very dark mud
            }
            
            // Apply brown/dark tones
            vec3 mudColor = objectColor * (0.3 + 0.4 * mudNoise);
            
            // Add slight wetness shimmer
            float wetness = noise(mudPos * 5.0 + time * 0.1);
            mudColor += vec3(0.05, 0.04, 0.03) * wetness * 0.3;
            
            finalObjectColor = mudColor;
        }
    }

    // BRIGHTER ROOM LIGHTING
    vec3 ambient = ambientLight * finalObjectColor * 0.55;  // Increased ambient for room feel
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = ambient;
    
    for(int i = 0; i < numLights; i++) {
        vec3 lightPos = lights[i].position;
        float intensity = lights[i].intensity * 1.2;  // Boosted intensity
        
        // Diffuse
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * intensity;
        
        // Specular (Blinn-Phong)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
        vec3 specular = spec * lights[i].color * intensity * 0.3;
        
        // Attenuation - balanced falloff
        float distance = length(lightPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.04 * distance + 0.01 * distance * distance);
        
        // ENHANCED SHADOWS
        float shadowFactor = 1.0;
        
        // Self-shadowing - sharper transition
        float selfShadow = smoothstep(-0.1, 0.4, dot(norm, lightDir));
        shadowFactor *= mix(0.2, 1.0, selfShadow);  // Darker shadows (min 20%)
        
        // Ground shadow
        float groundHeight = FragPos.y;
        if (groundHeight < 3.0) {
            float groundShadow = groundHeight / 3.0;
            shadowFactor *= mix(0.4, 1.0, groundShadow);  // Stronger ground shadow
        }
        
        result += (diffuse + specular) * attenuation * shadowFactor;
    }
    
    // Light fog for depth
    float fogDensity = 0.015;
    float fogGradient = 1.8;
    float dist = length(viewPos - FragPos);
    float fogFactor = exp(-pow((dist * fogDensity), fogGradient));
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    vec3 fogColor = vec3(0.12, 0.12, 0.15);
    
    // Final lighting calculation
    vec3 totalLight = ambientLight * 0.4;  // Base ambient
    
    for(int i=0; i<numLights; i++) {
        vec3 lightPos = lights[i].position;
        float intensity = lights[i].intensity * 1.2;

        float d = length(lightPos - FragPos);
        float att = 1.0 / (1.0 + 0.04 * d + 0.01 * d * d);
        
        vec3 lDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lDir), 0.0);
        
        vec3 hDir = normalize(lDir + viewDir);
        float spec = pow(max(dot(norm, hDir), 0.0), shininess);
        
        // Shadow calculation
        float shadowFactor = smoothstep(-0.1, 0.4, dot(norm, lDir));
        shadowFactor = mix(0.2, 1.0, shadowFactor);
        
        // Floor shadow
        float groundProximity = smoothstep(0.0, 3.0, FragPos.y);
        shadowFactor *= mix(0.4, 1.0, groundProximity);
        
        totalLight += (lights[i].color * intensity * diff * att * shadowFactor);
        result += (lights[i].color * intensity * spec * 0.2 * att); 
    }
    
    vec3 final = totalLight * finalObjectColor + (result - ambient) * 0.4;
    
    // Subtle ambient occlusion
    float ao = 1.0;
    ao *= 0.9 + 0.1 * smoothstep(0.0, 2.0, FragPos.y);
    final *= ao;
    
    // Minimum visibility
    final = max(final, finalObjectColor * 0.1);
    
    // Apply fog
    final = mix(fogColor, final, fogFactor);
    
    FragColor = vec4(final, transparency);
}


