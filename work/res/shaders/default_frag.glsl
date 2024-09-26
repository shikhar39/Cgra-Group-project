#version 430 core
out vec4 FragColor;
	
in vec2 TexCoords;
	

	
void main()
{             
    vec3 texCol = vec3(1.0f, 0.0f, 0.0f);
    FragColor = vec4(texCol, 1.0);
}