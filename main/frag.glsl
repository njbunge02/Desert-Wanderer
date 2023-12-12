#version 410

uniform sampler2D textureSampler;

uniform int textureNum;

uniform float ny_frames;
uniform float nx_frames;
uniform float uv_x;
uniform float uv_y;
uniform int reverse;

in vec3 normals;
in vec2 textureCoordinates;

out vec4 fragColor;

void main() {

    vec3 result;

  

    if (textureNum == 0)
    {
     float frameXHeight = 1.0f / nx_frames;
    float frameYHeight = 1.0f / ny_frames;
    vec2 frameOffset = vec2(uv_x * frameXHeight, uv_y * frameYHeight); 
    vec2 texCoord;

    if (reverse == 0)
        texCoord = vec2(textureCoordinates.x * frameXHeight, textureCoordinates.y * frameYHeight) + frameOffset;
    else
        texCoord = vec2((1.0 - textureCoordinates.x) * frameXHeight, textureCoordinates.y * frameYHeight) + frameOffset;

    fragColor = texture(textureSampler, texCoord);

    }else if (textureNum == 1) //floor and platform
    {
        vec2 modifiedTexCoord = vec2(textureCoordinates.x * 5, textureCoordinates.y);
       fragColor = texture(textureSampler, modifiedTexCoord);
    }

}
