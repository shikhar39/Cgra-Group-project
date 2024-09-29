#version 430 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec3 aBitangent;
layout(location = 4) in vec2 aTexCoord;
struct ParticleData {
    vec4 position;  // 4D position
    vec4 velocity;  // 4D velocity
};

layout(std430, binding = 0) buffer PositionBuffer {
    ParticleData particles[];
};
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;


out VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoord;
} v_out;
	
void main()
{
    vec3 position = particles[gl_InstanceID].position.xyz + aPosition;
    v_out.position = (uViewMatrix * vec4(position, 1.0)).xyz;
    v_out.normal = normalize((uViewMatrix * vec4(aNormal, 0)).xyz);
    v_out.textureCoord = aTexCoord;
    gl_Position = uProjectionMatrix * uViewMatrix * vec4(position, 1.0);
}