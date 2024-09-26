#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
struct ParticleData {
    vec4 position;  // 4D position
    vec4 velocity;  // 4D velocity
};

layout(std430, binding = 0) buffer PositionBuffer {
    ParticleData particles[];
};
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;


out vec2 TexCoords;
	
void main()
{
    vec4 instancePosition = particles[gl_InstanceID].position;
    vec3 position = vec3(instancePosition) * 2 + aPos;
    TexCoords = aTexCoords;
    gl_Position = uProjectionMatrix * uViewMatrix * vec4(position, 1.0);
}