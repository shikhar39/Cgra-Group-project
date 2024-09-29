#version 430 core
out vec4 FragColor;
uniform mat4 uViewMatrix;
uniform vec3 ucamPos;
// viewspace data (this must match the output of the fragment shader)
in VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoord;
} f_in;
	
void main()
{             
    float ambientStrength = 0.3f;
	float diffuseStrength = 0.6f;
	float specularStrength = 0.2f;
	float shininess = 8.0f;

	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 lightDir = (uViewMatrix * vec4(normalize(vec3(-1.0f, -1.0f, -1.0f)), 0.0f)).xyz;

	//Ambient Light
	float ambient = ambientStrength;

	//Diffuse Light
	vec3 norm = normalize(f_in.normal);
    float diff = max(dot(norm, -lightDir), 0.0f);
    float diffuse = (diff * diffuseStrength);

	//Specular Light
	vec3 reflected = reflect(lightDir, norm);
	float specular = specularStrength * pow(max(dot(reflected, normalize(ucamPos-f_in.position)), 0.0f), shininess);

	vec3 result = vec3(1.f, 1.f, 0.f) * lightColor * (ambient + diffuse + specular);
	FragColor = vec4(result, 0.2f);
}