#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 FragPos;

uniform float time;

// Hash function for noise
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// 2D noise
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Fractal brownian motion
float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for (int i = 0; i < 5; i++) {
        value += amplitude * noise(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return value;
}

// Draw a horizontal line segment
float drawLine(vec2 uv, vec2 start, vec2 end, float thickness) {
    vec2 lineDir = end - start;
    float lineLen = length(lineDir);
    lineDir = normalize(lineDir);
    
    vec2 toPoint = uv - start;
    float proj = clamp(dot(toPoint, lineDir), 0.0, lineLen);
    vec2 closest = start + lineDir * proj;
    
    return 1.0 - smoothstep(0.0, thickness, length(uv - closest));
}

// Draw letter G
float drawG(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m, h-m), t));    // top
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));        // left
    result = max(result, drawLine(uv, vec2(m, m), vec2(w-m, m), t));        // bottom
    result = max(result, drawLine(uv, vec2(w-m, m), vec2(w-m, h/2), t));    // right bottom
    result = max(result, drawLine(uv, vec2(w/2, h/2), vec2(w-m, h/2), t));  // middle right
    
    return result;
}

// Draw letter A
float drawA(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m, h-m), t));  // top
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));      // left
    result = max(result, drawLine(uv, vec2(w-m, m), vec2(w-m, h-m), t));  // right
    result = max(result, drawLine(uv, vec2(m, h/2), vec2(w-m, h/2), t));  // middle
    
    return result;
}

// Draw letter M
float drawM(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));          // left
    result = max(result, drawLine(uv, vec2(w-m, m), vec2(w-m, h-m), t));      // right
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w/2, h/2), t));      // left diagonal
    result = max(result, drawLine(uv, vec2(w/2, h/2), vec2(w-m, h-m), t));    // right diagonal
    
    return result;
}

// Draw letter E
float drawE(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m, h-m), t));  // top
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));      // left
    result = max(result, drawLine(uv, vec2(m, h/2), vec2(w-m*2, h/2), t)); // middle
    result = max(result, drawLine(uv, vec2(m, m), vec2(w-m, m), t));      // bottom
    
    return result;
}

// Draw letter O
float drawO(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m, h-m), t));  // top
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));      // left
    result = max(result, drawLine(uv, vec2(w-m, m), vec2(w-m, h-m), t));  // right
    result = max(result, drawLine(uv, vec2(m, m), vec2(w-m, m), t));      // bottom
    
    return result;
}

// Draw letter V
float drawV(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w/2, m), t));    // left diagonal
    result = max(result, drawLine(uv, vec2(w/2, m), vec2(w-m, h-m), t));  // right diagonal
    
    return result;
}

// Draw letter R
float drawR(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));        // left vertical
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m, h-m), t));    // top
    result = max(result, drawLine(uv, vec2(w-m, h/2), vec2(w-m, h-m), t));  // right top
    result = max(result, drawLine(uv, vec2(m, h/2), vec2(w-m, h/2), t));    // middle
    result = max(result, drawLine(uv, vec2(m+0.1, h/2-0.05), vec2(w-m, m), t)); // diagonal
    
    return result;
}

// Draw "GAME"
float drawGAME(vec2 uv, float size, float startX) {
    float result = 0.0;
    float letterWidth = size * 0.7;
    
    result = max(result, drawG(uv - vec2(startX + letterWidth * 0.0, 0.0), size));
    result = max(result, drawA(uv - vec2(startX + letterWidth * 1.0, 0.0), size));
    result = max(result, drawM(uv - vec2(startX + letterWidth * 2.0, 0.0), size));
    result = max(result, drawE(uv - vec2(startX + letterWidth * 3.0, 0.0), size));
    
    return result;
}

// Draw "OVER"
float drawOVER(vec2 uv, float size, float startX) {
    float result = 0.0;
    float letterWidth = size * 0.7;
    
    result = max(result, drawO(uv - vec2(startX + letterWidth * 0.0, 0.0), size));
    result = max(result, drawV(uv - vec2(startX + letterWidth * 1.0, 0.0), size));
    result = max(result, drawE(uv - vec2(startX + letterWidth * 2.0, 0.0), size));
    result = max(result, drawR(uv - vec2(startX + letterWidth * 3.0, 0.0), size));
    
    return result;
}

void main() {
    vec2 uv = FragPos;
    
    // Animated dark red/black background with smoke effect
    vec2 noiseCoord = uv * 2.0 + vec2(time * 0.05, time * 0.03);
    float n = fbm(noiseCoord);
    
    // Dark red/black gradient background
    vec3 bgColor1 = vec3(0.15, 0.02, 0.02);  // Dark red
    vec3 bgColor2 = vec3(0.05, 0.0, 0.0);    // Almost black
    vec3 bgColor = mix(bgColor1, bgColor2, uv.y * 0.5 + 0.5 + n * 0.3);
    
    // Add some floating embers/particles
    float embers = 0.0;
    for (int i = 0; i < 5; i++) {
        vec2 emberUV = uv * (5.0 + float(i) * 3.0);
        emberUV.y += time * (0.2 + float(i) * 0.1);  // Rise upward
        float emberNoise = hash(floor(emberUV));
        if (emberNoise > 0.95) {
            float flicker = sin(time * 5.0 + emberNoise * 50.0) * 0.3 + 0.7;
            embers = max(embers, flicker * (1.0 - length(fract(emberUV) - 0.5) * 4.0));
        }
    }
    bgColor += vec3(1.0, 0.3, 0.0) * embers * 0.5;  // Orange ember color
    
    // Draw title "GAME"
    float titleSize = 0.2;
    float gameWidth = titleSize * 0.7 * 4.0;
    float gameStartX = -gameWidth / 2.0;
    float title1 = drawGAME(uv - vec2(0.0, 0.2), titleSize, gameStartX);
    
    // Draw title "OVER"
    float overWidth = titleSize * 0.7 * 4.0;
    float overStartX = -overWidth / 2.0;
    float title2 = drawOVER(uv - vec2(0.0, -0.05), titleSize, overStartX);
    
    // Combine titles
    float titles = max(title1, title2);
    
    // Title color - pulsing red
    float pulse = sin(time * 2.0) * 0.3 + 0.7;
    vec3 titleColor = vec3(1.0, 0.2, 0.1) * pulse;
    
    // Add glow effect
    float glow = titles * 0.6;
    vec3 glowColor = vec3(1.0, 0.0, 0.0) * glow;
    
    // Blinking "PRESS ANY KEY" indicator
    float blink = sin(time * 3.0) * 0.5 + 0.5;
    float promptY = -0.5;
    
    // Simple line as prompt indicator
    float promptLine = 0.0;
    if (uv.y > promptY - 0.015 && uv.y < promptY + 0.015) {
        if (abs(uv.x) < 0.25) {
            promptLine = 1.0 - abs(uv.x) / 0.25;
        }
    }
    
    // Combine everything
    vec3 finalColor = bgColor;
    finalColor += titleColor * titles;
    finalColor += glowColor;
    finalColor += vec3(0.8, 0.4, 0.2) * promptLine * blink;  // Orange prompt
    
    // Vignette effect (stronger for game over)
    float vignette = 1.0 - length(uv) * 0.6;
    finalColor *= vignette;
    
    // Add screen shake effect based on time (first second)
    float shake = max(0.0, 1.0 - time) * 0.1;
    
    FragColor = vec4(finalColor, 1.0);
}
