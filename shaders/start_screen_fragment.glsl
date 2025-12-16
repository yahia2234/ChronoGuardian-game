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

// Simple 7-segment digit display
float drawDigit(vec2 uv, int d, float size) {
    float result = 0.0;
    float t = size * 0.08;  // line thickness
    
    // Scale UV to digit space
    uv = uv / size;
    
    // Segment positions (normalized 0-1 space)
    // Segments: top, top-left, top-right, middle, bottom-left, bottom-right, bottom
    
    bool segments[7];
    
    // Define which segments are on for each digit
    if (d == 0) { segments[0]=true; segments[1]=true; segments[2]=true; segments[3]=false; segments[4]=true; segments[5]=true; segments[6]=true; }
    else if (d == 1) { segments[0]=false; segments[1]=false; segments[2]=true; segments[3]=false; segments[4]=false; segments[5]=true; segments[6]=false; }
    else if (d == 2) { segments[0]=true; segments[1]=false; segments[2]=true; segments[3]=true; segments[4]=true; segments[5]=false; segments[6]=true; }
    else if (d == 3) { segments[0]=true; segments[1]=false; segments[2]=true; segments[3]=true; segments[4]=false; segments[5]=true; segments[6]=true; }
    else if (d == 4) { segments[0]=false; segments[1]=true; segments[2]=true; segments[3]=true; segments[4]=false; segments[5]=true; segments[6]=false; }
    else if (d == 5) { segments[0]=true; segments[1]=true; segments[2]=false; segments[3]=true; segments[4]=false; segments[5]=true; segments[6]=true; }
    else if (d == 6) { segments[0]=true; segments[1]=true; segments[2]=false; segments[3]=true; segments[4]=true; segments[5]=true; segments[6]=true; }
    else if (d == 7) { segments[0]=true; segments[1]=false; segments[2]=true; segments[3]=false; segments[4]=false; segments[5]=true; segments[6]=false; }
    else if (d == 8) { segments[0]=true; segments[1]=true; segments[2]=true; segments[3]=true; segments[4]=true; segments[5]=true; segments[6]=true; }
    else if (d == 9) { segments[0]=true; segments[1]=true; segments[2]=true; segments[3]=true; segments[4]=false; segments[5]=true; segments[6]=true; }
    else { for(int i=0; i<7; i++) segments[i] = false; }
    
    float w = 0.6;  // width
    float h = 1.0;  // height
    float m = 0.1;  // margin
    
    // Top
    if (segments[0]) result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m, h-m), t));
    // Top-left
    if (segments[1]) result = max(result, drawLine(uv, vec2(m, h/2), vec2(m, h-m), t));
    // Top-right
    if (segments[2]) result = max(result, drawLine(uv, vec2(w-m, h/2), vec2(w-m, h-m), t));
    // Middle
    if (segments[3]) result = max(result, drawLine(uv, vec2(m, h/2), vec2(w-m, h/2), t));
    // Bottom-left
    if (segments[4]) result = max(result, drawLine(uv, vec2(m, m), vec2(m, h/2), t));
    // Bottom-right
    if (segments[5]) result = max(result, drawLine(uv, vec2(w-m, m), vec2(w-m, h/2), t));
    // Bottom
    if (segments[6]) result = max(result, drawLine(uv, vec2(m, m), vec2(w-m, m), t));
    
    return result;
}

// Draw letter C
float drawC(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m, h-m), t));  // top
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));      // left
    result = max(result, drawLine(uv, vec2(m, m), vec2(w-m, m), t));      // bottom
    
    return result;
}

// Draw letter H
float drawH(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));        // left
    result = max(result, drawLine(uv, vec2(w-m, m), vec2(w-m, h-m), t));    // right
    result = max(result, drawLine(uv, vec2(m, h/2), vec2(w-m, h/2), t));    // middle
    
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

// Draw letter D
float drawD(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(m, m), vec2(m, h-m), t));          // left
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m*2, h-m), t));    // top
    result = max(result, drawLine(uv, vec2(m, m), vec2(w-m*2, m), t));        // bottom
    result = max(result, drawLine(uv, vec2(w-m*2, h-m), vec2(w-m, h-m*2), t)); // top-right curve
    result = max(result, drawLine(uv, vec2(w-m, h-m*2), vec2(w-m, m+m), t));  // right
    result = max(result, drawLine(uv, vec2(w-m*2, m), vec2(w-m, m+m), t));    // bottom-right curve
    
    return result;
}

// Draw letter I
float drawI(vec2 uv, float size) {
    uv = uv / size;
    float result = 0.0;
    float t = 0.08;
    float w = 0.6, h = 1.0, m = 0.1;
    
    result = max(result, drawLine(uv, vec2(w/2, m), vec2(w/2, h-m), t));  // center vertical
    result = max(result, drawLine(uv, vec2(m, h-m), vec2(w-m, h-m), t));  // top
    result = max(result, drawLine(uv, vec2(m, m), vec2(w-m, m), t));      // bottom
    
    return result;
}

// Draw the title "CHRONO"
float drawCHRONO(vec2 uv, float size, float startX) {
    float result = 0.0;
    float letterWidth = size * 0.7;
    
    result = max(result, drawC(uv - vec2(startX + letterWidth * 0.0, 0.0), size));
    result = max(result, drawH(uv - vec2(startX + letterWidth * 1.0, 0.0), size));
    result = max(result, drawR(uv - vec2(startX + letterWidth * 2.0, 0.0), size));
    result = max(result, drawO(uv - vec2(startX + letterWidth * 3.0, 0.0), size));
    result = max(result, drawN(uv - vec2(startX + letterWidth * 4.0, 0.0), size));
    result = max(result, drawO(uv - vec2(startX + letterWidth * 5.0, 0.0), size));
    
    return result;
}

// Draw the title "GUARDIAN"
float drawGUARDIAN(vec2 uv, float size, float startX) {
    float result = 0.0;
    float letterWidth = size * 0.7;
    
    result = max(result, drawG(uv - vec2(startX + letterWidth * 0.0, 0.0), size));
    result = max(result, drawU(uv - vec2(startX + letterWidth * 1.0, 0.0), size));
    result = max(result, drawA(uv - vec2(startX + letterWidth * 2.0, 0.0), size));
    result = max(result, drawR(uv - vec2(startX + letterWidth * 3.0, 0.0), size));
    result = max(result, drawD(uv - vec2(startX + letterWidth * 4.0, 0.0), size));
    result = max(result, drawI(uv - vec2(startX + letterWidth * 5.0, 0.0), size));
    result = max(result, drawA(uv - vec2(startX + letterWidth * 6.0, 0.0), size));
    result = max(result, drawN(uv - vec2(startX + letterWidth * 7.0, 0.0), size));
    
    return result;
}

void main() {
    vec2 uv = FragPos;
    
    // Animated background
    vec2 noiseCoord = uv * 3.0 + vec2(time * 0.1, time * 0.05);
    float n = fbm(noiseCoord);
    
    // Deep blue/purple gradient background
    vec3 bgColor1 = vec3(0.05, 0.02, 0.15);  // Deep purple
    vec3 bgColor2 = vec3(0.02, 0.08, 0.2);   // Deep blue
    vec3 bgColor = mix(bgColor1, bgColor2, uv.y * 0.5 + 0.5 + n * 0.2);
    
    // Add some stars
    float stars = 0.0;
    for (int i = 0; i < 3; i++) {
        vec2 starUV = uv * (10.0 + float(i) * 5.0);
        float starNoise = hash(floor(starUV));
        if (starNoise > 0.97) {
            float twinkle = sin(time * 3.0 + starNoise * 100.0) * 0.5 + 0.5;
            stars = max(stars, twinkle * (1.0 - length(fract(starUV) - 0.5) * 4.0));
        }
    }
    bgColor += vec3(stars) * 0.8;
    
    // Draw title "CHRONO"
    float titleSize = 0.18;
    float chronoWidth = titleSize * 0.7 * 6.0;
    float chronoStartX = -chronoWidth / 2.0;
    float title1 = drawCHRONO(uv - vec2(0.0, 0.25), titleSize, chronoStartX);
    
    // Draw title "GUARDIAN"
    float guardianWidth = titleSize * 0.7 * 8.0;
    float guardianStartX = -guardianWidth / 2.0;
    float title2 = drawGUARDIAN(uv - vec2(0.0, 0.0), titleSize, guardianStartX);
    
    // Combine titles with glow effect
    float titles = max(title1, title2);
    
    // Title color with time-based animation
    vec3 titleColor1 = vec3(0.3, 0.7, 1.0);   // Cyan
    vec3 titleColor2 = vec3(0.8, 0.4, 1.0);   // Purple
    vec3 titleColor = mix(titleColor1, titleColor2, sin(time * 0.5) * 0.5 + 0.5);
    
    // Add glow
    float glow = titles * 0.5;
    vec3 glowColor = titleColor * glow * 2.0;
    
    // Blinking "PRESS ANY KEY" text indicator
    float blink = sin(time * 3.0) * 0.5 + 0.5;
    float promptY = -0.5;
    
    // Simple line as prompt indicator
    float promptLine = 0.0;
    if (uv.y > promptY - 0.02 && uv.y < promptY + 0.02) {
        if (abs(uv.x) < 0.3) {
            promptLine = 1.0 - abs(uv.x) / 0.3;
        }
    }
    
    // Combine everything
    vec3 finalColor = bgColor;
    finalColor += titleColor * titles;
    finalColor += glowColor;
    finalColor += vec3(0.5, 0.8, 1.0) * promptLine * blink;
    
    // Vignette effect
    float vignette = 1.0 - length(uv) * 0.5;
    finalColor *= vignette;
    
    FragColor = vec4(finalColor, 1.0);
}
