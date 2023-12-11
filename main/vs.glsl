#version 410

layout (location = 0) in vec3 vtxPosition;
layout (location = 1) in vec3 norms;
layout(location = 2) in vec2 texCoords;


out vec3 normals;
out vec2 textureCoordinates;

uniform mat4 proj_mat, view_mat, model_mat;


void main() {

    normals = norms;
    textureCoordinates = texCoords;

    mat4 MVP = proj_mat * view_mat * model_mat; // Calculate Model-View-Projection matrix
    gl_Position = MVP * vec4(vtxPosition, 1.0); // Transform the vertex position
}