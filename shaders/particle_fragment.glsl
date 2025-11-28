#version 120

varying vec4 ParticleColor;

void main()
{
    // Create circular particles
    vec2 coord = gl_PointCoord - vec2(0.5);
    if(length(coord) > 0.5)
        discard;
    
    gl_FragColor = ParticleColor;
}
