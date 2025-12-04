#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform vec3 textColor;

void main()
{
    // Simple colored quad for text
    FragColor = vec4(textColor, 1.0);
}
