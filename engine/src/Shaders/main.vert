#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPosition;

void main()
{
    Normal = mat3(transpose(inverse(transform))) * aNormal;
    TexCoords = aTexCoord;
    FragPosition = vec3(transform * vec4(aPos, 1.0));

    gl_Position = projection * view * vec4(FragPosition, 1.0);
}