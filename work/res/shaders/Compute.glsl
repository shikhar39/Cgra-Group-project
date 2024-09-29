#version 430 core

layout (local_size_x = 10, local_size_y = 1, local_size_z = 1) in;
// layout(rgba32f, binding = 0) uniform image2D imgOutput;

struct ParticleData {
    vec4 position;  // 4D position
    vec4 velocity;  // 4D velocity
};

layout(std430, binding = 0) buffer PositionBuffer {
    ParticleData particles[];
};
layout (location = 0) uniform float t;                 /** Time */
	
void main() {
    vec3 a = vec3(0.0f, -9.8f, 0.0f);
    particles[gl_GlobalInvocationID.x].position.xyz = particles[gl_GlobalInvocationID.x].position.xyz + particles[gl_GlobalInvocationID.x].velocity.xyz * t + 0.5 * a * t * t;
    particles[gl_GlobalInvocationID.x].velocity.xyz = particles[gl_GlobalInvocationID.x].velocity.xyz + a * t;

    // positions[gl_GlobalInvocationID.x] = positions[gl_GlobalInvocationID.x] + t * speed * value;
    if(particles[gl_GlobalInvocationID.x].position.y < 0.f) {
        particles[gl_GlobalInvocationID.x].velocity.y = abs(particles[gl_GlobalInvocationID.x].velocity.y);
    }

}