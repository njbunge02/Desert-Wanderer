#version 410


// Define uniform variables
uniform vec3 lightColor;
uniform vec3 lightDirection;
uniform float ambience;
uniform sampler2D textureSampler;
uniform float specularHigh;

uniform int diffuseOn;
uniform int specularOn;
uniform int textureOn;


in vec3 normals;
in vec2 textureCoordinates;


out vec4 fragColor;

void main() {

    vec4 texColor = texture(textureSampler, textureCoordinates);

 
    fragColor = vec4(texColor.rgb, 1.0);
}
