#version 410

layout (location = 0) in vec3 vtxPosition;
layout(location = 2) in vec2 texCoords;


out vec3 normals;
out vec2 textureCoordinates;

uniform mat4 proj_mat, view_mat, model_mat, model_player;
uniform int isPlayer;


void main() {

    mat4 model_matrix;
    textureCoordinates = texCoords;
    

    if (isPlayer == 1)
    {
        model_matrix = model_player;
    }else
    {
        model_matrix = model_mat;
    }

    mat4 MVP = proj_mat * view_mat * model_matrix; // Calculate Model-View-Projection matrix
    gl_Position = MVP * vec4(vtxPosition, 1.0);; // Transform the vertex position
}