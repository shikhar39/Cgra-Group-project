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

void main() {
    float c = (0.95 * gridWidth)/time;
    uvec2 idx = gl_GlobalInvocationID.xy;
    if ( idx.x == 20 && idx.y == 20 && Pulse) {
        gridData[idx.y * gridCountX + idx.x ].height = gridData[idx.y * gridCountX + idx.x ].height + 0.4;
    }
    uvec2 n1, n2, n3, n4;
    if (idx.x != 0) { n1 = uvec2(idx.x - 1, idx.y); } else { n1 = uvec2(idx.x, idx.y); }
    if (idx.y != gridCountY - 1) { n2 = uvec2(idx.x , idx.y + 1); } else { n2 = uvec2(idx.x, idx.y); }
    if (idx.x != gridCountX - 1) { n3 = uvec2(idx.x + 1, idx.y); } else { n3 = uvec2(idx.x, idx.y); }
    if (idx.y != 0) { n4 = uvec2(idx.x, idx.y - 1); } else { n4 = uvec2(idx.x, idx.y); }

    float acceleration = 0.5 * (gridData[n1.y * gridCountX + n1.x ].height
                        + gridData[n2.y * gridCountX + n2.x ].height
                        + gridData[n3.y * gridCountX + n3.x ].height
                        + gridData[n4.y * gridCountX + n4.x ].height
                        - 4 * gridData[idx.y * gridCountX + idx.x ].height) * c * c/ (gridWidth * gridWidth);

    gridData[idx.y * gridCountX + idx.x ].velocity = gridData[idx.y * gridCountX + idx.x ].velocity + time * acceleration;

    gridData[idx.y * gridCountX + idx.x ].height = gridData[idx.y * gridCountX + idx.x ].height + time * gridData[idx.y * gridCountX + idx.x ].velocity;
    

    vertexPositions[idx.y * gridCountX + idx.x ].pos = vec4(gridWidth * idx.x, gridWidth * idx.y, gridData[idx.y * gridCountX + idx.x ].height , 1.0f);
    
    vertexPositions[idx.y * gridCountX + idx.x ].color = vec4(0.0f, 1.0f, 1.0f, 0.4f);

}