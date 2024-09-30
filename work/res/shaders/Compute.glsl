#version 430 core

layout (local_size_x = 10, local_size_y = 1, local_size_z = 1) in;
// layout(rgba32f, binding = 0) uniform image2D imgOutput;

struct waterMeshVertex {
    vec4 pos;
    vec4 normal;
};

struct voxel {
    ivec3 index;
    int type;
};
const int TERRAIN = 0,
const int TALL_CELL = 1,
const int REG_CELL = 2,
const int AIR = 3

// Binding points
layout (std430, binding = 0) buffer VertexBuffer {
    waterMeshVertex vertexPositions[]; // VBO to store vertex positions
};

layout (std430, binding = 2) buffer WorldGrid {
    int gridData[]; // Your 3D grid data flattened into a 1D array
};


layout (binding = 1, offset = 0) uniform atomic_uint vertexCount; // Atomic counter for vertex count


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