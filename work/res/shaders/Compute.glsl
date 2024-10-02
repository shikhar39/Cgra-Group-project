#version 430 core

layout (local_size_x = 100, local_size_y = 1, local_size_z = 1) in;
// layout(rgba32f, binding = 0) uniform image2D imgOutput;

struct waterMeshVertex {
    vec4 pos;
    vec4 normal;
    vec4 color;
};

struct voxel {
    ivec3 index;
    int type;
};

const int TERRAIN = 0;
const int TALL_CELL = 1;
const int REG_CELL = 2;
const int AIR = 3;


// Binding points
layout (std430, binding = 0) buffer WorldGrid {
    voxel gridData[]; // Your 3D grid data flattened into a 1D array
};

layout (std430, binding = 1) buffer VertexBuffer {
    waterMeshVertex vertexPositions[]; // VBO to store vertex positions
};

layout (binding = 2, offset = 0) uniform atomic_uint vertexCount; // Atomic counter for vertex count




layout (location = 0) uniform float gridWidth;
uniform float time;


void main() {
    voxel vox = gridData[gl_GlobalInvocationID.x];
    vec4 position = vec4(gridWidth * vox.index, 1.0f);
    uint vertexIndex = atomicCounterIncrement(vertexCount);
    vertexPositions[vertexIndex].pos = position;
    if(vox.type == TERRAIN) {
        vertexPositions[vertexIndex].color = vec4(0.66f, 0.3f, 0.0f, 1.0f);
    } else if (vox.type == TALL_CELL) {
        vertexPositions[vertexIndex].color = vec4(0.0f, 1.0f, 1.0f, 0.4f);
    }

    /*
    vec3 a = vec3(0.0f, -9.8f, 0.0f);
    particles[gl_GlobalInvocationID.x].position.xyz = particles[gl_GlobalInvocationID.x].position.xyz + particles[gl_GlobalInvocationID.x].velocity.xyz * t + 0.5 * a * t * t;
    particles[gl_GlobalInvocationID.x].velocity.xyz = particles[gl_GlobalInvocationID.x].velocity.xyz + a * t;

    // positions[gl_GlobalInvocationID.x] = positions[gl_GlobalInvocationID.x] + t * speed * value;
    if(particles[gl_GlobalInvocationID.x].position.y < 0.f) {
        particles[gl_GlobalInvocationID.x].velocity.y = abs(particles[gl_GlobalInvocationID.x].velocity.y);
    }
    */

}