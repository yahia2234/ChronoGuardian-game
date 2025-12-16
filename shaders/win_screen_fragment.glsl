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

// Draw letter Y
float drawY(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w/2, h/2), t));      // left diagonal
    result = max(result, drawLine(uv, vec2(w-m, h-m), vec2(w/2, h/2), t));    // right diagonal
    result = max(result, drawLine(uv, vec2(w/2, h/2), vec2(w/2, m), t));      // stem
    
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

// Draw letter U
float drawU(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));      // left
    result = max(result, drawLine(uv, vec2(w-m, m), vec2(w-m, h-m), t));  // right
    result = max(result, drawLine(uv, vec2(m, m), vec2(w-m, m), t));      // bottom
    
    return result;
}

// Draw letter W
float drawW(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.7, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(m+0.1, m), t));           // left
    result = max(result, drawLine(uv, vec2(m+0.1, m), vec2(w/2, h/2), t));         // left-center
    result = max(result, drawLine(uv, vec2(w/2, h/2), vec2(w-m-0.1, m), t));       // right-center
    result = max(result, drawLine(uv, vec2(w-m-0.1, m), vec2(w-m, h-m), t));       // right
    
    return result;
}

// Draw letter I
float drawI(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.4, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(w/2, m), vec2(w/2, h-m), t));  // center vertical
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m, h-m), t));  // top
    result = max(result, drawLine(uv, vec2(m, m), vec2(w-m, m), t));      // bottom
    
    return result;
}

// Draw letter N
float drawN(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));          // left
    result = max(result, drawLine(uv, vec2(w-m, m), vec2(w-m, h-m), t));      // right
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m, m), t));        // diagonal
    
    return result;
}

// Draw exclamation mark
float drawExclaim(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.3, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(w/2, h*0.35), vec2(w/2, h-m), t));  // vertical line
    
    // Dot at bottom
    float dotDist = length(uv - vec2(w/2, m + 0.08));
    result = max(result, 1.0 - smoothstep(0.0, 0.08, dotDist));
    
    return result;
}

// Draw "YOU"
float drawYOU(vec2 uv, float size, float startX) {
    float result = 0.0;
    float letterWidth = size * 0.7;
    
    result = max(result, drawY(uv - vec2(startX + letterWidth * 0.0, 0.0), size));
    result = max(result, drawO(uv - vec2(startX + letterWidth * 1.0, 0.0), size));
    result = max(result, drawU(uv - vec2(startX + letterWidth * 2.0, 0.0), size));
    
    return result;
}

// Draw "WIN!"
float drawWIN(vec2 uv, float size, float startX) {
    float result = 0.0;
    float letterWidth = size * 0.7;
    
    result = max(result, drawW(uv - vec2(startX + letterWidth * 0.0, 0.0), size));
    result = max(result, drawI(uv - vec2(startX + letterWidth * 1.0, 0.0), size * 0.7));
    result = max(result, drawN(uv - vec2(startX + letterWidth * 1.5, 0.0), size));
    result = max(result, drawExclaim(uv - vec2(startX + letterWidth * 2.4, 0.0), size));
    
    return result;
}

void main() {
    vec2 uv = FragPos;
    
    // Animated golden/celebratory background
    vec2 noiseCoord = uv * 2.0 + vec2(time * 0.08, time * 0.05);
    float n = fbm(noiseCoord);
    
    // Deep blue to purple gradient background
    vec3 bgColor1 = vec3(0.05, 0.02, 0.15);  // Deep purple
    vec3 bgColor2 = vec3(0.02, 0.05, 0.12);  // Deep blue
    vec3 bgColor = mix(bgColor1, bgColor2, uv.y * 0.5 + 0.5 + n * 0.2);
    
    // Add celebratory sparkles/stars
    float sparkles = 0.0;
    for (int i = 0; i < 8; i++) {
        vec2 sparkleUV = uv * (8.0 + float(i) * 4.0);
        sparkleUV.y -= time * (0.3 + float(i) * 0.1);  // Fall downward like confetti
        sparkleUV.x += sin(time * 2.0 + float(i)) * 0.2;  // Sway
        float sparkleNoise = hash(floor(sparkleUV));
        if (sparkleNoise > 0.92) {
            float twinkle = sin(time * 5.0 + sparkleNoise * 80.0) * 0.4 + 0.6;
            sparkles = max(sparkles, twinkle * (1.0 - length(fract(sparkleUV) - 0.5) * 3.5));
        }
    }
    
    // Golden sparkle color
    vec3 sparkleColor = mix(vec3(1.0, 0.85, 0.3), vec3(1.0, 0.5, 0.8), hash(uv * 10.0 + time));
    bgColor += sparkleColor * sparkles * 0.7;
    
    // Draw title "YOU"
    float titleSize = 0.18;
    float youWidth = titleSize * 0.7 * 3.0;
    float youStartX = -youWidth / 2.0;
    float title1 = drawYOU(uv - vec2(0.0, 0.22), titleSize, youStartX);
    
    // Draw title "WIN!"
    float winWidth = titleSize * 0.7 * 3.0;
    float winStartX = -winWidth / 2.0;
    float title2 = drawWIN(uv - vec2(0.0, -0.05), titleSize, winStartX);
    
    // Combine titles
    float titles = max(title1, title2);
    
    // Title color - rainbow/golden shifting
    float colorPhase = time * 0.5;
    vec3 titleColor1 = vec3(1.0, 0.85, 0.2);   // Gold
    vec3 titleColor2 = vec3(0.2, 1.0, 0.5);    // Emerald
    vec3 titleColor3 = vec3(0.4, 0.8, 1.0);    // Cyan
    
    vec3 titleColor = mix(titleColor1, titleColor2, sin(colorPhase) * 0.5 + 0.5);
    titleColor = mix(titleColor, titleColor3, sin(colorPhase * 1.3 + 1.0) * 0.3 + 0.3);
    
    // Pulsing intensity
    float pulse = sin(time * 3.0) * 0.2 + 1.0;
    titleColor *= pulse;
    
    // Add strong glow effect
    float glow = titles * 0.8;
    vec3 glowColor = vec3(1.0, 0.9, 0.5) * glow * 1.5;
    
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
    finalColor += vec3(1.0, 0.9, 0.5) * promptLine * blink;  // Golden prompt
    
    // Slight vignette
    float vignette = 1.0 - length(uv) * 0.3;
    finalColor *= vignette;
    
    FragColor = vec4(finalColor, 1.0);
}
