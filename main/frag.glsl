#version 410

uniform sampler2D textureSampler;

uniform int textureOn;


in vec3 normals;
in vec2 textureCoordinates;

out vec4 fragColor;

void main() {

    vec3 result;

    if (textureOn == 1)
    {
        vec4 texColor = texture(textureSampler, textureCoordinates);
        fragColor = vec4(texColor.rgb, 1.0);

    }else
    {
       fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0);
    }

    
}
