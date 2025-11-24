uniform float transparency;
uniform int materialType; // 0=None, 1=Brick, 2=Checkered, 3=Rock, 4=Organic
uniform float time; // For animations

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
        bool pattern = (int(gl_FragCoord.x) % 2 == int(gl_FragCoord.y) % 2);
        if (pattern) discard;
    }

    // Procedural Textures
    vec3 finalObjectColor = objectColor;
    
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
    }

    // Ambient
    vec3 ambient = ambientLight * finalObjectColor;
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = ambient;
    
    for(int i = 0; i < numLights; i++) {
        vec3 lightPos = lights[i].position;
        float intensity = lights[i].intensity;
        
        // Flickering Logic (if intensity is odd, treat as flickering torch)
        // Hacky way to identify torches without new structs, assuming high intensity = stable
        // Actually, let's just use time to flicker ALL lights slightly for atmosphere, 
        // or specifically Level 2 lights.
        // For now, let's just add subtle flicker to everything if time > 0
        if (time > 0.0) {
             float flicker = 0.9 + 0.1 * sin(time * 10.0 + float(i));
             intensity *= flicker;
        }

        // Diffuse
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * intensity;
        
        // Specular (Blinn-Phong)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
        vec3 specular = spec * lights[i].color * intensity * 0.5;
        
        // Attenuation
        float distance = length(lightPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        result += (diffuse + specular) * attenuation;
    }
    
    // Fog
    float fogDensity = 0.04;
    float fogGradient = 1.5;
    float dist = length(viewPos - FragPos);
    float fogFactor = exp(-pow((dist * fogDensity), fogGradient));
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    vec3 fogColor = vec3(0.1, 0.1, 0.15);
    vec3 final = mix(fogColor, result * finalObjectColor, fogFactor); // Apply texture color to result
    
    // Fix: result already contains light color, we need to multiply by texture color properly
    // Standard lighting: (Ambient + Diffuse + Specular) * ObjectColor
    // But we calculated result = Ambient + (Diffuse+Specular)*Atten
    // Ambient already multiplied by ObjectColor. Diffuse/Spec need it too?
    // Actually: Ambient = Amb * Obj. Diffuse = Light * Diff * Obj. Spec = Light * Spec.
    // My previous code: diffuse = diff * lightColor * intensity. Missing ObjectColor!
    
    // Let's redo the combination for correctness with textures
    vec3 totalLight = ambientLight; // Start with ambient
    
    for(int i=0; i<numLights; i++) {
        vec3 lightPos = lights[i].position;
        float intensity = lights[i].intensity;
        if (time > 0.0) intensity *= (0.9 + 0.1 * sin(time * 20.0 + float(i)*13.0)); // Flicker

        float d = length(lightPos - FragPos);
        float att = 1.0 / (1.0 + 0.09 * d + 0.032 * d * d);
        
        vec3 lDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lDir), 0.0);
        
        vec3 hDir = normalize(lDir + viewDir);
        float spec = pow(max(dot(norm, hDir), 0.0), shininess);
        
        totalLight += (lights[i].color * intensity * diff * att);
        // Specular is additive, not multiplied by object color usually (plastic), but for metal it is.
        // Let's keep it simple:
        result += (lights[i].color * intensity * spec * 0.5 * att); 
    }
    
    final = totalLight * finalObjectColor + (result - ambient); // Add specular on top
    
    final = mix(fogColor, final, fogFactor);
    FragColor = vec4(final, 1.0);
}


