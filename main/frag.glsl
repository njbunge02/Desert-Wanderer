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

    float frameHeight = 1.0 / ny_frames; // Height of one frame
    vec2 frameOffset = vec2(0.0f, uv_y); // Since there is only one column, the x offset is 0
    vec2 texCoord = vec2(textureCoordinates.x, textureCoordinates.y * frameHeight) + vec2(0.0f, frameOffset.y);
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
