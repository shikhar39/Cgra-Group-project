#version 430 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec3 aBitangent;
layout(location = 4) in vec2 aTexCoord;
struct waterMeshVertex {
    vec4 pos;
    vec4 normal;
    vec4 color;
};

layout (std430, binding = 1) buffer VertexBuffer {
    waterMeshVertex vertexPositions[]; // VBO to store vertex positions
};
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;


out VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoord;
    vec4 color;
} v_out;
	
void main()
{
    vec3 position = vertexPositions[gl_InstanceID].pos.xyz + aPosition;
    v_out.position = (uViewMatrix * vec4(position, 1.0)).xyz;
    v_out.normal = normalize((uViewMatrix * vec4(aNormal, 0)).xyz);
    v_out.textureCoord = aTexCoord;
    v_out.color = vertexPositions[gl_InstanceID].color;
    gl_Position = uProjectionMatrix * uViewMatrix * vec4(position, 1.0);
}