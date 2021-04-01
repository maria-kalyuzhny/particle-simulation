#version 330 core

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

in vec3 fragNormal;
in vec3 normalC;
in vec3 posC;

// uniforms used for lighting
uniform vec3 AmbientColor = vec3(1.0,1.0,0.2);
uniform vec3 AmbientCube = vec3(0.0,0.1,0.2);
uniform vec3 LightDirection = normalize(vec3(1, 5, 2));
uniform vec3 LightPos = vec3(0, 3, 0);
uniform vec3 LightColor = vec3(1.0,1.0,1.0);
uniform vec3 LightDirection2 = normalize(vec3(-4, -3, 1));
//uniform vec3 LightPos2 = vec3(-4, -3, 1);
uniform vec3 LightPos2 = vec3(-4, 3, 1);
uniform vec3 LightColor2 = vec3(0.2,0.7,1.0);
uniform vec3 DiffuseColor;	// passed in from c++ side NOTE: you can also set the value here and then remove 
							// color from the c++ side

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	vec3 L = normalize(LightPos-posC);
	vec3 Atten = 20*LightColor/(distance(LightPos,posC));
	vec3 DiffuseNew = DiffuseColor*clamp(dot(L,normalC),0,1);

	vec3 L2 = normalize(LightPos2-posC);
	vec3 Atten2 = 7*LightColor2/(distance(LightPos2,posC));
	vec3 DiffuseNew2 = DiffuseColor*clamp(dot(L2,normalC),0,1);

	// fragColor = vec4(AmbientColor+Atten*(DiffuseNew),1); //+Atten2*(DiffuseNew2),1);

	// normal shading for debug
	// fragColor = vec4(fragNormal*0.5f + 0.5f,1);
	fragColor = vec4(DiffuseColor,1);

	//quick hack to set cube color differently
	if (DiffuseColor == vec3(0.123)){
		fragColor = vec4(AmbientCube+Atten*(DiffuseNew),1);
	}
}