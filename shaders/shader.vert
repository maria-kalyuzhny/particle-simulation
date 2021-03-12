#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables
uniform mat4 viewProj;
uniform mat4 view;
uniform mat4 model;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. 
out vec3 fragNormal;
out vec3 normalC;
out vec3 posC;

void main()
{
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = viewProj * model * vec4(position, 1.0);

    // for shading
	fragNormal = vec3(model * vec4(normal, 0));

	// normal in camera space
	normalC = normalize(mat3(inverse(transpose(view * model)))*normal);
	posC = vec3(view * model*vec4(position, 1.0));
}