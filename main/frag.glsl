#version 410

uniform sampler2D textureSampler;

uniform int textureOn;

uniform float ny_frames;
uniform float nx_frames;
uniform float uv_x;
uniform float uv_y;

in vec3 normals;
in vec2 textureCoordinates;

out vec4 fragColor;

void main() {

    vec3 result;

    /*float x = 1.0f / nx_frames;
    float y = 1.0f / ny_frames;
    frag_color = texture(textureSampler, vec2(textureCoordinates.x * x, textureCoordinates.y * y) + vec2(x * uv_x, y * uv_y));*/

    float frameXHeight = 1.0f / nx_frames;
    float frameYHeight = 1.0f / ny_frames;
    vec2 frameOffset = vec2(uv_x * frameXHeight, uv_y * frameYHeight); 
    vec2 texCoord = vec2(textureCoordinates.x * frameXHeight, textureCoordinates.y * frameYHeight) + frameOffset;
    fragColor = texture(textureSampler, texCoord);

    /*if (textureOn == 1)
    {
        vec4 texColor = texture(textureSampler, textureCoordinates);
        fragColor = vec4(texColor.rgb, 1.0);

    }else
    {
       fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0);
    }*/

    
}
