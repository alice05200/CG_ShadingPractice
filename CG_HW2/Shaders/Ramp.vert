#version 400

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightPosition;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 vertTexCoord;

out vec2 fragTexCoord;
out vec3 fragPos;
out vec3 fragNormal;

void main() {
    // Pass the tex coord straight through to the fragment shader
    fragTexCoord = vertTexCoord;
    
    // Apply all matrix transformations to vert
	fragPos = vec3(modelMatrix * vec4(vert, 1.0f));
	fragNormal = normalize(mat3(transpose(inverse(modelMatrix))) * norm);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vert, 1);
	//normal = vec3(view * model) * norm;
}