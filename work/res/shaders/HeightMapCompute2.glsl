#version 430 core

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;
// layout(rgba32f, binding = 0) uniform image2D imgOutput;

struct waterMeshVertex {
    vec4 pos;
    vec4 normal;
    vec4 color;
};

struct cellParams {
    float velocity;
    float height;
    float terrainHeight;
};

// Binding points
layout (std430, binding = 0) buffer WorldGrid {
    cellParams gridData[]; // Your 3D grid data flattened into a 1D array
};

layout (std430, binding = 1) buffer VertexBuffer {
    waterMeshVertex vertexPositions[]; // VBO to store vertex positions
};

layout (binding = 2, offset = 0) uniform atomic_uint vertexCount; // Atomic counter for vertex count




layout (location = 0) uniform float gridWidth;
uniform int gridCountX;
uniform int gridCountY;
uniform float time;
uniform bool Pulse;
uniform float Dampening;

void main() {
    float c = (0.8 * gridWidth)/time;
    uvec2 idx = gl_GlobalInvocationID.xy;
    
    gridData[idx.y * gridCountX + idx.x ].velocity = gridData[idx.y * gridCountX + idx.x ].velocity * Dampening;

    gridData[idx.y * gridCountX + idx.x ].height = gridData[idx.y * gridCountX + idx.x ].height + time * gridData[idx.y * gridCountX + idx.x ].velocity;
    
    if (gridData[idx.y * gridCountX + idx.x ].height > gridData[idx.y * gridCountX + idx.x ].terrainHeight) {

        vertexPositions[idx.y * gridCountX + idx.x ].pos = vec4(gridWidth * idx.x, gridWidth * idx.y, gridData[idx.y * gridCountX + idx.x ].height , 1.0f);
        
        vertexPositions[idx.y * gridCountX + idx.x ].color = vec4(0.0f, 1.0f, 1.0f, 0.4f);
    } else {
        vertexPositions[idx.y * gridCountX + idx.x ].pos = vec4(gridWidth * idx.x, gridWidth * idx.y, gridData[idx.y * gridCountX + idx.x ].terrainHeight , 1.0f);
        vertexPositions[idx.y * gridCountX + idx.x ].color = vec4(0.66f, 0.3f, 0.0f, 1.0f);

    }

}