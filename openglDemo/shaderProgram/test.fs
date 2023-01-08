
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float ratio;

void main()
{
    //FragColor =  mix(texture(texture0, TexCoord), texture(texture1, TexCoord), ratio)* vec4(ourColor, 1.0f);
    FragColor =  mix(texture(texture0, TexCoord), texture(texture1, TexCoord), ratio);
}
